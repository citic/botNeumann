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

double CpuCores::getHeightInRows() const
{
	return cpuCores.count() > 0 ? cpuCores[0]->getHeightInRows() : 0.0;
}

void CpuCores::createCpuCores()
{
	int coreCount = unit.getCpuCores();
	cpuCores.reserve(coreCount);
	for ( int index = 0; index < coreCount; ++index )
	{
		CpuCore* cpuCore = new CpuCore(index, unit, scene);
		cpuCores.append(cpuCore);
		this->addLayout( cpuCore, 1.0 / coreCount );
		// Overlap the workstation separator with the previous one
		if ( index ) cpuCore->setMarginLeft(-0.04105605779343);
	}
}
