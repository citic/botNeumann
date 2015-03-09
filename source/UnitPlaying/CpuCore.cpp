#include "CpuCore.h"
#include "Prop.h"
#include "Scene.h"

CpuCore::CpuCore(size_t stackSegmentSize, Scene* scene)
	: LinearLayout(Qt::Vertical)
	, stackSegmentSize(stackSegmentSize)
	, scene(scene)
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
	addItem(workstation, 1);
}
