#include "MemoryAllocation.h"
#include "Common.h"
#include "GdbCall.h"
#include "GraphicVariable.h"
#include "LogManager.h"
#include "VisualizationContext.h"

#include <QRegularExpression>

MemoryAllocation::MemoryAllocation(AllocationSegment segment, VisAddress size, VisAddress visualizationAddress)
	: segment(segment)
	, visualizationAddress(visualizationAddress)
	, size(size)
{
	if ( isFreeFragment() )
		name = watchName = "free";
}

MemoryAllocation::~MemoryAllocation()
{
	DELETE_POINTERS_ARRAY(children);
}

bool MemoryAllocation::loadFromGdbVariableObject(const GdbItemTree& tree)
{
	// The tree has the result of creation of a GDB's variable object, eg:

	//	^done,
	//	name="bn_io_stdin",
	//	numchild="1",
	//	value="0x0",
	//	type="char *",
	//	has_more="0"
	watchName = tree.findNodeTextValue("name");
	value = tree.findNodeTextValue("value");
	dataTypeStr = tree.findNodeTextValue("type");
	//numChildren = tree.findNodeTextValue("numchild");
	//hasMore = tree.findNodeTextValue("has_more");

	return ! watchName.isEmpty();
}

bool MemoryAllocation::updateMissingFields(GdbCall* debuggerCall)
{
	// Does GDB/MI offer a method to get data-type size or unroll data types? As a workaround we use
	// C/C++ expressions and normal GDB user interface to get the missing data

	// The size on bytes of this variable in inferior, it may not match the size the Botnu architecture
	GdbItemTree resultSizeof;
	if ( size == 0 && debuggerCall->sendGdbCommand(QString("-data-evaluate-expression sizeof(%1)").arg(name), visMemoryMapper, &resultSizeof) == GDB_ERROR )
		return false;
	size = resultSizeof.findNodeTextValue("value").toULongLong();

	// The address where the variable is allocated in inferior
	GdbItemTree resultAddress;
	if ( inferiorAddress == 0 && debuggerCall->sendGdbCommand(QString("-data-evaluate-expression &%1").arg(name), visMemoryMapper, &resultAddress) == GDB_ERROR )
		return false;

	// The address may have extra text, eg: "14^done,value="0x1000020b8 <i_love_globals>"
	QString addressStr = resultAddress.findNodeTextValue("value");
	int pos = addressStr.indexOf(' ');
	if ( pos > 0 )
		addressStr = addressStr.mid(0, pos);
	inferiorAddress = addressStr.toULongLong(nullptr, 0);
	qCCritical(logApplication, "Address of %s: %zd == %zx", qPrintable(name), inferiorAddress, inferiorAddress);

	// If we can infer the data type, OK, otherwise unroll it using GDB ptype user command
	if ( dataType == typeUnknown )
	{
		if ( ! parseDataTypeStr(dataTypeStr, debuggerCall) )
		{
			// Try to unroll typedefs
			GdbItemTree resultUnrolledDataType;
			if ( debuggerCall->sendGdbCommand(QString("-interpreter-exec console \"ptype /mt %1\"").arg(name), visMemoryMapper, &resultUnrolledDataType ) != GDB_ERROR )
			{
				this->unrolledDataTypeStr = resultUnrolledDataType.getRoot()->getTextValue();
				this->unrolledDataTypeStr.replace("type = ", "");
				this->unrolledDataTypeStr.replace("\n", "");
				parseDataTypeStr( unrolledDataTypeStr, debuggerCall );
			}
		}
	}

	qCCritical(logTemporary).nospace() << getId() << " will be visualized as " << dataType << " from " << dataTypeStr;
	return true;
}

bool MemoryAllocation::parseDataTypeStr(const QString& text, GdbCall* debuggerCall)
{
	// Unnecessary but safer
//	dataTypeStr = dataTypeStr.simplified();

	// Do some basic static analysis to recognize the data type in order to represent this value
	// in visualization
	return parseAtomicDataTypeStr(text)
		|| parseIndirectionDataTypeStr(text)
		|| parseArrayDataTypeStr(text, debuggerCall)
		|| parseCompositeDataTypeStr(text);
}

