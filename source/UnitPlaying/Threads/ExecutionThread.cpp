#include "ExecutionThread.h"
#include "CallStack.h"
#include "Common.h"
#include "CpuCore.h"
#include "ExecutionThreadActor.h"
#include "GdbItemTree.h"
#include "GdbResponseListener.h" // updateMaxDuration()
#include "LogManager.h"
#include "Scene.h"
#include "Spacer.h"

#include <QTimer>

// Proportion of the callStackSpacer on top of the robot when it is active/busy (non idle)
const qreal busyCallStackSpacerProportion = 0.9;


ExecutionThread::ExecutionThread(size_t startByte, size_t rowSize, Scene* scene, int id)
	: LinearLayout(Qt::Vertical)
	, scene(scene)
	, id(id)
	, startByte(startByte)
	, rowSize(rowSize)
{
	buildExecutionThread();
}

void ExecutionThread::buildExecutionThread()
{
	setMargins(0.05, 0.05, 0.0);

	// Create an actor (robot) for the execution thread with its line number
	Q_ASSERT(robot == nullptr);
	robot = new ExecutionThreadActor(id, scene);

	// The actor is in the bottom part of the execution thread, almost under the call stack
	Q_ASSERT(actorLayout == nullptr);
	actorLayout = new LinearLayout(Qt::Vertical);
	const qreal zActor = zUnitPlaying::executionThread + 0.2;
	callStackSpacer = new Spacer();
	actorLayout->addItem(callStackSpacer, busyCallStackSpacerProportion, zActor);
	actorLayout->addItem(robot, 1.0 / 3.0, zActor);

	addItem(actorLayout, 1.0, zActor);

	// Create the object in charge of managing the function calls for this execution thread
	// No functions are added to the call stack until the execution thread gets updated form GDB
	Q_ASSERT(callStack == nullptr);
	callStack = new CallStack(startByte, rowSize, zUnitPlaying::stackFrame, scene);
}

int ExecutionThread::animateAppear()
{
	return robot->appear();
}

int ExecutionThread::animateDisappear()
{
	// ToDo: make callStack appear and disappear also
	return robot->disappear();
}

bool ExecutionThread::updateFromDebugger(const GdbTreeNode* threadNode, int& maxDuration)
{
	/* example of Gdb node on Linux:

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
	FilenameUpdateResult filenameUpdateResult = updateFilename( threadNode->findTextValue("frame/file"), maxDuration );
	if ( filenameUpdateResult == fileNotInPlayerSolution )
		return false;

	// Let's check if the line number or function call changed
	bool lineNumberUpdated = updateLineNumber( threadNode->findTextValue("frame/line").toInt(), maxDuration );
	functionName = threadNode->findTextValue("frame/func");

	// If any (file, line or function) changed, the thread was updated
	return filenameUpdateResult == newFileInPlayerSolution || lineNumberUpdated;
}

void ExecutionThread::setIdle(bool idle)
{
	this->idle = idle;

	Q_ASSERT(robot);
	callStackSpacer->setProportion( idle ? 0.0 : busyCallStackSpacerProportion );

	// Caller must call updateLayoutItem when this thread is finished to be setup as idle or not
//	updateLayoutItem();
}

const QColor& ExecutionThread::getHighlightColor() const
{
	Q_ASSERT(robot);
	return robot->getHighlightColor();
}

qreal ExecutionThread::getActorReferenceWidth() const
{
	Q_ASSERT(robot);
	return robot->boundingRect().width();
}

ExecutionThread::FilenameUpdateResult ExecutionThread::updateFilename(const QString& updatedFilename, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	if ( filename == updatedFilename )
		return fileIsTheSame;

	// ToDo: if the file is not part of the player's solution, do not update
//	if ( playerSolution.hasFile(updatedFilename) == false )
//		return fileNotInPlayerSolution;

	// Update to the new filename
	previousFilename = filename;
	filename = updatedFilename;

	// Avoid to keep the emtpy filename if execution thead stays on the same file all the time
	if ( previousFilename.isEmpty() )
		previousFilename = filename;

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

bool ExecutionThread::processFunctionCall(const GdbItemTree& tree, DebuggerBreakpoint* breakpoint, int& maxDuration)
{
	Q_UNUSED(breakpoint);

	// The ExecutionThread must be active (must have an assigned CPU core)
	if ( isIdle() )
	{
		// ToDo: If execution thread is idle in visualization (does not have an assigned CPU core),
		// it should be stopped at inferior through GDB, to avoid it generating more responses.
		qCCritical(logVisualizator) << "ERROR: idle execution thread running a function call. Disable GDB's all-stop mode";
		return false;
	}

	// Animate the door opening in its CPU core
	Q_ASSERT(cpuCore);
	updateMaxDuration( cpuCore->openMemoryInterface() );

	// For testing only: close the interface after the function is called
	QTimer::singleShot( maxDuration + 2000, cpuCore, SLOT(closeMemoryInterface()) );

	// Animate the function call
	Q_ASSERT(callStack);
	functionName = tree.findNodeTextValue("/frame/func");
	updateMaxDuration( callStack->callFunction(tree) );

	return true;
}
