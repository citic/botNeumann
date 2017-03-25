#include "CpuCore.h"
#include "Common.h"
#include "CpuCores.h"
#include "DebuggerBreakpoint.h"
#include "ExecutionThread.h"
#include "LogManager.h"
#include "Scene.h"
#include "Unit.h"

// Reserve 1 more row for idle threads, and create visual separation between cores and data segment
const double idleThreadsRows = 1.0;

CpuCores::CpuCores(Unit& unit, Scene* scene, QObject* parent)
	: GdbResponseListener(parent)
	, MemorySegment(unit, scene, Qt::Vertical)
{
	createCpuCores();
}

CpuCores::~CpuCores()
{
	// Do not delete cpuCores[] or executionThreads[]. They are deleted by the scene
	// ToDo: Remove executionThreads[] that are not visible
}

double CpuCores::getHeightInRows() const
{
	Q_ASSERT(cpuCores.count() > 0);
	return cpuCores[0]->getHeightInRows() + idleThreadsRows;
}

bool CpuCores::allocate(MemoryAllocation* memoryAllocation)
{
	// ToDo: Implement allocation
	Q_UNUSED(memoryAllocation);
	Q_ASSERT(false);
	return false;
}

bool CpuCores::deallocate(MemoryAllocation* memoryAllocation)
{
	// ToDo: Implement deallocation
	Q_UNUSED(memoryAllocation);
	Q_ASSERT(false);
	return false;
}

void CpuCores::createCpuCores()
{
	// CPU cores are in a horizontal layout
	Q_ASSERT(cpuCoresLayout == nullptr);
	cpuCoresLayout = new LinearLayout(Qt::Horizontal);

	// Get the number of CPU cores that this Unit was configured for
	int coreCount = unit.getCpuCores();
	Q_ASSERT(coreCount > 0);
	cpuCores.reserve(coreCount);

	// Create a graphical CPU core for each core requested in Unit
	for ( int index = 0; index < coreCount; ++index )
	{
		// Create the CPU core and add it to the topmost horizontal layout
		CpuCore* cpuCore = new CpuCore(index, unit, scene);
		cpuCores.append(cpuCore);
		cpuCoresLayout->addLayout( cpuCore, 1.0 / coreCount );

		// Overlap the workstation separator with the previous one
		if ( index ) cpuCore->setMarginLeft(-0.04105605779343);
	}

	// Create the layout to place future execution threads that will be sleeping (idle)
	Q_ASSERT(idleThreadsLayout == nullptr);
	idleThreadsLayout = new LinearLayout(Qt::Horizontal);

	// Add both layouts to this segment
	double totalRows = getHeightInRows();
	addLayout(cpuCoresLayout, cpuCores[0]->getHeightInRows() / totalRows, zUnitPlaying::cpuCores);
	// Separate idle threads from busy threads
	addStretch( 0.5 / totalRows, zUnitPlaying::cpuCores);
	// Idle threads overflow to use space behind the data segment
	addLayout(idleThreadsLayout, (idleThreadsRows + 2.0) / totalRows, zUnitPlaying::cpuCores);
}

void CpuCores::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(context);
	switch ( asyncClass )
	{
		case AsyncClass::AC_THREAD_CREATED:
			updateMaxDuration( createThread( tree.findNodeTextValue("/id").toInt() ) );
			break;

		case AsyncClass::AC_THREAD_EXITED:
			updateMaxDuration( removeThread( tree.findNodeTextValue("/id").toInt() ) );
			break;

		default: break;
	}
}

void CpuCores::onResult(const GdbItemTree& tree, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(context);
	const GdbTreeNode* node = nullptr;
	if ( ( node = tree.findNode("/threads") ) )
		return updateThreads( node, maxDuration );
}

