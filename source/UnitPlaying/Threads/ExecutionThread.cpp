#include "ExecutionThread.h"
#include "ExecutionThreadActor.h"
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
	robot = new ExecutionThreadActor(id, scene);
	addItem(robot, 1.15, zUnitPlaying::executionThread + 0.2);
}

int ExecutionThread::animateAppear()
{
	return robot->animateAppear();
}

int ExecutionThread::animateDisappear()
{
	return robot->animateDisappear();
}

void ExecutionThread::updateFromDebugger(const GdbTreeNode* threadNode, int& maxDuration)
{
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

	// Only update the execution thread if it is running code that is part of the player's solution
	// E.g: if it is running an external library function, it is considered a black-box and it is
	// not visualized, because it is very likely to have no C/C++ source code (assembler)
	if ( updateFilename( threadNode->findTextValue("frame/file"), maxDuration ) )
	{
		updateLineNumber( threadNode->findTextValue("frame/line").toInt(), maxDuration );
		updateFunctionName( threadNode->findTextValue("frame/func"), maxDuration );
	}
}

bool ExecutionThread::updateFilename(const QString& updatedFilename, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	if ( filename == updatedFilename ) return false;

	// ToDo: if the file is not part of the player's solution, do not update

	// ToDo: emit a file change signal to show a code editor with the filename

	return true;
}

bool ExecutionThread::updateLineNumber(int updatedLineNumber, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	// If the execution thread stays at the same line, do not reflect any change on the GUI
	if ( lineNumber == updatedLineNumber )
		return false;

	// Update the line within the robot's display
	Q_ASSERT(robot);
	lineNumber = updatedLineNumber;
	robot->updateLineNumber(lineNumber);

	// ToDo: emit a line number change signal to hightlight it in code editor

	return true;
}

bool ExecutionThread::updateFunctionName(const QString& updatedFunctionName, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	if ( functionName == updatedFunctionName ) return false;

	// ToDo: call function

	return true;
}