bool MemoryAllocation::parseAtomicDataTypeStr(const QString& text)
{
	// ToDo: A static code analyzer should be used, instead of simplified regular expressions
	// Atomic data types have the following structure on most cases:

	// ^(const|volatile)?\s*(signed|unsigned)?\s*((short|long long|long)|(char|char16_t|char32_t|wchar_t|bool|int|float|void))+$
	//   1=constVolatile     2=sign              3 4=sizeQualifier        5=dataType

	// Notice that C/C++ allows declarations in any order, eg 'double long const g = 6.67e-11'
	// but GDB reorder them, so we can evaluate them in the GDB's order
	QRegularExpression re("^(const|volatile)?\\s*(signed|unsigned)?\\s*((short|long long|long)|(char|char16_t|char32_t|wchar_t|bool|int|float|double|void))+$");
	QRegularExpressionMatch match = re.match(text);
	if ( ! match.hasMatch() )
		return false;

	// Extract each piece of the declaration
	const QString& constVolatile = match.captured(1).trimmed(); // const|volatile
	const QString& signQualifier = match.captured(2).trimmed(); // signed|unsigned
	const QString& sizeQualifier = match.captured(4).trimmed(); // short|long long|long
	QString dataTypeText = match.captured(5).trimmed(); // char|char16_t|char32_t|wchar_t|bool|int|float|double|void

	// Update atomic value data
	this->isConst = constVolatile == "const";
	this->isVolatile = constVolatile == "volatile";
	this->isSigned = signQualifier != "unsigned";
	this->sizeQualifier = mapSizeQualifier(sizeQualifier);

	// "int" is optional when used preceded by short|long|long long|unsigned
	if ( dataTypeText.isEmpty() && (! isSigned || ! sizeQualifier.isEmpty()) )
		dataTypeText = "int";

	// Map the data type text to the enumeration. This is the most important value to be able to
	// choose the graphical representation of this memory allocation
	this->dataType = mapDataType(dataTypeText);

	// Do some adjustments
	if ( this->dataType == typeFloat && dataTypeText == "double" )
		this->sizeQualifier = sizeLong;

	qCCritical(logTemporary).nospace() << "cv=" << constVolatile << " sign=" << signQualifier << " size=" << sizeQualifier << " dataType=" << dataTypeText;
	return true;
}

SizeQualifier MemoryAllocation::mapSizeQualifier(const QString& text)
{
	if ( text.isEmpty() ) return sizeDefault;
	if ( text == "long" ) return sizeLong;
	if ( text == "long long" ) return sizeLongLong;
	if ( text == "short" ) return sizeShort;

	return sizeDefault;
}

DataType MemoryAllocation::mapDataType(const QString& text)
{
	if ( text == "int" ) return typeInt;
	if ( text == "char" ) return typeChar;
	if ( text == "double" ) return typeFloat;
	if ( text == "bool" ) return typeBool;
	if ( text == "float" ) return typeFloat;
	if ( text == "char16_t" ) return typeInt;
	if ( text == "char32_t" ) return typeInt;
	if ( text == "wchar_t" ) return typeInt;
	if ( text == "void" ) return typeVoid;

	return typeUnknown;
}

bool MemoryAllocation::parseIndirectionDataTypeStr(const QString& text)
{
	// Data types for indirection (pointers and references) have the following structure:

	// ^(.+)\s*(\&|\*)(\s?const)?$
	// 1=pd   2=PRef 3=constancy

	QRegularExpression re("^(.+)\\s*(\\&|\\*)(\\s?const)?$");
	QRegularExpressionMatch match = re.match(text);
	if ( ! match.hasMatch() )
		return false;

	// Extract each piece of the declaration
	const QString& pointedDataTypeStr = match.captured(1).trimmed(); // pointed data type
	const QString& pointerReference = match.captured(2).trimmed(); // *|&
	const QString& pointerConstancy = match.captured(3).trimmed(); // const

	// Set the type according to the symbol used in declaration
	if ( pointerReference == '*' )
		this->dataType = typePointer;
	else if ( pointerReference == '&' )
		this->dataType = typeReference;
	else
		return false;

	// We know we have a pointer or reference, it may be constant
	this->isConst = pointerConstancy == "const";

	// The pointed data type may be atomic or very complex declaration (recursive)
	if ( pointedDataTypeStr.isEmpty() )
	{
		qCCritical(logApplication) << "Pointer to empty data type:" << text;
		return false;
	}
	// For the moment, we create a child MemoryAllocation declaration that will be unrolled later
	// when the pointer/reference will be used to access data
	MemoryAllocation* pointedDataTypeObject = new MemoryAllocation(AllocationSegment::unknown);
	pointedDataTypeObject->parent = this;
	pointedDataTypeObject->dataTypeStr = pointedDataTypeStr;
	children.append( pointedDataTypeObject );

	qCCritical(logApplication) << "Pointer/Reference" << name << "to" << pointedDataTypeStr;
	return true;
}

