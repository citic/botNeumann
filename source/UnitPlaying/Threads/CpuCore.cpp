#include "CpuCore.h"
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
	addItem(workstation, 1.0, zUnitPlaying::cpuCore);
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