int CpuCores::createThread(int id)
{
	// If user exceed the number of allowed threads, animate a program terminated
	if ( unit.getMaxThreads() > 0 && executionThreads.count() + 1 > unit.getMaxThreads() )
	{
		// ToDo: Animate program termination due to exceeded allowed threads
		qCCritical(logSolutionCrash) << "Player solution exceeded max allowed threads" << unit.getMaxThreads();
		return -1;
	}

	// Get memory for the stack segment of the new execution thread
	// ToDo: The unit's memory distribution model assigns memory to cores, not threads
	size_t startByte = unit.getCoreStartByte( executionThreads.count() );
	size_t rowSize = unit.getStackSegmentVisibleSize() / unit.getStackSegmentVisibleRows() / 2;

	// Create an execution thread, that is represeted by a robot with racks
	ExecutionThread* thread = new ExecutionThread(startByte, rowSize, scene, id);
	executionThreads.append(thread);

	// If there is an idle CPU core, assign the new execution thread to it. Otherwise the thread
	// will appeared sleeping in the visualization in the idle layout until a cpu core gets freed
	int cpuCoreIndex = findFirstIdleCpuCore();
	return cpuCoreIndex >= 0
		? thread->run( cpuCores[cpuCoreIndex] )
		: thread->sleep(idleThreadsLayout, ++idleThreadsCount);
}

int CpuCores::removeThread(int id)
{
	// Get a pointer to the thread with the given id and its index
	int index = -1;
	ExecutionThread* thread = findThread(id, &index);

	// Remove the thread from scene
	Q_ASSERT(thread);
	int duration = thread->terminate();

	// Remove the obsolete thread pointer from the list
	Q_ASSERT( index >= 0 && index < executionThreads.count() );
	executionThreads.remove(index);

	// Return duration of animation in milliseconds
	return duration;
}

int CpuCores::findFirstIdleCpuCore() const
{
	for ( int index = 0; index < cpuCores.count(); ++index )
		if ( cpuCores[index] && cpuCores[index]->isIdle() )
			return index;

	return -1;
}

ExecutionThread* CpuCores::findThread(int id, int* threadIndex) const
{
	for ( int index = 0; index < executionThreads.count(); ++index )
	{
		if ( executionThreads[index]->getId() == id )
		{
			if ( threadIndex ) *threadIndex = index;
			return executionThreads[index];
		}
	}

	return nullptr;
}

void CpuCores::clearAnimation()
{
	// Remove all execution threads, visible or not
	for ( int index = 0; index < executionThreads.count(); ++index )
		executionThreads[index]->terminate();

	// Destroy all threads
	executionThreads.clear();
}

void CpuCores::updateThreads(const GdbTreeNode* threadsNode, int& maxDuration)
{
	// Each child node of threadsNode is a thread in execution, gets its information
	for ( int childIndex = 0; childIndex < threadsNode->getChildCount(); ++childIndex )
	{
		// Get the child and its name representing a thread
		const GdbTreeNode* threadChildNode = threadsNode->getChild(childIndex);

		// Get the id of the thread
		int threadId = threadChildNode->findTextValue("id").toInt();

		// Find the thread with that id
		ExecutionThread* thread = findThread(threadId);
		Q_ASSERT(thread);

		// Update its correspondent execution thread object
		if ( thread->updateFromDebugger(threadChildNode) )
		{
			// The thread was updated, refresh its highlighted line on the code editor
			emit executionThreadUpdated(thread, maxDuration);
		}
	}
}

bool CpuCores::processFunctionCall(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration)
{
	// Player solution hit a breakpoint that has the role of functionCall. The breakpoint must be
	// at the beginning of the body of a function in a file that is part of player solution.
	// See comment at Visualization::processBreakpointHit() body for a breakpoint-hit example

	// Get the /thread-id="#" from *stopped response
	bool ok = false;
	int threadId = tree.findNodeTextValue("/thread-id").toInt(&ok);
	Q_ASSERT(ok);

	// Get the ExecutionThread object that is identified by that number
	ExecutionThread* executionThread = findThread(threadId);
	Q_ASSERT(executionThread);

	// The execution thread will animate the function call
	return executionThread->processFunctionCall(tree, debuggerCall, maxDuration);
}
