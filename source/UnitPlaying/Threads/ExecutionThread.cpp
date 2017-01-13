#include "ExecutionThread.h"
#include "Common.h"
#include "ExecutionThreadActor.h"
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

bool ExecutionThread::updateFromDebugger(const GdbTreeNode* threadNode, int& maxDuration)
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
	FilenameUpdateResult result1 = updateFilename( threadNode->findTextValue("frame/file"), maxDuration );
	if ( result1 == fileNotInPlayerSolution )
		return false;

	// Let's check if the line number or function call changed
	bool result2 = updateLineNumber( threadNode->findTextValue("frame/line").toInt(), maxDuration );
	bool result3 = updateFunctionName( threadNode->findTextValue("frame/func"), maxDuration );

	// If any (file, line or function) changed, the thread was updated
	return result1 == newFileInPlayerSolution || result2 || result3;
}

const QColor& ExecutionThread::getHighlightColor() const
{
	Q_ASSERT(robot);
	return robot->getHighlightColor();
}

ExecutionThread::FilenameUpdateResult ExecutionThread::updateFilename(const QString& updatedFilename, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	if ( filename == updatedFilename ) return fileIsTheSame;

	// ToDo: if the file is not part of the player's solution, do not update
//	if ( playerSolution.hasFile(updatedFilename) == false )
//		return fileNotInPlayerSolution;

	// Update to the new filename
	previousFilename = filename;
	filename = updatedFilename;

	return newFileInPlayerSolution;
}

bool ExecutionThread::updateLineNumber(int updatedLineNumber, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	// If the execution thread stays at the same line, do not reflect any change on the GUI
	if ( lineNumber == updatedLineNumber )
		return false;

	// Update to the new line number
	previousLineNumber = lineNumber;
	lineNumber = updatedLineNumber;

	// Update the line within the robot's display
	Q_ASSERT(robot);
	robot->updateLineNumber(lineNumber);

	return true;
}

bool ExecutionThread::updateFunctionName(const QString& updatedFunctionName, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	if ( functionName == updatedFunctionName ) return false;

	// Update the function name
//	previousFunctionName = functionName;
	functionName = updatedFunctionName;

	return true;
}

