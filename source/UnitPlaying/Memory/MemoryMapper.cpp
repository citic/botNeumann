#include "MemoryMapper.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "GdbCall.h"
#include "HeapSegment.h"
#include "LogManager.h"
#include "VisualizationContext.h"

/*static*/ MemoryMapper* MemoryMapper::instance = nullptr;

MemoryMapper::MemoryMapper(GdbCall* debuggerCall, QObject* parent)
	: QObject(parent)
	, debuggerCall(debuggerCall)
{
	Q_ASSERT(instance == nullptr);
	instance = this;
}

MemoryMapper::~MemoryMapper()
{
	for ( QHash<QString, MemoryAllocation*>::Iterator itr = mapNameMemoryAllocation.begin(); itr != mapNameMemoryAllocation.end(); ++itr )
		delete itr.value();

	instance = nullptr;
}

MemoryAllocation* MemoryMapper::createWatch(const QString& name, const QString& watchName, AllocationSegment segment, bool shouldAllocate)
{
	// Create a memory block for the watch and load it from the tree
	MemoryAllocation* watch = new MemoryAllocation(segment);
	watch->name = name;
	watch->watchName = watchName;

	// Fill the missing data
	addMapping(watch);

	// The variable was updated, we can now allocate it in the visualization segments
	if ( shouldAllocate )
		allocate(watch);

	// Success
	return watch;
}

MemoryAllocation* MemoryMapper::createLocal(const QString& name, const QString& type, const QString& value, const QString& watchName)
{
	// Create a memory block for the watch and load it from the tree
	MemoryAllocation* watch = new MemoryAllocation(AllocationSegment::stack);
	watch->name = name;
	watch->watchName = watchName;
	watch->dataTypeStr = type;
	watch->value = value;

	// Fill the missing data
	addMapping(watch);

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

bool MemoryMapper::addMapping(MemoryAllocation* watch)
{
	// Add the watch to the maps
	mapNameMemoryAllocation.insert( watch->getId(), watch );

	GdbItemTree resultWatch;
	const QString& command = QString("-var-create %1 @ %2").arg(watch->watchName).arg(watch->name);
	if ( debuggerCall->sendGdbCommand(command, visMemoryMapper, &resultWatch) == GDB_ERROR )
	{
		qCCritical(logVisualizator).noquote() << "Failed to set watch for" << watch->name;
		return false;
	}

	// Update the watch from GDB's tree result
	if ( ! watch->loadFromGdbVariableObject(resultWatch) )
	{
		qCCritical(logApplication) << "Invalid watch result:" << resultWatch.buildDescription();
		return false;
	}

	qCDebug(logApplication) << "Watch created:" << watch->getId() << "referring to" << watch->name;
	if ( watch->updateMissingFields(debuggerCall) == false )
		return false;

	return true;
}
