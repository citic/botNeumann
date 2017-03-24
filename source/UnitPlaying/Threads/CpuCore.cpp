#include "CpuCore.h"
#include "Actor.h"
#include "CallStack.h"
#include "Common.h"
#include "ExecutionThread.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

// The number of memory rows reserved for the robot
const double robotRows = 1.0;

CpuCore::CpuCore(int cpuCoreNumber, Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
	, cpuCoreNumber(cpuCoreNumber)
{
	buildCpuCore();
}

CpuCore::~CpuCore()
{
}

double CpuCore::getHeightInRows() const
{
	// Visible rows indicated by unit, + 1.0 for the roof, and the robot
	return unit.getStackSegmentVisibleRows() + 1.0 + robotRows;
}

void CpuCore::buildCpuCore()
{
	Q_ASSERT(scene);
	workstation = new Prop("up_cpu_core", scene);
	addItem(workstation, 1.0, zUnitPlaying::cpuCore + 0.0);

	memoryInterface = new Actor("up_stack_segment_allocator_closed", scene);
	memoryInterface->setMargins(0.85, 0.075, 0.0);
	addItem(memoryInterface, 1.0, zUnitPlaying::cpuCore + 0.1);

	QStringList memoryInterfaceFaces;
	memoryInterfaceFaces << "up_stack_segment_allocator_closed";
	memoryInterfaceFaces << "up_stack_segment_allocator_open_33";
	memoryInterfaceFaces << "up_stack_segment_allocator_open_66";
	memoryInterfaceFaces << "up_stack_segment_allocator_open_100";
	memoryInterface->setFaces( memoryInterfaceFaces );

	// ToDo: slice memory interface into:
	// 1. "up_stack_segment_allocator_open_upper"
	// 2. "up_stack_segment_allocator_open_lower"
}

int CpuCore::runThread(ExecutionThread* thread)
{
	// If the thread is already set, done
	Q_ASSERT(thread);
	if ( executionThread == thread )
		return -1;

	// If there is an old execution thread running, hide it
//	if ( executionThread )
//		executionThread->setVisible(false);

	// Assign the new execution thread
	executionThread = thread;
	executionThread->setCpuCore(this);

	// Add the execution thread (actor) as a child in a higher layer
	addItem( executionThread, 1.0, zUnitPlaying::executionThread );

	// Add the call stack of the execution thread to the cpu core
	CallStack* callStack = executionThread->getCallStack();
	addItem( callStack, 1.0, zUnitPlaying::stackFrame );

	// Set margins for the call stack
	callStack->setMargins(0.06, 0.075);

//	executionThread->setVisible(true);
	updateLayoutItem();
	return executionThread->animateAppear();
}

int CpuCore::removeThread()
{
	if ( executionThread == nullptr )
		return -1;

	int duration = executionThread->animateDisappear();

	executionThread->setCpuCore(nullptr);

	removeItem( executionThread->getCallStack(), true );
	removeItem(executionThread, false);
	executionThread = nullptr;

	return duration;
}

int CpuCore::openMemoryInterface()
{
	Q_ASSERT(memoryInterface);
	return memoryInterface->transitionFaces(QTimeLine::Forward);
}

int CpuCore::closeMemoryInterface()
{
	Q_ASSERT(memoryInterface);
	return memoryInterface->transitionFaces(QTimeLine::Backward);
}
