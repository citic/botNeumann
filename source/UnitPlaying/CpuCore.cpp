#include "CpuCore.h"
#include "Prop.h"
#include "Scene.h"

CpuCore::CpuCore(int cpuCoreNumber, Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
	, cpuCoreNumber(cpuCoreNumber)
{
	buildCpuCore();
}

CpuCore::~CpuCore()
{
}

void CpuCore::buildCpuCore()
{
	Q_ASSERT(scene);
	Prop* workstation = new Prop(":/unit_playing/unit_playing/workstation.svg", scene);
	addItem(workstation, 1.0);
}
