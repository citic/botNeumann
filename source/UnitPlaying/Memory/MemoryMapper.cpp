#include "MemoryMapper.h"
#include "GdbCall.h"
#include "LogManager.h"
#include "VisualizationContext.h"

MemoryMapper::MemoryMapper(GdbCall* debuggerCall, QObject* parent)
	: QObject(parent)
	, debuggerCall(debuggerCall)
{
}

MemoryMapper::~MemoryMapper()
{
	for ( QHash<QString, MemoryAllocation*>::Iterator itr = mapNameMemoryAllocation.begin(); itr != mapNameMemoryAllocation.end(); ++itr )
		delete itr.value();
}

bool MemoryMapper::createWatch(const QString& name, const QString& watchName, MemoryAllocation::WatchType type)
{
	// Create a memory block for the watch and load it from the tree
	MemoryAllocation* watch = new MemoryAllocation(type);
	watch->name = name;
	watch->watchName = watchName;

	// Add the watch to the maps
	mapNameMemoryAllocation.insert( watch->getId(), watch );

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
