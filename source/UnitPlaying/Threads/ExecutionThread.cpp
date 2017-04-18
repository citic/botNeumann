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
#include "StaticAnalysisGlobals.h"

#include <climits>
#include <QRegularExpression>
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
	callStack = new CallStack(id, startByte, rowSize, maxSize, zUnitPlaying::stackFrame, scene);
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
	actor->setMainStartProportion(0.9);
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
	actor->setMainStartProportion(0.0);
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

bool ExecutionThread::updateLocation(const GdbTreeNode* threadNode, int breakpointLineNumber)
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

	// We prefer to use breakpoint lines than adjusted GDB lines
	int updatedLineNumber = breakpointLineNumber;
	if ( updatedLineNumber <= 0 )
		updatedLineNumber = threadNode->findTextValue("frame/line").toInt();

	// If the execution thread stays at the same line, do not reflect any change on the GUI
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
//	qCInfo(logTemporary(), "ExecutionThread(%d) line=[%s] filename=%s:%d-%d", id, qPrintable( loadRunningLine(false) ), qPrintable(filename), lineNumber, previousLineNumber );
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

int ExecutionThread::isWaitingForIO()
{
	// GDB does not provide any way to know if a thread issued a input/output operation
	// As a workaround we do static code analysis. If thread is running a line that contains a
	// call to some standard library function to read or print, it is likely to be running an input
	// or output operation. Notice that macros or other libraries may deceive this detection system

	// Get the lines being executed
	const QString& line = loadRunningLine(false);

	// ToDo: a line may have two or more stdin/stdout operations
	// Check for any input function call
	for ( size_t index = 0; index < stdInputTokensSize; ++index )
		if ( line.contains(QRegularExpression("\\b" + QString(stdInputTokens[index]) + "\\b")) )
			return 1;

	// Check for any output function call
	for ( size_t index = 0; index < stdOutputTokensSize; ++index )
		if ( line.contains(QRegularExpression("\\b" + QString(stdOutputTokens[index]) + "\\b")) )
			return 2;

	return 0;
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

	// We are going to ask a new command to gdb, and tree will be destroyed. Save a copy
	GdbItemTree breakpointHitTree = tree;

	// Animate the door opening in its CPU core
	Q_ASSERT(cpuCore);
	int duration = cpuCore->openMemoryInterface();

	// The tree has the new current location of the thread
	updateLocation( breakpointHitTree.getRoot() );

	// Animate the function call
	Q_ASSERT(callStack);
	functionName = breakpointHitTree.findNodeTextValue("/frame/func");
	duration += callStack->callFunction(breakpointHitTree, duration);

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

bool ExecutionThread::checkForFunctionCallOrReturn(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration, bool checkCall)
{
	// If we are running on the same function, do not animate a function return
	// ToDo: it may have return several functions
	int depthChange = updateCallStackDepth(debuggerCall);
	if ( depthChange > -INT_MAX && depthChange < 0 )
		return returnFunction(debuggerCall, maxDuration);

	// If actually the stack depth increased, animate a function call
	if ( checkCall && depthChange > 0 )
		return callFunction(tree, debuggerCall, maxDuration);

	// depthChange == 0
	return false;
}

bool ExecutionThread::returnFunction(GdbCall* debuggerCall, int& maxDuration)
{
	// If there are no remaining function calls, execute `-exec-continue`. An execution thread may
	// end with no stack frames (function calls). This is possible because we are not animating all
	// function calls. Library functions may be executing. For example GNU libc++ runs
	// __static_initialization_and_destruction_0 to call global objects' constructors before main().
	if ( callStack->isEmpty() )
		return debuggerCall->sendGdbCommand("-exec-continue", visExecutionLoop) != GDB_ERROR;

	// Animate the door opening in its CPU core
	Q_ASSERT(cpuCore);
	int duration = cpuCore->openMemoryInterface();

	// Animate stack frame being dropped through the CPU core’s memory interface
	Q_ASSERT(callStack);
	duration += callStack->returnFunction(duration);

	// ToDo: If using pointed data variable objects bn_pd_th_fc, remove all of the for function call
	// fc index running by execution thread th. Example:

	// -var-delete bn_pd_1_2
	// ^done,ndeleted="1"
	// (gdb)

	// If there are remaining function calls, make them to move a step towards the robot, and
	// return to the 5. Execution loop

	// Close the interface after the function is finally called
	QTimer::singleShot( duration, cpuCore, SLOT(closeMemoryInterface()) );

	// Tell caller the duration of the entire animation
	if ( duration > maxDuration )
		maxDuration = duration;

	// We must update the stack depth after a function return
	int difference = updateCallStackDepth(debuggerCall);
	qCInfo(logTemporary(), "Thread %d: function returned. Call stack depth: %d. Diff %d", id, callStackDepth, difference);
	Q_ASSERT(difference <= 0);

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
	int result = newCallStackDepth - callStackDepth;
	qCCritical(logTemporary, "ExecutionThread[%d]::callStackDepth from %d to %d (%+d)", id, callStackDepth, newCallStackDepth, result);

	callStackDepth = newCallStackDepth;
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
	return callStack->createLocalVariables(gdbVariableArray, initialDelay);
}

QString ExecutionThread::loadRunningLine(bool fromPreviousLine) const
{
	// Check we have the enough data to locate the line or lines
	if ( lineNumber <= 0 ) return "";
	if ( filename.isEmpty() ) return "";

	// We have the first line number, get the last one
	int lastLine = lineNumber;
	if ( fromPreviousLine && previousLineNumber < lineNumber )
		lastLine = previousLineNumber;

	// Open the file
	QFile inputFile(filename);
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == false )
		{ qCritical(logApplication) << "ExecutionThread: Could not open" << filename; return ""; }

	// Concatenate the desired lines to this string
	QString result;

	// Read until reach the expected line
	QTextStream inputCode(&inputFile);
	for ( int currentLine = 1; ! inputCode.atEnd(); ++currentLine )
	{
		// Get a line from the source resource
		const QString& line = inputCode.readLine();
//		qCritical(logApplication, "LINE %s:%d [%s]", qPrintable(filename), currentLine, qPrintable(line));

		// The line is done, write it to the target file
		if ( currentLine >= lineNumber && currentLine <= lastLine )
			result += line + '\n';
	}

	// Done
	return result;
}
