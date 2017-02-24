#include "MemoryAllocation.h"
#include "GdbCall.h"
#include "LogManager.h"
#include "VisualizationContext.h"

#include <QRegularExpression>

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
		if ( ! parseDataTypeStr() )
		{
			// Try to unroll typedefs
			if ( debuggerCall->sendGdbCommand(QString("-interpreter-exec console \"ptype /mt %1\"").arg(name)) == GDB_ERROR )
				return false;
		}
	}

	qCCritical(logTemporary).nospace() << getId() << " will be visualized as " << dataType << " from " << dataTypeStr;
	return true;
}

bool MemoryAllocation::parseDataTypeStr()
{
	// Unnecessary but safer
	dataTypeStr = dataTypeStr.simplified();

	// Do some basic static analysis to recognize the data type in order to represent this value
	// in visualization
	return parseAtomicDataTypeStr()
		|| parseIndirectionDataTypeStr()
		|| parseArrayDataTypeStr()
		|| parseCompositeDataTypeStr();
}

bool MemoryAllocation::parseAtomicDataTypeStr()
{
	// ToDo: A static code analyzer should be used, instead of simplified regular expressions
	// Atomic data types have the following structure on most cases:

	// ^(const|volatile)?\s*(signed|unsigned)?\s*((short|long long|long)|(char|char16_t|char32_t|wchar_t|bool|int|float|void))+$
	//   1=constVolatile     2=sign              3 4=sizeQualifier        5=dataType

	// Notice that C/C++ allows declarations in any order, eg 'double long const g = 6.67e-11'
	// but GDB reorder them, so we can evaluate them in the GDB's order
	QRegularExpression re("^(const|volatile)?\\s*(signed|unsigned)?\\s*((short|long long|long)|(char|char16_t|char32_t|wchar_t|bool|int|float|double|void))+$");
	QRegularExpressionMatch match = re.match(dataTypeStr);
	if ( ! match.hasMatch() )
		return false;

	// Extract each piece of the declaration
	const QString& constVolatile = match.captured(1); // const|volatile
	const QString& signQualifier = match.captured(2); // signed|unsigned
	const QString& sizeQualifier = match.captured(4); // short|long long|long
	QString dataTypeText = match.captured(5); // char|char16_t|char32_t|wchar_t|bool|int|float|double|void

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

bool MemoryAllocation::parseIndirectionDataTypeStr()
{
	/*
	Indirection
		^(const )?(.+)\s*(\&|\*)( const)?$
	*/
	return false;
}

bool MemoryAllocation::parseArrayDataTypeStr()
{
	/*
	Compound: array
		^(const )?(.+)\s*\[(\d+)\]?$
	*/
	return false;
}

bool MemoryAllocation::parseCompositeDataTypeStr()
{
	return false;
}
