#include "CpuCore.h"
#include "ExecutionThread.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

// The number of memory rows reserved for the robot
const double robotRows = 2.0;

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

void CpuCore::runThread(ExecutionThread* thread)
{
	// If there is an old execution thread running, hide it
	if ( executionThread )
		executionThread->setVisible(false);

	// Assign the new execution thread
	executionThread = thread;

	// Place the thread in the middle of the CPU core
	// ToDo: calculate real center
	Q_ASSERT(workstation);
	double x = workstation->x() + workstation->boundingRect().width() * 0.5;
	double y = workstation->y() + workstation->boundingRect().height() * 0.66;
	executionThread->setPos( x, y );
	executionThread->setVisible(true);
}

void CpuCore::buildCpuCore()
{
	Q_ASSERT(scene);
	workstation = new Prop(":/unit_playing/stack_segment.svg", scene);
	addItem(workstation, 1.0);
}
