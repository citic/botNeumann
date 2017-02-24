#include "Visualizator.h"
#include "CtagsCall.h"
#include "DebuggerBreakpoint.h"
#include "GdbCall.h"
#include "GuiBreakpoint.h"
#include "LogManager.h"
#include "MessagesArea.h"
#include "PlayerSolution.h"
#include "UnitPlayingScene.h"
#include "Util.h"
#include "VariableMapper.h"
#include "VisualizationSpeed.h"



// Construction -----------------------------------------------------------------------------------

Visualizator::Visualizator(PlayerSolution* playerSolution, int testCase, UnitPlayingScene* unitPlayingScene)
	: GdbResponseListener(unitPlayingScene)
	, playerSolution(playerSolution)
	, testCaseNumber(testCase)
	, unitPlayingScene(unitPlayingScene)
{
}

Visualizator::~Visualizator()
{
	for ( int index = 0; index < debuggerBreakpoints.size(); ++index )
		delete debuggerBreakpoints[index];

	delete debuggerCall;
	delete entryPointTree;
	delete variableMapper;
}



// Start visualization ---------------------------------------------------------------------------

bool Visualizator::start(bool preparation)
{
	if ( preparation )
	{
		// Start GDB
		if ( ! createGdb() ) return false;
		if ( ! startGdb() ) return false;

		// Do the preparation phase
		if ( ! setInferiorAndArguments() ) return false;
		setUserDefinedBreakpoints();
		setFunctionDefinitionBreakpoints();
		if ( ! startInferior() ) return false;
	}
	else
	{
		setDynamicMemoryBreakpoints();
		watchStandardInputOutput();
		watchGlobalVariables();
	}

	// Visualization started
	return true;
}

bool Visualizator::createGdb()
{
	// Create the object in charge of communicating with GDB
	Q_ASSERT(debuggerCall == nullptr);
	debuggerCall = new GdbCall(this);
	if ( debuggerCall == nullptr )
		return false;

	// Connect events
	// When there is a pending GdbResponse process it
	connect( debuggerCall, SIGNAL(pendingGdbResponses()), this, SLOT(processGdbResponse()) );
	// Each time an animation is done, process the next GdbResponse, if any
	connect( &animationDone, SIGNAL(timeout()), this, SLOT(processGdbResponse()));
	animationDone.setSingleShot(true);
	// This object also processes GdbResponses
	connect( this, SIGNAL(dispatchGdbResponse(const GdbResponse*,int&)), this, SLOT(onGdbResponse(const GdbResponse*,int&)) );

	// Create the object that will map variables between player solution and visualization
	Q_ASSERT(variableMapper == nullptr);
	variableMapper = new VariableMapper(debuggerCall, this);
	return true;
}

bool Visualizator::startGdb()
{
	if ( ! debuggerCall->start() )
	{
		qCritical(logVisualizator(), "Could not start gdb\n");
		return false;
	}
	qCDebug(logVisualizator(), "gdb started");
	return true;
}

bool Visualizator::setInferiorAndArguments()
{
	// Ask GDB run user program
	const QString executablePath = playerSolution->getExecutablePath();
	if ( debuggerCall->sendGdbCommand( QString("-file-exec-and-symbols \"%1\"").arg(executablePath), visStarting ) == GDB_ERROR )
	{
		qCritical(logVisualizator(), "Failed to run user program: '%s'", qPrintable(executablePath));
		return false;
	}

	// Give inferior parameters to GDB
	return debuggerCall->sendGdbCommand( QString("-exec-arguments %1").arg( buildInferiorArguments() ), visStarting ) != GDB_ERROR;
}

bool Visualizator::setUserDefinedBreakpoints()
{
	// Get current user-defined breakpoints on GUI, if any as strings "filename:lineNumber"
	const QList<GuiBreakpoint*>& editorBreakpoints = unitPlayingScene->retrieveBreakpoints();
	foreach(const GuiBreakpoint* guiBreakpoint, editorBreakpoints)
	{
		const QString& originalLocation = guiBreakpoint->buildOriginalLocation();
		if ( debuggerCall->sendGdbCommand( "-break-insert " + originalLocation, visUserDefinedBreakpoint ) == GDB_ERROR )
			qCWarning( logVisualizator, "Error: -break-insert %s", qUtf8Printable(originalLocation) );
	}

	// If user defined at least one breakpoint, animation must seek until reach the first one
	VisualizationSpeed::getInstance().setSeeking( editorBreakpoints.count() > 0 );
	return true;
}

