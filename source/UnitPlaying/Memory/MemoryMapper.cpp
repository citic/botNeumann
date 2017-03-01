#include "MemoryMapper.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "GdbCall.h"
#include "HeapSegment.h"
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

MemoryAllocation* MemoryMapper::createWatch(const QString& name, const QString& watchName, AllocationSegment segment, bool shouldAllocate)
{
	// Create a memory block for the watch and load it from the tree
	MemoryAllocation* watch = new MemoryAllocation(segment);
	watch->name = name;
	watch->watchName = watchName;

	// Add the watch to the maps
	mapNameMemoryAllocation.insert( watch->getId(), watch );

	GdbItemTree resultWatch;
	const QString& command = QString("-var-create %1 @ %2").arg(watchName).arg(name);
	if ( debuggerCall->sendGdbCommand(command, visMemoryMapper, &resultWatch) == GDB_ERROR )
	{
		qCCritical(logVisualizator).noquote() << "Failed to set watch for" << name;
		return nullptr;
	}

	// Update the watch from GDB's tree result
	if ( ! watch->loadFromGdbVariableObject(resultWatch) )
	{
		qCCritical(logApplication) << "Invalid watch result:" << resultWatch.buildDescription();
		return nullptr;
	}

	qCDebug(logApplication) << "Watch created:" << watch->getId() << "referring to" << watch->name;
	if ( watch->updateMissingFields(debuggerCall) == false )
		return nullptr;

	// The variable was updated, we can now allocate it in the visualization segments
	if ( shouldAllocate )
		allocate(watch);

	// Success
		return watch;
}

bool MemoryMapper::allocate(MemoryAllocation* memoryAllocation)
{
	// Allocation is done in the segement where the variable belongs
	Q_ASSERT(memoryAllocation);
	switch ( memoryAllocation->segment )
	{
		case AllocationSegment::code:
			// ToDo: some constants are stored in code segment, and they have a memory address
			// but, the way to represent these constants is still pending in botNeumann
			qCCritical(logNotImplemented) << "Allocate memory in code segment (constant?)" << memoryAllocation->getId();
			return false;

		case AllocationSegment::heap:
			Q_ASSERT(heapSegment);
			return heapSegment->allocate( memoryAllocation );

		case AllocationSegment::stack:
			Q_ASSERT(cpuCores);
			return cpuCores->allocate( memoryAllocation );

		case AllocationSegment::data:
			Q_ASSERT(dataSegment);
			return dataSegment->allocate( memoryAllocation );

		case AllocationSegment::unknown:
		case AllocationSegment::inputOutput:
		case AllocationSegment::free:
			// Variables cannot be allocated in standard input/output or unknown segments
			Q_ASSERT(false);
			return false;
	}

	return false;
}
