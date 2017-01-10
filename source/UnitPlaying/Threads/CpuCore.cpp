#include "CpuCore.h"
#include "Common.h"
#include "ExecutionThread.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

// The number of memory rows reserved for the robot
const double robotRows = 3.0;

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
	return unit.getStackSegmentVisibleRows() + robotRows;
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

	// Add the execution thread as a child in a higher layer
	addItem( executionThread, 1.0, zUnitPlaying::executionThread );
//	executionThread->setVisible(true);
	updateLayoutItem();
	return executionThread->animateAppear();
}

int CpuCore::removeThread()
{
	if ( executionThread == nullptr )
		return -1;

	int duration = executionThread->animateDisappear();

	removeItem(executionThread, false);
	executionThread = nullptr;

	return duration;
}

void CpuCore::buildCpuCore()
{
	Q_ASSERT(scene);
	workstation = new Prop(":/unit_playing/stack_segment.svg", scene);
	addItem(workstation, 0.85, zUnitPlaying::cpuCore);
}
