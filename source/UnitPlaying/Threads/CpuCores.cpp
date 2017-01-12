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
	// ToDo: Remove executionThreads[] that are not visible
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
	switch ( asyncClass )
	{
		case AsyncClass::AC_THREAD_CREATED:
			updateDuration( createThread( tree.findNodeTextValue("/id").toInt() ) );
			break;

		default: break;
	}
}

void CpuCores::onResult(const GdbItemTree& tree, int& maxDuration)
{
	const GdbTreeNode* node = nullptr;
	if ( ( node = tree.findNode("/threads") ) )
		return updateThreads( node, maxDuration );
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

void CpuCores::clearAnimation()
{
	// Remove all execution threads from the cores
	for ( int index = 0; index < cpuCores.count(); ++index )
		cpuCores[index]->removeThread();

	// Remove all execution threads, visible or not
	for ( int index = 0; index < executionThreads.count(); ++index )
		delete executionThreads[index];
	executionThreads.clear();
}

void CpuCores::updateThreads(const GdbTreeNode* threadsNode, int& maxDuration)
{
	// Each child node of threadsNode is a thread in execution, gets its information
	for ( int childIndex = 0; childIndex < threadsNode->getChildCount(); ++childIndex )
	{
		// Get the child and its name representing a thread
		const GdbTreeNode* threadChildNode = threadsNode->getChild(childIndex);

		// Get the id of the thread
		int threadId = threadChildNode->findTextValue("id").toInt();

		// Update its correspondent execution thread object
		Q_ASSERT(threadId > 0 && threadId <= executionThreads.count());
		executionThreads[threadId - 1]->updateFromDebugger(threadChildNode, maxDuration);
	}
}
