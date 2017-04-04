#include "CpuCore.h"
#include "Actor.h"
#include "CallStack.h"
#include "Common.h"
#include "ExecutionThread.h"
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
	workstation = new Actor("up_cpu_core", scene);
	addItem(workstation, 1.0, zUnitPlaying::cpuCore + 0.0);

	memoryInterface = new Actor("up_stack_segment_allocator_closed", scene);
	memoryInterface->setMargins(0.85, 0.05, 0.0);
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

int CpuCore::openMemoryInterface()
{
	Q_ASSERT(memoryInterface);
	return memoryInterface->transitionFaces(QTimeLine::Forward, 500);
}

int CpuCore::closeMemoryInterface()
{
	Q_ASSERT(memoryInterface);
	return memoryInterface->transitionFaces(QTimeLine::Backward, 500);
}