bool MemoryAllocation::parseArrayDataTypeStr(const QString& text, GdbCall* debuggerCall)
{
	// Compound types can be sliced in two types: arrays and structures (struct, class, union)
	// Arrays always finish with brackets, and its size is known:

	// ^(.+)\s*\[(\d+)\]?$
	//  1=et     2=sz

	QRegularExpression re("^(.+)\\s*\\[(\\d+)\\]?$");
	QRegularExpressionMatch match = re.match(text);
	if ( ! match.hasMatch() )
		return false;

	// Extract each piece of the declaration
	bool ok = false;
	const QString& elementTypeStr = match.captured(1).trimmed(); // element type
	size_t arraySize = match.captured(2).toULongLong(&ok);

	// The type of elements and the size must be known to have a valid array
	if ( ok == false || elementTypeStr.isEmpty() )
	{
		qCCritical(logApplication) << "Invalid array type:" << text;
		return false;
	}

	// We know this is a valid array
	this->dataType = typeArray;

	// We create an array of child MemoryAllocation elements
	children.reserve( arraySize );
	for ( size_t index = 0; index < arraySize; ++index )
	{
		// For each element, we get missing information in order to visualize it
		// ToDo: This can be done updating just one elment, and copying the results to the rest
		MemoryAllocation* arrayElement = new MemoryAllocation(this->segment);
		arrayElement->name = QString("%1[%2]").arg(name).arg(index);
		arrayElement->parent = this;
		arrayElement->dataTypeStr = elementTypeStr;
		children.append( arrayElement );
		arrayElement->updateMissingFields(debuggerCall);
	}

	qCCritical(logApplication) << "Array" << name << ":" << arraySize << "of" << elementTypeStr;
	return true;
}

bool MemoryAllocation::parseCompositeDataTypeStr(const QString& text)
{
	// If this method is called, the other methods failed to recognize the data type for this
	// memory allocation. A final shot is checking for classes, structures and unions
	// These begin with their respective names when unrolling with ptype gdb command

	if ( text.startsWith("class") )
		this->dataType = typeClass;
	else if ( text.startsWith("struct") )
		this->dataType = typeStruct;
	else if ( text.startsWith("union") )
		this->dataType = typeUnion;
	else
		return false;

	// ToDo: unroll structure fields
	qCCritical(logApplication) << "Class/struct/union" << name << "from" << dataTypeStr.mid(0, 10) << "...";

	return false;
}

VisAddress MemoryAllocation::calculateAllocationOffset(const MemoryAllocation* variable, VisAddress frameStartAddress) const
{
	// Only free fragments can allocate variables
	if ( this->isFreeFragment() == false ) return -1;

	// If the variable requires more bytes than the size of this free fragment
	Q_ASSERT(variable);
	if ( variable->size > this->size ) return -2;

	// The offset is calculated by applying word-alignment according to the size of the variable
	VisAddress wordSize = variable->getWordAlignment();
	VisAddress offset = (this->visualizationAddress - frameStartAddress) % wordSize;

	// If offset is 0, we are at alignment boundary, otherwise, we have to move to the next boundary
	// that is, to move the remaining bytes to reach it
	if ( offset > 0 )
		offset = wordSize - offset;

	// After applying word-alignment, the variable may exceed the available space
	if ( offset + variable->size > this->size ) return -3;

	// It seems, the variable can be allocated at this offset
	return offset;
}

void MemoryAllocation::reduceSize(VisAddress bytes)
{
	Q_ASSERT( isFreeFragment() );
	Q_ASSERT(this->size >= bytes);

	this->visualizationAddress += bytes;
	this->size -= bytes;
}

short MemoryAllocation::getWordAlignment() const
{
	// ToDo: depends on the architecture of the botnu file

	// Depends on the type of this variable:
	switch ( dataType )
	{
		case typeUnknown: Q_ASSERT(false); return 1;
		case typeVoid: Q_ASSERT(false); return 1;
		case typeBool: return alignof(bool);
		case typeChar: return alignof(char);
		case typeInt: return alignof(int);
		case typeEnum: return alignof(unsigned);
		case typeBitField: return alignof(unsigned);
		case typeFloat: return alignof(float);
		case typePointer: return alignof(void*);
		case typeReference: return alignof(void*);
		case typeRValue: return alignof(void*);
		case typeFunction: Q_ASSERT(false); return 1;

		// ToDo: Alignment of composite types is the largest alignment of their fields
		case typeArray: return alignof(void*);
		case typeStruct: return alignof(void*);
		case typeClass: return alignof(void*);
		case typeUnion: return alignof(void*);
	}

	Q_ASSERT(false);
	return 1;
}

GraphicVariable* MemoryAllocation::getGraphicVariableFor(VisAddress firstByte, VisAddress lastByte, MemoryRow* memoryRow)
{
	// Check ranges are right
	if ( firstByte < visualizationAddress || firstByte > visualizationAddress + size ) return nullptr;
	if ( lastByte < visualizationAddress || lastByte > visualizationAddress + size ) return nullptr;
	if ( lastByte < firstByte ) return nullptr;

	// Locate the variable at the given bytes, if there is one variable, return it
	for ( int index = 0; index < graphicVariables.count(); ++index )
		if ( graphicVariables[index]->isAllocatedAt(firstByte, lastByte) )
			return graphicVariables[index];

	// The variable does not exist, we create one
	GraphicVariable* graphicVariable = new GraphicVariable(this, firstByte, lastByte, memoryRow);
	graphicVariables.append(graphicVariable);
	return graphicVariable;
}
