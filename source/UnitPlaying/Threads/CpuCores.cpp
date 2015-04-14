#include "CpuCore.h"
#include "CpuCores.h"
#include "Unit.h"

CpuCores::CpuCores(Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Horizontal)
{
	createCpuCores();
}

CpuCores::~CpuCores()
{
}

void CpuCores::createCpuCores()
{
	int coreCount = unit.getCpuCores();
	cpuCores.reserve(coreCount);
	for ( int i = 0; i < coreCount; ++i )
	{
		CpuCore* cpuCore = new CpuCore(i, unit, scene);
		this->addLayout( cpuCore, 1.0 / coreCount );
		// Overlap the workstation separator with the previous one
		if ( i ) cpuCore->setMarginLeft(-0.04105605779343);
	}
}
