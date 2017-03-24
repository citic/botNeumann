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

	// If there is an idle CPU core, assign the new execution thread to it
	int cpuCoreIndex = findFirstIdleCpuCore();
	if ( cpuCoreIndex >= 0 )
		return cpuCores[cpuCoreIndex]->runThread(thread);

	// There are not available CPU cores, the thread will be sleeping in the visualization
	// the idle layout will manage these threads
	return setupIdleThread(thread);
}

int CpuCores::findFirstIdleCpuCore() const
{
	for ( int index = 0; index < cpuCores.count(); ++index )
		if ( cpuCores[index] && cpuCores[index]->isIdle() )
			return index;

	return -1;
}

int CpuCores::setupIdleThread(ExecutionThread* thread)
{
	// The thread is idle, it displays and behaves differently of an active thread
	thread->setIdle(true);

	// Width of robots is calculated using a 1024px width scene as reference
	const int refWidthScene = 1024;
	const int refWidth = thread->getActorReferenceWidth();
	double proportion = (double)refWidth / (double)refWidthScene;

	// If there are lots of execution threads, distribute them in layers
	int layer = ++idleThreadsCount * refWidth / refWidthScene;
	idleThreadsLayout->addItem( thread, proportion, zUnitPlaying::cpuCores + 0.1 + layer / 100.0 );

	// Resize idle threads within the idle layout
	idleThreadsLayout->updateLayoutItem();
	return thread->animateAppear();
}

void CpuCores::clearAnimation()
{
	// Remove all execution threads from the cores
	for ( int index = 0; index < cpuCores.count(); ++index )
		cpuCores[index]->removeThread(true);

	// Remove all execution threads, visible or not
	for ( int index = 0; index < executionThreads.count(); ++index )
		if ( executionThreads[index]->isIdle() )
			executionThreads[index]->animateDisappear(true);

	// Destroy all threads
	idleThreadsLayout->removeAllItems(true);
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

		// Update its correspondent execution thread object
		Q_ASSERT(threadId > 0 && threadId <= executionThreads.count());
		if ( executionThreads[threadId - 1]->updateFromDebugger(threadChildNode, maxDuration) )
		{
			// The thread was updated, refresh its highlighted line on the code editor
			emit executionThreadUpdated(executionThreads[threadId - 1]);
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

ExecutionThread* CpuCores::findThread(int id) const
{
	for ( int index = 0; index < executionThreads.count(); ++index )
		if ( executionThreads[index]->getId() == id )
			return executionThreads[index];

	return nullptr;
}
