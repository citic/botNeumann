#include "CpuCore.h"
#include "CpuCores.h"

CpuCores::CpuCores(int availableCpuCoreCount, int minThreadCount, size_t size, Scene* scene)
	: LinearLayout(Qt::Horizontal)
	, availableCpuCoreCount(availableCpuCoreCount)
	, minThreadCount(minThreadCount)
	, size(size)
	, scene(scene)
{
	createCpuCores();
}

CpuCores::~CpuCores()
{
}

void CpuCores::createCpuCores()
{
	cpuCores.reserve(availableCpuCoreCount);
	size_t cpuCoreSize = size / availableCpuCoreCount;
	for ( int i = 0; i < availableCpuCoreCount; ++i )
	{
		CpuCore* cpuCore = new CpuCore(cpuCoreSize, scene);
		this->addLayout(cpuCore, 1.0 / availableCpuCoreCount);
		// Overlap the workstation separator with the previous one
		if ( i ) cpuCore->setMargins(0.0, 0.0, 0.0, -0.04105605779343);
	}
}