bool Visualizator::setFunctionDefinitionBreakpoints()
{
	// Get all function definitions from player solution
	Q_ASSERT(playerSolution);
	CtagsCall* ctagsCall = playerSolution->getCtagsCall();

	// If the Ctags call failed, the pointer is null and no symbols are not extracted
	if ( ctagsCall == nullptr )
		return false;

	// Get the function definitions symbols and set breakpoints for all of them
	const QList<Symbol*>& functionDefinitions = ctagsCall->getFunctionDefinitions();
	for ( int index = 0; index < functionDefinitions.count(); ++index )
	{
		const Symbol* symbol = functionDefinitions[index];
		const QString& fileLine = QString("\"%1:%2\"").arg(symbol->filename).arg(symbol->line);
		if ( debuggerCall->sendGdbCommand("-break-insert " + fileLine, visFunctionDefinition) == GDB_ERROR )
		{
			qCCritical(logVisualizator).noquote() << "Failed to set function definition breakpoint at" << fileLine;
			return false;
		}
	}

	return true;
}

bool Visualizator::startInferior()
{
	// Ask GDB to start execution of user program (inferior), only if it is not running already
	if ( gdbState == STATE_STARTING )
	{
		qCritical(logVisualizator(), "gdb already starting");
		return false;
	}

	if ( gdbState == STATE_RUNNING )
	{
		qCritical(logVisualizator(), "gdb already running");
		return false;
	}

	inferiorProcessId = 0;
	GdbState oldState = gdbState;
	gdbState = STATE_STARTING;
	if ( debuggerCall->sendGdbCommand("-exec-run --start", visStarting) == GDB_ERROR )
	{
		gdbState = oldState;
		return false;
	}

	return true;
}

bool Visualizator::setDynamicMemoryBreakpoints()
{
	// Set breakpoint for the dynamic memory management functions: malloc, calloc, realloc, and free
	// These breakpoints are set after the libc library has been loaded.
	if ( debuggerCall->sendGdbCommand("-break-insert -f __libc_malloc", visMallocBreakpoint) == GDB_ERROR ) return false;
	if ( debuggerCall->sendGdbCommand("-break-insert -f __libc_calloc", visCallocBreakpoint) == GDB_ERROR ) return false;
	if ( debuggerCall->sendGdbCommand("-break-insert -f __libc_realloc", visReallocBreakpoint) == GDB_ERROR ) return false;
	if ( debuggerCall->sendGdbCommand("-break-insert -f __libc_free", visFreeBreakpoint) == GDB_ERROR ) return false;

	return true;
}

bool Visualizator::watchStandardInputOutput()
{
	// Create object variables watching changes in IO, using notation bn_io_file
	return variableMapper->createWatch("stdin->_IO_read_ptr", "bn_io_stdin", MemoryBlock::standardInputOutput )
		&& variableMapper->createWatch("stdout->_IO_write_ptr", "bn_io_stdout", MemoryBlock::standardInputOutput )
		&& variableMapper->createWatch("stderr->_IO_write_ptr", "bn_io_stderr", MemoryBlock::standardInputOutput );
}

bool Visualizator::watchGlobalVariables()
{
	// Get the list of global variables from player solution
	Q_ASSERT(playerSolution);
	CtagsCall* ctagsCall = playerSolution->getCtagsCall();

	// If the Ctags call failed, the pointer is null and no symbols are not extracted
	if ( ctagsCall == nullptr )
		return false;

	// Get the global variable declarations and set watches for all of them
	const QList<Symbol*>& globalVariables = ctagsCall->getGlobalVariables();
	for ( int index = 0; index < globalVariables.count(); ++index )
	{
		const Symbol* symbol = globalVariables[index];
		const QString watchName = QString("bn_gv_%1").arg(index + 1);
		variableMapper->createWatch(symbol->name, watchName, MemoryBlock::globalVariable);
	}

	return true;
}

