#include "ExecutionThread.h"
#include "CallStack.h"
#include "Common.h"
#include "CpuCore.h"
#include "ExecutionThreadActor.h"
#include "GdbCall.h"
#include "GdbResponseListener.h"
#include "LogManager.h"
#include "Scene.h"
#include "Spacer.h"

#include <QTimer>

ExecutionThread::ExecutionThread(size_t startByte, size_t rowSize, Scene* scene, int id)
	: LinearLayout(Qt::Vertical)
	, id(id)
	, startByte(startByte)
	, rowSize(rowSize)
	, scene(scene)
{
	buildExecutionThread();
}

void ExecutionThread::buildExecutionThread()
{
	setMargins(0.05, 0.05, 0.0);

	// Create an actor (robot) for the execution thread with its line number
	Q_ASSERT(actor == nullptr);
	actor = new ExecutionThreadActor(id, scene);
	actor->setVisible(false);

	// The actor is in the bottom part of the execution thread, almost under the call stack
	const qreal zActor = zUnitPlaying::executionThread + 0.2;
	insertItem(actor, 0.9, 1.0 / 3.0, zActor);

	// Create the object in charge of managing the function calls for this execution thread
	// No functions are added to the call stack until the execution thread gets updated form GDB
	// By default the call stack is invisible, until functions are called
	Q_ASSERT(callStack == nullptr);
	callStack = new CallStack(startByte, rowSize, zUnitPlaying::stackFrame, scene);
	callStack->setMargins(0.06, 0.075);

	// Add the call stack to this thread, but keep it invisible
	addItem( callStack, 1.0, zUnitPlaying::stackFrame );
	callStack->setVisible(false);
}

int ExecutionThread::run(CpuCore* cpuCore)
{
	Q_ASSERT(cpuCore);
	Q_ASSERT(cpuCore->getThread() != this);
	Q_ASSERT(cpuCore->getThread() == nullptr);

	// Detach from the previous area of the scene
	int duration = detach();

	// Change to active state running on the given cpu core
	state = threadActive;
	this->cpuCore = cpuCore;

	// The cpuCore needs to know it is not idle
	cpuCore->setThread(this);

	// The call stack needs to know the height of the cpu core to raise function calls
	Q_ASSERT(callStack);
	callStack->setCpuCoreRows( cpuCore->getHeightInRows() );

	// The actor will be at the bottom of the call stack
	Q_ASSERT(actor);
	actor->setStartProportion(0.9);
	duration += actor->appear();

	// Add this execution thread (both actor and call stack) to the cpu core
	cpuCore->addItem( this, 1.0, zUnitPlaying::executionThread );
	cpuCore->updateLayoutItem();

	// Animate appearing of both, actor and call stack
	actor->setVisible(true);
	callStack->setVisible(true);
	duration += callStack->animateAppear();
	return duration;
}

int ExecutionThread::sleep(LinearLayout* idleThreads, int idleThreadNumber)
{
	// Detach from the previous area of the scene
	int duration = detach();

	// Change to idle state
	state = threadIdle;
	this->idleThreads = idleThreads;

	// Width of robots is calculated using a 1024px width scene as reference
	const int refWidthScene = 1024;
	const int refWidth = getActorReferenceWidth();
	double proportion = (double)refWidth / (double)refWidthScene;

	// If there are lots of execution threads, distribute them in layers
	qreal start = (idleThreadNumber * refWidth + qrand() % refWidth) % refWidthScene;
	idleThreads->insertItem( this, qreal(start) / refWidthScene, proportion, zUnitPlaying::cpuCores + 0.1 );

	// The space that the call stack released on the top, will be used by the robot
	Q_ASSERT(actor);
	actor->setStartProportion(0.0);
	idleThreads->updateLayoutItem();

	duration += actor->appear();
	return duration;
}

int ExecutionThread::terminate()
{
	// Detach from the previous area of the scene
	int duration = detach();

	// Remove the thread from the scene after the terminate animation is done
	this->removeAllItems(true);

	return duration;
}

int ExecutionThread::detach()
{
	// If already detached, do nothing
	if ( state == threadNew || state == threadTerminated )
		return 0;

	// If thread was active, detach the call stack
	int duration = 0;
	if ( state == threadActive )
	{
		// Detach the actor and the call stack
		Q_ASSERT(callStack);
		duration += callStack->animateDisappear();

		// Release the cpu core
		Q_ASSERT(cpuCore);
		cpuCore->removeItem(this, false);
		cpuCore->setThread(nullptr);
	}
	else
	{
		Q_ASSERT(idleThreads);
		idleThreads->removeItem(this, false);
	}

	// Detach the actor from the cpu or idle zone and
	// Make it disappear after the call stack has disappeared
	duration += actor->disappear(1000, duration);

	// Thread returns to the new state
	state = threadNew;
	cpuCore = nullptr;
	idleThreads = nullptr;

	return duration;
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

const QColor& ExecutionThread::getHighlightColor() const
{
	Q_ASSERT(actor);
	return actor->getHighlightColor();
}

qreal ExecutionThread::getActorReferenceWidth() const
{
	Q_ASSERT(actor);
	return actor->boundingRect().width();
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
	Q_ASSERT(actor);
	actor->updateLineNumber(lineNumber);

	return true;
}

bool ExecutionThread::processFunctionCall(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration)
{
	// The ExecutionThread must be active (must have an assigned CPU core)
	if ( ! isActive() )
	{
		// ToDo: If execution thread is idle in visualization (does not have an assigned CPU core),
		// it should be stopped at inferior through GDB, to avoid it generating more responses.
		qCCritical(logSolutionCrash()) << "ERROR: idle execution thread running a function call. Disable GDB's all-stop mode";
		return false;
	}

	// Animate the door opening in its CPU core
	Q_ASSERT(cpuCore);
	updateMaxDuration( cpuCore->openMemoryInterface() );

	// Update the stack depth value, reported by debugger
	updateCallStackDepth(debuggerCall);

	// For testing only: close the interface after the function is called
	QTimer::singleShot( maxDuration + 2000, cpuCore, SLOT(closeMemoryInterface()) );

	// Animate the function call
	Q_ASSERT(callStack);
	functionName = tree.findNodeTextValue("/frame/func");
	updateMaxDuration( callStack->callFunction(tree) );

	return true;
}

bool ExecutionThread::updateCallStackDepth(GdbCall* debuggerCall)
{
	// Get the number of functions currently being executed by this thread
	GdbItemTree depthResponse;
	if ( debuggerCall->sendGdbCommand(QString("-stack-info-depth"), visExecutionLoop, &depthResponse) == GDB_ERROR )
		return false;

	// GDB response seems:
	//
	//	^done,depth="2"
	//	(gdb)

	bool ok = false;
	callStackDepth = depthResponse.findNodeTextValue("depth").toInt(&ok);
	Q_ASSERT(ok);

	qCCritical(logTemporary, "ExecutionThread[%d]::callStackDepth=%d", id, callStackDepth);
	return true;
}
