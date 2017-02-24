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

	return true;
}

bool MemoryAllocation::parseDataTypeStr()
{
	dataTypeStr = dataTypeStr.simplified();
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

	QRegularExpression re("^(const|volatile)?\\s*(signed|unsigned)?\\s*((short|long long|long)|(char|char16_t|char32_t|wchar_t|bool|int|float|void))+$");
	QRegularExpressionMatch match = re.match(dataTypeStr);
	if ( ! match.hasMatch() )
		return false;

	// Extract each piece of the declaration
	const QString& constVolatile = match.captured(1); // const|volatile
	const QString& signQualifier = match.captured(2); // signed|unsigned
	const QString& sizeQualifier = match.captured(4); // short|long long|long
	const QString& dataTypeText  = match.captured(5); // char|char16_t|char32_t|wchar_t|bool|int|float|void

	qCCritical(logTemporary).nospace() << "cv=" << constVolatile << " sign=" << signQualifier << " size=" << sizeQualifier << " dataType=" << dataTypeText;
	return true;
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
