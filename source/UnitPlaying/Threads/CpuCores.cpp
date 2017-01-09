#include "CpuCore.h"
#include "CpuCores.h"
#include "ExecutionThread.h"
#include "Scene.h"
#include "Unit.h"

CpuCores::CpuCores(Unit& unit, Scene* scene, QObject* parent)
	: GdbResponseListener(parent)
	, MemorySegment(unit, scene, Qt::Horizontal)
{
	createCpuCores();
}

CpuCores::~CpuCores()
{
	// Do not delete cpuCores[] or executionThreads[]. They are deleted by the scene
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

void CpuCores::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	switch ( asyncClass )
	{
		case AsyncClass::AC_THREAD_CREATED:
			updateDuration( createThread( tree.findNodeTextValue("/id").toInt() ) );
			break;

		default: break;
	}
}

int CpuCores::createThread(int id)
{
	// Create an execution thread, that is represeted by a robot with racks
	ExecutionThread* thread = new ExecutionThread(scene, id);
	executionThreads.append(thread);

	// If there is an idle CPU core, assign the new execution thread
	int cpuCoreIndex = findFirstIdleCpuCore();
	if ( cpuCoreIndex == -1 )
		return -1;

	// Assign the CPU core to the thread
	return cpuCores[cpuCoreIndex]->runThread(thread);
}

int CpuCores::findFirstIdleCpuCore() const
{
	for ( int index = 0; index < cpuCores.count(); ++index )
		if ( cpuCores[index] && cpuCores[index]->isIdle() )
			return index;

	return -1;
}
