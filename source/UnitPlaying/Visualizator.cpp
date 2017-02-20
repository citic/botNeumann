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
#include "VisualizationSpeed.h"

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
}

bool Visualizator::start()
{
	Q_ASSERT(debuggerCall == nullptr);
	debuggerCall = new GdbCall(this);

	// Connect events
	// When there is a pending GdbResponse process it
	connect( debuggerCall, SIGNAL(pendingGdbResponses()), this, SLOT(processGdbResponse()) );
	// Each time an animation is done, process the next GdbResponse, if any
	connect( &animationDone, SIGNAL(timeout()), this, SLOT(processGdbResponse()));
	animationDone.setSingleShot(true);
	// This object also processes GdbResponses
	connect( this, SIGNAL(dispatchGdbResponse(const GdbResponse*,int&)), this, SLOT(onGdbResponse(const GdbResponse*,int&)) );

	// Start GDB
	if ( ! startGdb() ) return false;
	// Do the preparation phase
	if ( ! setInferiorAndArguments() ) return false;
	setUserDefinedBreakpoints();
	setFunctionDefinitionBreakpoints();
	if ( ! startInferior() ) return false;

	// Visualization started
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

// Setting and removing breakpoints from CodeEditor

void Visualizator::breakpointAction(GuiBreakpoint* guiBreakpoint)
{
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

int Visualizator::findDebuggerBreakpointIndex(const GuiBreakpoint& guiBreakpoint) const
{
	// For all debugger breakpoints, find the one that matches the GUI breakpoint and return its index
	for ( int index = 0; index < debuggerBreakpoints.size(); ++index )
		if ( debuggerBreakpoints[index] && guiBreakpoint.matches( *debuggerBreakpoints[index] ) )
			return index;

	// Not found
	return -1;
}

// Responses received by GdbCall

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

void Visualizator::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onExecAsyncOut(%s) %s | ctx=%d", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass), context);

	switch ( asyncClass )
	{
		case AsyncClass::AC_RUNNING:
			gdbState = STATE_RUNNING;
			break;

		case AsyncClass::AC_STOPPED:
			gdbState = STATE_STOPPED;
			// Gede gets pid asking the list of threads, we got pid from AC_THREAD_GROUP_STARTED
			// if (inferiorProcessId == 0) debuggerCall->sendGdbCommand("-list-thread-groups");

			// ToDo: for first breakpoint-hit, save tree
			// ToDo: These are Gede commands. Check against session example. Contexts may be wrong
			debuggerCall->sendGdbCommand("-thread-info", visExecutionLoop);
			debuggerCall->sendGdbCommand("-var-update --all-values *", visExecutionLoop);
			debuggerCall->sendGdbCommand("-stack-list-locals 1", visExecutionLoop);
			break;

		default:
			break;
	}
/*
	// Get the current thread id
	const QString& threadIdStr = tree.findNodeTextValue("/thread-id");
	if ( threadIdStr.isEmpty() == false )
	{
		bool ok = false;
		int threadId = threadIdStr.toInt(&ok, 0);
		if ( ok ) emit currentThreadChanged(threadId);
	}
*/
}

void Visualizator::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onStatusAsyncOut(%s) %s | ctx=%d", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass), context);
}

void Visualizator::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
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

void Visualizator::onResult(const GdbItemTree& tree, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onResult(%s) | ctx=%d", qPrintable(tree.buildDescription()), context);
	const GdbTreeNode* node = nullptr;

	if ( ( node = tree.findNode("/bkpt") ) )
		return updateDebuggerBreakpoint( node, context );
}

void Visualizator::onConsoleStreamOutput(const QString& text, VisualizatorContext context, int& maxDuration)
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

void Visualizator::onTargetStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onTargetStreamOutput(%s) | ctx=%d", qPrintable(str), context);
}

void Visualizator::onLogStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logTemporary, "onLogStreamOutput(%s) | ctx=%d", qPrintable(str), context);
}


void Visualizator::updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode, VisualizatorContext context)
{
	// Create a debugger breakpoint that parsers the output sent by debugger
	Q_ASSERT(breakpointNode);
	DebuggerBreakpoint* debuggerBreakpoint = new DebuggerBreakpoint( *breakpointNode );

	// Check if the breakpoint already exists in our vector
	int breakpointNumber = debuggerBreakpoint->getNumber();
	if ( breakpointNumber < debuggerBreakpoints.count() && debuggerBreakpoints[breakpointNumber] != nullptr )
	{
		// The breakpoint already exists in our vector, update it by replacing the old one by the new one
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
	switch ( context )
	{
		case visUserDefinedBreakpoint: debuggerBreakpoint->addRole( DebuggerBreakpoint::userDefined ); break;
		case visFunctionDefinition: debuggerBreakpoint->addRole( DebuggerBreakpoint::functionDefinition ); break;
		case visProgramEntryPoint: debuggerBreakpoint->addRole( DebuggerBreakpoint::programEntryPoint ); break;
		default: break;
	}

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