QString Visualizator::buildInferiorArguments()
{
	// Send test case arguments
	Q_ASSERT(playerSolution);
	const QString& args = playerSolution->buildTestCaseFilepath(testCaseNumber, "args");
	const QStringList& testCaseArgs = Util::readAllLines(args);

	// We need to join all arguments separating them by spaces
	QString result;
	if ( testCaseArgs.count() > 0 )
		result = '"' + testCaseArgs.join("\" \"") + '"';

  #ifndef Q_OS_WIN
	// In Unix-like operating systems, GDB passes arguments to inferior using a shell.
	// The shell is able to redirect standard input, output and error in arguments

	// Get the full path to these files
	const QString& input = playerSolution->buildTestCaseFilepath(testCaseNumber, "input");
	const QString& output_ps = playerSolution->buildTestCaseFilepath(testCaseNumber, "output_ps");
	const QString& error_ps = playerSolution->buildTestCaseFilepath(testCaseNumber, "error_ps");

	// Redirect standard input, output and error for player solution
	if ( ! result.isEmpty() ) result += ' ';
	result += "< \"" + input + '"';
	result += " > \"" + output_ps + '"';
	result += " 2> \"" + error_ps + '"';
  #endif

	return result;
}



// Control the visualization ---------------------------------------------------------------------

bool Visualizator::stop()
{
	// Stop the GdbResult fetching mechanism
	animationDone.stop();

	// Removing elements from the scene must be done at-once
	VisualizationSpeed::getInstance().setSeeking(true);

	// Stop gdb
	return debuggerCall->sendGdbCommand("-exec-interrupt", visStopping) != GDB_ERROR;
}

bool Visualizator::pause()
{
	// Just stop the GdbResult fetching mechanism. This is not necessary because the
	// processGdbResponse() stops when visualization is paused
	animationDone.stop();
	return true;
}

bool Visualizator::resume()
{
	// Continue processing the next available GdbResult
	processGdbResponse();
	return true;
}

bool Visualizator::step(const QString& gdbCommand, const QString& description)
{
	Q_ASSERT(debuggerCall);
	qCInfo(logPlayer) << description;
	if ( debuggerCall->sendGdbCommand(gdbCommand, visExecutionLoop) == GDB_ERROR )
	{
		qCCritical(logVisualizator) << "Error sending" << description << "command";
		return false;
	}

	inStep = true;
	processGdbResponse();
	return true;
}



// Responses received by GdbCall ------------------------------------------------------------------

void Visualizator::processGdbResponse()
{
//	static long long callCount = 0;
//	qCDebug(logVisualizator, "====processGdbResponse(%lld)", ++callCount);

	// If there is an active animation, wait until it is done
	if ( animationDone.remainingTime() > 0 )
		return;

	// The last animation is finished, stop its timer
	animationDone.stop();

//	qCDebug(logVisualizator, "++++processGdbResponse(%lld) animation done", callCount);

	// If animation is paused, do not animate
	if ( unitPlayingScene->getState() == UnitPlayingState::paused && inStep == false )
		return;

	// Fetch the next pending response
	Q_ASSERT(debuggerCall);
	GdbResponse* gdbResponse = debuggerCall->takeNextResponse();

	// If there is no pending responses, we are done
	if ( gdbResponse == nullptr )
	{
		inStep = false;
		return;
	}

	// Notify all actors to animate this response, they will inform how many milliseconds they
	// will take to complete the animation
	int maxDuration = 0;
	emit dispatchGdbResponse(gdbResponse, maxDuration);

	// Wait until the animation is done, then call this method again to process the next pending
	// response
	animationDone.start(maxDuration);
//	qCDebug(logVisualizator, "----processGdbResponse(%lld) next in %dms", callCount, maxDuration);
}

void Visualizator::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onExecAsyncOut(%s) %s | ctx=%d", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass), context);

	switch ( asyncClass )
	{
		// *running,thread-id="all"
		case AsyncClass::AC_RUNNING:
			gdbState = STATE_RUNNING;
			break;

		// *stopped,reason="text",...
		case AsyncClass::AC_STOPPED:
			gdbState = STATE_STOPPED;
			processPlayerSolutionStopped(tree, context, maxDuration);
			break;

		default:
			break;
	}
}

void Visualizator::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onStatusAsyncOut(%s) %s | ctx=%d", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass), context);
}

