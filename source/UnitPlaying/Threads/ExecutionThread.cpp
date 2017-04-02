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

#include <climits>
#include <QTimer>

ExecutionThread::ExecutionThread(size_t startByte, size_t rowSize, size_t maxSize, Scene* scene, int id)
	: LinearLayout(Qt::Vertical)
	, id(id)
	, startByte(startByte)
	, rowSize(rowSize)
	, maxSize(maxSize)
	, scene(scene)
{
	buildExecutionThread();
}

void ExecutionThread::buildExecutionThread()
{
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
	callStack = new CallStack(startByte, rowSize, maxSize, zUnitPlaying::stackFrame, scene);
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
	duration += actor->animateAppear();

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

	duration += actor->animateAppear();
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
	duration += actor->animateDisappear(1000, duration);

	// Thread returns to the new state
	state = threadNew;
	cpuCore = nullptr;
	idleThreads = nullptr;

	return duration;
}

bool ExecutionThread::updateFromDebugger(const GdbTreeNode* threadNode)
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

	// We keep track of some GDB values
	inspectedLevel = threadNode->findTextValue("frame/level").toInt();
	functionName = threadNode->findTextValue("frame/func");

	// If this thread is running a different line of some file that belongs th the player's solution
	// we return true, in order to update the highlighted line in code editor
	return /* locationChanged =*/ updateLocation(threadNode);
}

bool ExecutionThread::updateLocation(const GdbTreeNode* threadNode)
{
	bool locationChanged = false;

	// Only update the execution thread if it is running code that is part of the player's solution
	// E.g: if it is running an external library function, it is considered a black-box and it is
	// not visualized, because it is very likely to have no C/C++ source code (assembler)
	const QString& updatedFilename = threadNode->findTextValue("frame/file");
	if ( filename != updatedFilename )
	{
		// Update to the new filename
		filename = updatedFilename;
		locationChanged = true;
	}

	// If the execution thread stays at the same line, do not reflect any change on the GUI
	int updatedLineNumber = threadNode->findTextValue("frame/line").toInt();
	if ( lineNumber != updatedLineNumber )
	{
		// Update to the new line number
		lineNumber = updatedLineNumber;
		locationChanged = true;
	}

	// If the location (file or line) changed, the thread was updated
	return locationChanged;
}

int ExecutionThread::locationUpdateAccepted()
{
	// Backup the accepted location (being displayed in code editor) before a new update overwrites
	// the location. Code editors need the previous location in order to clear highlited lines
	previousFilename = filename;
	previousLineNumber = lineNumber;

	// Update the line within the robot's display
	Q_ASSERT(actor);
	return actor->updateLineNumber(lineNumber);
}

void ExecutionThread::clearLocation()
{
	// Remove current filename and line number
	filename.clear();
	lineNumber = -1;

	// This method is called before an execution thread will be terminated, therefore there is not
	// need to update the line number to 00 on the robot
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

bool ExecutionThread::callFunction(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration)
{
	// The ExecutionThread must be active (must have an assigned CPU core)
	if ( ! isActive() )
	{
		// ToDo: If execution thread is idle in visualization (does not have an assigned CPU core),
		// it should be stopped at inferior through GDB, to avoid it generating more responses.
		qCCritical(logSolutionCrash()) << "ERROR: idle execution thread running a function call. Disable GDB's all-stop mode";
		return false;
	}

	// Update the stack depth value, reported by debugger
	if ( updateCallStackDepth(debuggerCall) == 0 )
		return false;

	// Animate the door opening in its CPU core
	Q_ASSERT(cpuCore);
	int duration = cpuCore->openMemoryInterface();

	// Animate the function call
	Q_ASSERT(callStack);
	functionName = tree.findNodeTextValue("/frame/func");
	duration += callStack->callFunction(tree, duration);

	// Animate parameter passing and creation of local variables
	duration += createLocalVariables(debuggerCall, "-stack-list-arguments 2 0 0", "stack-args", duration);
	duration += createLocalVariables(debuggerCall, "-stack-list-locals 2", "locals", duration);
	// The two previous gdb calls can be combinated in the following one:
	// duration += createLocalVariables(debuggerCall, "-stack-list-variables 2", "variables");

	// Close the interface after the function is finally called
	QTimer::singleShot( duration, cpuCore, SLOT(closeMemoryInterface()) );

	// Tell caller the duration of the entire animation
	if ( duration > maxDuration )
		maxDuration = duration;

	return true;
}

int ExecutionThread::updateCallStackDepth(GdbCall* debuggerCall)
{
	// Get the number of functions currently being executed by this thread
	GdbItemTree depthResponse;
	if ( debuggerCall->sendGdbCommand("-stack-info-depth", visExecutionLoop, &depthResponse) == GDB_ERROR )
		return -INT_MAX;

	// GDB response seems:
	//
	//	^done,depth="2"
	//	(gdb)

	bool ok = false;
	int newCallStackDepth = depthResponse.findNodeTextValue("depth").toInt(&ok);
	Q_ASSERT(ok);

	// The result value is the difference between the known depth to the new depth
	int result = callStackDepth - newCallStackDepth;
	callStackDepth = newCallStackDepth;

	qCCritical(logTemporary, "ExecutionThread[%d]::callStackDepth=%d", id, callStackDepth);
	return result;
}

int ExecutionThread::createLocalVariables(GdbCall* debuggerCall, const QString& gdbCommand, const QString& gdbRootNodeName, int initialDelay)
{
	// Example of GDB response to `-stack-list-arguments 2 0 0`
	/*
		^done,
		stack-args=
		[
			frame=
			{
				level="0",
				args=
				[
					{
						name="this",
						type="InputArgument * const",
						value="0x60b580 <global_program_name>"
					},
					{
						name="number",
						type="ull",
						value="0"
					},
					{
						name="value",
						type="const char *",
						value="0x406dc6 \"all_inclusive\""
					}
				]
			}
		]
		(gdb)
	*/

	// Get the list of arguments
	// The "2" argument asks GDB to include name, type and value for each parameter
	// The "0 0" argument is for selecting top frame only: /frame/level == 0
	GdbItemTree gdbResponseTree;
	if ( debuggerCall->sendGdbCommand(gdbCommand, visExecutionLoop, &gdbResponseTree) == GDB_ERROR )
		return -1;

	// Get the array of variables
	const GdbTreeNode* gdbVariableArray = gdbResponseTree.findNode(gdbRootNodeName);
	Q_ASSERT(gdbVariableArray);

	// If gdb produced a frame node, the list of variables is inside it
	const GdbTreeNode* argFrame = gdbVariableArray->findNode("#1/args");
	if ( argFrame )
		gdbVariableArray = argFrame;

	// Parameter passing is done by the callStack object
	return callStack->createLocalVariables(gdbVariableArray, this->id, initialDelay);
}
