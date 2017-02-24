#include "MemoryMapper.h"
#include "GdbCall.h"
#include "LogManager.h"
#include "VisualizationContext.h"


// MemoryBlock class ---------------------------------------------------------------------------

bool MemoryBlock::loadFromGdbVariableObject(const GdbItemTree& tree)
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

bool MemoryBlock::updateMissingFields(GdbCall* debuggerCall)
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
//		dataType = mapDataTypeStr(dataTypeStr);
		if ( dataType == typeUnknown )
		{
			// Try to unroll typedefs
			if ( debuggerCall->sendGdbCommand(QString("-interpreter-exec console \"ptype /mt %1\"").arg(name)) == GDB_ERROR )
				return false;
		}
	}

	return true;
}



// VariableMapper class ---------------------------------------------------------------------------

MemoryMapper::MemoryMapper(GdbCall* debuggerCall, QObject* parent)
	: QObject(parent)
	, debuggerCall(debuggerCall)
{
}

MemoryMapper::~MemoryMapper()
{
	for ( QHash<QString, MemoryBlock*>::Iterator itr = mapNameMemoryBlock.begin(); itr != mapNameMemoryBlock.end(); ++itr )
		delete itr.value();
}

bool MemoryMapper::createWatch(const QString& name, const QString& watchName, MemoryBlock::WatchType type)
{
	// Create a memory block for the watch and load it from the tree
	MemoryBlock* watch = new MemoryBlock(type);
	watch->name = name;
	watch->watchName = watchName;

	// Add the watch to the maps
	mapNameMemoryBlock.insert( watch->getId(), watch );

	GdbItemTree resultWatch;
	const QString& command = QString("-var-create %1 @ %2").arg(watchName).arg(name);
	if ( debuggerCall->sendGdbCommand(command, visMemoryMapper, &resultWatch) == GDB_ERROR )
	{
		qCCritical(logVisualizator).noquote() << "Failed to set watch for" << name;
		return false;
	}

	// Update the watch from GDB's tree result
	if ( ! watch->loadFromGdbVariableObject(resultWatch) )
	{
		qCCritical(logApplication) << "Invalid watch result:" << resultWatch.buildDescription();
		return false;
	}

	qCDebug(logApplication) << "Watch updated:" << watch->getId() << "referring to" << watch->name;
	return watch->updateMissingFields(debuggerCall);
}