void Visualizator::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	Q_ASSERT(debuggerCall);
	qCDebug(logTemporary, "onNotifyAsyncOut(%s) %s | ctx=%d", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass), context);
	const GdbTreeNode* node = nullptr;

	switch ( asyncClass )
	{
		case AsyncClass::AC_THREAD_GROUP_STARTED:
			// Get the process id of the user program
			inferiorProcessId = tree.findNodeTextValue("/pid").toInt();
			break;

		case AsyncClass::AC_THREAD_CREATED:
			debuggerCall->sendGdbCommand("-thread-info", visExecutionLoop);
			break;

		case AsyncClass::AC_BREAKPOINT_CREATED:
		case AsyncClass::AC_BREAKPOINT_MODIFIED:
			if ( ( node = tree.findNode("/bkpt") ) )
				return updateDebuggerBreakpoint( node, context );
			break;

		case AsyncClass::AC_BREAKPOINT_DELETED:
			if ( ( node = tree.findNode("/bkpt") ) )
				return deleteDebuggerBreakpoint( node );
			break;

		default:
			break;
	}
}

void Visualizator::onResult(const GdbItemTree& tree, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onResult(%s) | ctx=%d", qPrintable(tree.buildDescription()), context);
	const GdbTreeNode* node = nullptr;

	// If this is the result of inserting or removing a breakpoint
	if ( ( node = tree.findNode("/bkpt") ) )
		return updateDebuggerBreakpoint( node, context );
}

void Visualizator::onConsoleStreamOutput(const QString& text, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
	QStringList lines = text.split('\n');
	for ( int lineIndex = 0; lineIndex < lines.size(); ++lineIndex )
	{
		const QString& line = lines[lineIndex];
		if ( ! line.isEmpty() )
			unitPlayingScene->getMessagesArea()->appendDebuggerMessage(QtInfoMsg, "", line);
	}
}

void Visualizator::onTargetStreamOutput(const QString& str, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onTargetStreamOutput(%s) | ctx=%d", qPrintable(str), context);
}

void Visualizator::onLogStreamOutput(const QString& str, VisualizationContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onLogStreamOutput(%s) | ctx=%d", qPrintable(str), context);
}


// Reacting to responses --------------------------------------------------------------------------

bool Visualizator::processPlayerSolutionStopped(const GdbItemTree& tree, VisualizationContext context, int& maxDuration)
{
	// Player solution stopped execution. GDB reported: *stopped,reason="text",...
	// The list of reasons is documented in GDB/MI
	const QString& reason = tree.findNodeTextValue("/reason");

	// A breakpoint was reached.
	if ( reason == "breakpoint-hit" ) return processBreakpointHit(tree, context, maxDuration);
	// A watchpoint was triggered.
	//if ( reason == "watchpoint-trigger" ) return processWatchpointTrigger(tree, context, maxDuration);
	// A read watchpoint was triggered.
	//if ( reason == "read-watchpoint-trigger" ) return processReadWatchpointTrigger(tree, context, maxDuration);
	// An access watchpoint was triggered.
	//if ( reason == "access-watchpoint-trigger" ) return processAccessWatchpointTrigger(tree, context, maxDuration);
	// An -exec-finish or similar CLI command was accomplished.
	//if ( reason == "function-finished" ) return processFunctionFinished(tree, context, maxDuration);
	// An -exec-until or similar CLI command was accomplished.
	//if ( reason == "location-reached" ) return processLocationReached(tree, context, maxDuration);
	// A watchpoint has gone out of scope.
	//if ( reason == "watchpoint-scope" ) return processWatchpointScope(tree, context, maxDuration);
	// -exec-next/-exec-next-instruction/-exec-step/-exec-step-instruction was acc
	//if ( reason == "end-stepping-range" ) return processEndSteppingRange(tree, context, maxDuration);
	// The inferior exited because of a signal.
	//if ( reason == "exited-signalled" ) return processExitedSignalled(tree, context, maxDuration);
	// The inferior exited.
	//if ( reason == "exited" ) return processExited(tree, context, maxDuration);
	// The inferior exited normally.
	//if ( reason == "exited-normally" ) return processExitedNormally(tree, context, maxDuration);
	// A signal was received by the inferior.
	//if ( reason == "signal-received" ) return processSignalReceived(tree, context, maxDuration);
	// Inferior stopped due to a library being loaded or unloaded (catch un/load).
	//if ( reason == "solib-event" ) return processSolibEvent(tree, context, maxDuration);
	// The inferior has forked and a fork catchpoint was triggered
	//if ( reason == "fork" ) return processFork(tree, context, maxDuration);
	// The inferior has vforked and a catchpoint was used.
	//if ( reason == "vfork" ) return processVfork(tree, context, maxDuration);
	// The inferior entered a system call and a catchpoint was used.
	//if ( reason == "syscall-entry" ) return processSyscallEntry(tree, context, maxDuration);
	// The inferior returned from a system call and a catchpoint was used.
	//if ( reason == "syscall-return" ) return processSyscallReturn(tree, context, maxDuration);
	// The inferior called exec and a catchpoint was used.
	//if ( reason == "exec" ) return processExec(tree, context, maxDuration);

	// Gede gets pid asking the list of threads, we got pid from AC_THREAD_GROUP_STARTED
	// if (inferiorProcessId == 0) debuggerCall->sendGdbCommand("-list-thread-groups");
	// These are Gede commands:
	//debuggerCall->sendGdbCommand("-thread-info", visExecutionLoop);
	//debuggerCall->sendGdbCommand("-var-update --all-values *", visExecutionLoop);
	//debuggerCall->sendGdbCommand("-stack-list-locals 1", visExecutionLoop);
	return false;
}

