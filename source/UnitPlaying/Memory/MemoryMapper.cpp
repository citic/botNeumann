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
	if ( ! addMapping(watch) )
	{
		delete watch;
		return nullptr;
	}

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
	// Create a gdb's watch
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

	// That watch is valid. Add it the watch to the maps
	mapNameMemoryAllocation.insert( watch->getId(), watch );
	return true;
}

bool MemoryMapper::removeWatch(const QString& watchName)
{
	// Find the memory allocation associated to the watch name and delete it
	MemoryAllocation* variable = mapNameMemoryAllocation.value(watchName, nullptr);
	delete variable;

	// Remove the (watchName,variable) association from the map
	if ( mapNameMemoryAllocation.remove(watchName) <= 0 )
		return false;

	// Removes the gdb's watch
	GdbItemTree resultWatch;
	const QString& command = QString("-var-delete %1").arg(watchName);
	if ( debuggerCall->sendGdbCommand(command, visMemoryMapper, &resultWatch) == GDB_ERROR )
		qCCritical(logVisualizator).noquote() << "Failed to delete watch" << watchName;

	// Check gdb watch deletion is success, eg:
	// 8^done,ndeleted="1"
	Q_ASSERT( resultWatch.findNodeTextValue("ndeleted") == "1" );
	return true;
}

bool MemoryMapper::updateWatches(const GdbItemTree& tree, int& maxDuration)
{
	/*
		^done,
		changelist=
		[
			{
				name="bn_lv_1_1_2",
				value="5",
				in_scope="true",
				type_changed="false",
				has_more="0"
			},
			{
				name="bn_lv_1_1_1",
				value="2",
				in_scope="true",
				type_changed="false",
				has_more="0"
			}
		]
	*/
	const GdbTreeNode* changeList = tree.findNode("/changelist");
	Q_ASSERT(changeList);

	// Process each changed watch
	for ( int childIndex = 0; childIndex < changeList->getChildCount(); ++childIndex )
		updateWatch( changeList->getChild(childIndex), maxDuration );

	return true;
}

bool MemoryMapper::updateWatch(const GdbTreeNode* watchNode, int& maxDuration)
{
	// Get the name of the watch
	Q_ASSERT(watchNode);
	const QString& watchName = watchNode->findTextValue("name");

	// Act according to the type of the watch. It is indicated by its name:

	// bn_io_file  :  standard input/output/error streams
	// bn_gv_n     :  global variable with number n
	// bn_lv_t_f_n :  local variable n of thread t, function f
	// bn_pd_th_fc :  pointed data in function call fc of thread th

	if ( watchName.startsWith("bn_lv_") )
		return updateVariableValue(watchNode, maxDuration);
//	if ( watchName.startsWith("bn_io_") )
//		return processInputOutputUpdate(watchNode, maxDuration);
	if ( watchName.startsWith("bn_gv_") )
		return updateVariableValue(watchNode, maxDuration);
//	if ( watchName.startsWith("bn_pd_") )
//		return processPointedDataUpdate(watchNode, maxDuration);

	return false;
}

bool MemoryMapper::updateVariableValue(const GdbTreeNode* watchNode, int& maxDuration)
{
	// Get the name of the watch
	const QString& watchName = watchNode->findTextValue("name");
	qCCritical(logTemporary()) << "Updating variable value of watch:" << watchName;

	// Find the variable (memory allocation) that was updated
	MemoryAllocation* variable = mapNameMemoryAllocation.value(watchName, nullptr);

	// If the watch is not found in the map, it was deleted by a function return
	if ( ! variable )
		return false;

	// Update the variable's value in all its graphical variables
	int duration = variable->updateValue(watchNode);

	// Caller needs the longest duration to know how many milliseconds to wait until advance to the
	// next visualization step
	if ( duration > maxDuration )
		maxDuration = duration;

	// Done
	return duration >= 0;
}
