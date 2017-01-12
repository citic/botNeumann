#include "ExecutionThread.h"
#include "Actor.h"
#include "Common.h"
#include "GdbItemTree.h"
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

void ExecutionThread::updateFromDebugger(const GdbTreeNode* threadNode, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	/* example of Gdb node in Linux:
		id="1",
		target-id="process 20523",
		name="rand",
		frame=
		{
			level="0",
			addr="0x0000000000400695",
			func="main",
			args=
			[
				{ name="argc", value="3" },
				{ name="argv", value="0x7fffffffdfa8" }
			],
			file="rand.c",
			fullname="/full/path/to/rand.c",
			line="7"
		},
		state="stopped",
		core="7"
	*/
	functionName = threadNode->findTextValue("frame/func");
	filename = threadNode->findTextValue("frame/file");
	lineNumber =  threadNode->findTextValue("frame/line").toInt();

	qDebug("  Thread[id=%d][func=%s][file=%s][line=%d]", id, qUtf8Printable(functionName), qUtf8Printable(filename), lineNumber);
}