bool Visualizator::processBreakpointHit(const GdbItemTree& tree, VisualizationContext context, int& maxDuration)
{
	// Program stopped because a breakpoint was hit. An example of tree:

	//	*stopped,
	//	reason="breakpoint-hit",
	//	disp="keep",
	//	bkptno="2",
	//	frame=
	//	{
	//		addr="0x00000000004027c3",
	//		func="InputArgument::InputArgument",
	//		args=
	//		[
	//			{
	//				name="this",
	//				value="0x60b580 <global_program_name>"
	//			},
	//			{
	//				name="number",
	//				value="0"
	//			},
	//			{
	//				name="value",
	//				value="0x406dc6 \"all_inclusive\""
	//			}
	//		],
	//		file="alli.cpp",
	//		fullname="/tmp/alli/alli.cpp",
	//		line="64"
	//	},
	//	thread-id="1",
	//	stopped-threads="all",
	//	core="6"
	//	(gdb)

	// Get the number of the breakpoint
	bool ok = false;
	int breakpointNumber = tree.findNodeTextValue("/bkptno").toInt(&ok);

	// Get the breakpoint object that is identified by that number
	Q_ASSERT(ok);
	Q_ASSERT(breakpointNumber >= 0 && breakpointNumber < debuggerBreakpoints.count() );
	DebuggerBreakpoint* breakpoint = debuggerBreakpoints[breakpointNumber];
	Q_ASSERT(breakpoint);

	// If visualization is in starting state and entryPointTree is null process Program entry point:
	// if ( unitPlayingScene->getState() == UnitPlayingState::starting )
	if ( context == visStarting )
		return processEntryPoint(tree, breakpoint, maxDuration);

	// If breakpoint object has one or more roles:
	// * If breakpoint is functionBody or programEntryPoint: Do 4.2 Function call.
	// * If breakpoint is userDefined: Do 4.4 User defined breakpoint.
	// * Do 4.5 Dynamic memory management breakpoint
	return false;
}

bool Visualizator::processEntryPoint(const GdbItemTree& tree, DebuggerBreakpoint* breakpoint, int& maxDuration)
{
	// Visualization is in starting stage, not animating. Inferior is stopped by a breakpoint hit.
	// This is the first breakpoint hit, threfore, it is the actual program entry point.
	// GDB always set the program entry point breakpoint to the main() function, but it may fail
	// in C++ when there are global objects that call constructors before main().
	Q_UNUSED(maxDuration);

	// Because there are pending initialization steps, do not animate a function call.
	// We store the GDB response tree for later use
	Q_ASSERT(this->entryPointTree == nullptr);
	this->entryPointTree = new GdbItemTree(tree);
	qCDebug(logTemporary) << "Program entry point tree saved for later:" << entryPointTree->buildDescription();

	// Add the pogramEntryPoint role to breakpoints[/bkptno].
	Q_ASSERT(breakpoint);
	breakpoint->addRole( DebuggerBreakpoint::programEntryPoint );

	// Continue the starting process. It will eventually animate a function call
	return start(false);
}



