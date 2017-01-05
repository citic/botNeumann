#include "CpuCore.h"
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

void CpuCore::buildCpuCore()
{
	Q_ASSERT(scene);
	Prop* workstation = new Prop(":/unit_playing/stack_segment.svg", scene);
	addItem(workstation, 1.0);
}
