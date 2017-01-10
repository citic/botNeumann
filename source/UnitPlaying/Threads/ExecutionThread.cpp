#include "ExecutionThread.h"
#include "Actor.h"
#include "Common.h"
#include "Scene.h"

ExecutionThread::ExecutionThread(Scene* scene, int id)
	: LinearLayout(Qt::Vertical)
	, scene(scene)
	, id(id)
{
	buildExecutionThread();
}

void ExecutionThread::buildExecutionThread()
{
	setMargins(0.1);

	Q_ASSERT(robot == nullptr);
	robot = new Actor(QString(":/unit_playing/threads/thread%1_back.svg").arg( (id - 1) % 4 + 1 ), scene);
	robot->setMarginTop(0.7);
	robot->alignCenter();
	addItem(robot, 1.15, zUnitPlaying::executionThread + 0.2);
}

int ExecutionThread::animateAppear()
{
	return robot->appear();
}

int ExecutionThread::animateDisappear()
{
	return robot->disappear();
}