// Breakpoints ------------------------------------------------------------------------------------

void Visualizator::breakpointAction(GuiBreakpoint* guiBreakpoint)
{
	// Setting and removing breakpoints from CodeEditor
	// Only update breakpoints if there is an active debugger call
	Q_ASSERT(guiBreakpoint);
	if ( debuggerCall == nullptr  )
		return;

	// If breakpoints in code editor are not synchronized with debuger code, dot not continue
	if ( guiBreakpoint->getLineNumberInObjectCode() == -1 )
		return;

	// Check if the action is creation or deletion of a breakpoint
	switch ( guiBreakpoint->getAction() )
	{
		case GuiBreakpoint::Action::unknown:
			Q_ASSERT(false);
			break;

		case GuiBreakpoint::Action::created:
			debuggerCall->sendGdbCommand( QString("-break-insert %1").arg(guiBreakpoint->buildOriginalLocation()), visUserDefinedBreakpoint );
			break;

		case GuiBreakpoint::Action::removed:
			int breakpointNumber = findDebuggerBreakpointIndex( *guiBreakpoint );
			if ( breakpointNumber > 0 && debuggerCall->sendGdbCommand( QString("-break-delete %1").arg(breakpointNumber), visUserDefinedBreakpoint ) != GDB_ERROR )
			{
				delete debuggerBreakpoints[breakpointNumber];
				debuggerBreakpoints[breakpointNumber] = nullptr;
			}
			break;
	}
}

int Visualizator::findDebuggerBreakpointIndex(const GuiBreakpoint& guiBreakpoint) const
{
	// For all debugger breakpoints, find the one that matches the GUI breakpoint and return its index
	for ( int index = 0; index < debuggerBreakpoints.size(); ++index )
		if ( debuggerBreakpoints[index] && guiBreakpoint.matches( *debuggerBreakpoints[index] ) )
			return index;

	// Not found
	return -1;
}

void Visualizator::updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode, VisualizationContext context)
{
	// Create a debugger breakpoint that parsers the output sent by debugger
	Q_ASSERT(breakpointNode);
	DebuggerBreakpoint* debuggerBreakpoint = new DebuggerBreakpoint( *breakpointNode );

	// Check if the breakpoint already exists in our vector
	int breakpointNumber = debuggerBreakpoint->getNumber();
	if ( breakpointNumber < debuggerBreakpoints.count() && debuggerBreakpoints[breakpointNumber] != nullptr )
	{
		// The breakpoint already exists in our vector
		// We preserve the roles from the old object in the updated one
		debuggerBreakpoint->setRoles( debuggerBreakpoint->getRoles() | debuggerBreakpoints[breakpointNumber]->getRoles() );

		// We update the breakpoint by replacing the old one by the new one
		delete debuggerBreakpoints[breakpointNumber];
		debuggerBreakpoints[breakpointNumber] = debuggerBreakpoint;
	}
	else
	{
		// The breakpoint does not exist in our vector, add it using the breakpoint number as its index
		debuggerBreakpoints.resize( qMax(breakpointNumber + 1, debuggerBreakpoints.size()) );
		debuggerBreakpoints[breakpointNumber] = debuggerBreakpoint;
	}

	// Update the role of the breakpoint according to the context it was created
	debuggerBreakpoint->addRoleFor(context);

	debuggerBreakpoint->print();
	// Update the interface?
	//	emit breakpointUpdated( debuggerBreakpoints[breakpointNumber] );
}

void Visualizator::deleteDebuggerBreakpoint(const GdbTreeNode* breakpointNode)
{
	// Get the number of breakpoint that was deleted
	Q_ASSERT(breakpointNode);
	int breakpointNumber = breakpointNode->findTextValue("number").toInt();

	// Simply remove the object for that breakpoint
	Q_ASSERT(breakpointNumber < debuggerBreakpoints.count() );
	delete debuggerBreakpoints[breakpointNumber];

	// Mark the breakpoint entry as null
	debuggerBreakpoints[breakpointNumber] = nullptr;
	qCDebug(logApplication) << "Breakpoint deleted:" << breakpointNumber;
}
