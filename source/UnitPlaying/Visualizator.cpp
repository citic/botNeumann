#include "Visualizator.h"
#include "DebuggerBreakpoint.h"
#include "GdbCall.h"
#include "GuiBreakpoint.h"
#include "LogManager.h"
#include "MessagesArea.h"
#include "UnitPlayingScene.h"

Visualizator::Visualizator(const QFileInfo& executablePath, UnitPlayingScene* unitPlayingScene)
	: GdbResponseListener(unitPlayingScene)
	, executablePath(executablePath)
	, debuggerCall( nullptr )
	, unitPlayingScene(unitPlayingScene)
	, inferiorProcessId(0)
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
	connect( debuggerCall, SIGNAL(onGdbLogMessage(GdbLogType,QString)), unitPlayingScene->getMessagesArea(), SLOT(appendDebuggerMessage(GdbLogType,QString)));
	// Each time an animation is done, process the next GdbResponse, if any
	connect( &animationDone, SIGNAL(timeout()), this, SLOT(processGdbResponse()));
	animationDone.setSingleShot(true);
	// This object also processes GdbResponses
	connect( this, SIGNAL(dispatchGdbResponse(const GdbResponse*,int&)), this, SLOT(onGdbResponse(const GdbResponse*,int&)) );

	bool result = debuggerCall->start();
	if ( ! result )
	{
		qCritical(logVisualizator(), "Could not start gdb\n");
		return false;
	}

	qCDebug(logVisualizator(), "gdb started");

	// Ask GDB run user program
	if ( debuggerCall->sendGdbCommand(QString("-file-exec-and-symbols \"%1\"").arg(executablePath.filePath())) == GDB_ERROR )
		qCritical(logVisualizator(), "Failed to run user program: '%s'", qPrintable(executablePath.filePath()));

	// Give inferior parameters to GDB
	if ( userProgramArguments.length() > 0 )
		debuggerCall->sendGdbCommand(QString("-exec-arguments %1").arg(userProgramArguments));

	// Get current user-defined breakpoints on GUI, if any as strings "filename:lineNumber"
	const QList<GuiBreakpoint*>& editorBreakpoints = unitPlayingScene->retrieveBreakpoints();
	foreach(const GuiBreakpoint* guiBreakpoint, editorBreakpoints)
	{
		const QString& originalLocation = guiBreakpoint->buildOriginalLocation();
		if ( debuggerCall->sendGdbCommand( "-break-insert " + originalLocation ) == GDB_ERROR )
			qCWarning( logVisualizator, "Error: -break-insert %s", qUtf8Printable(originalLocation) );
	}

	// Always stop execution at main function
	if ( debuggerCall->sendGdbCommand("-break-insert -f main") == GDB_ERROR )
		qCritical(logVisualizator(), "Failed to set breakpoint at main() function");

	// ToDo: Extract source filenames. Not required by botNeumann++ for the moment

	// Ask GDB to start execution of user program (inferior), only if it is not running already
	if ( gdbState == STATE_STARTING )
		qCritical(logVisualizator(), "gdb already starting");
	else if ( gdbState == STATE_RUNNING )
		qCritical(logVisualizator(), "gdb already running");
	else
	{
		inferiorProcessId = 0;
		GdbState oldState = gdbState;
		gdbState = STATE_STARTING;
		if ( debuggerCall->sendGdbCommand("-exec-run") == GDB_ERROR )
			gdbState = oldState;
	}
/*
	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );
*/
	return result;
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

	// Check if the action is creator or deletion of a breakpoint
	switch ( guiBreakpoint->getAction() )
	{
		case GuiBreakpoint::Action::unknown:
			break;

		case GuiBreakpoint::Action::created:
			debuggerCall->sendGdbCommand( QString("-break-insert %1").arg(guiBreakpoint->buildOriginalLocation()) );
			break;

		case GuiBreakpoint::Action::removed:
			int breakpointNumber = findDebuggerBreakpointIndex( *guiBreakpoint );
			if ( breakpointNumber > 0 && debuggerCall->sendGdbCommand( QString("-break-delete %1").arg(breakpointNumber) ) != GDB_ERROR )
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

// Responses received by GdbCall

void Visualizator::processGdbResponse()
{
	// If there is an active animation, wait until it is done
	if ( animationDone.remainingTime() > 0 )
		return;

	// The last animation is finished, stop its timer
	animationDone.stop();

	// Fetch the next pending response
	Q_ASSERT(debuggerCall);
	GdbResponse* gdbResponse = debuggerCall->takeNextResponse();

	// If there is no pending responses, we are done
	if ( gdbResponse == nullptr )
		return;

	qCDebug(logVisualizator, "processGdbResponse: %s", qPrintable(gdbResponse->buildDescription(true)));

	// Notify all actors to animate this response, they will inform how many milliseconds they
	// will take to complete the animation
	int maxDuration = 0;
	emit dispatchGdbResponse(gdbResponse, maxDuration);

	// Wait until the animation is done, then call this method again to process the next pending
	// response
	animationDone.start(maxDuration);
}

void Visualizator::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logVisualizator(), "onExecAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));

	switch ( asyncClass )
	{
		case AsyncClass::AC_RUNNING:
			gdbState = STATE_RUNNING;
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

void Visualizator::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logVisualizator(), "onStatusAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void Visualizator::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	Q_ASSERT(debuggerCall);
	qCDebug(logVisualizator(), "onNotifyAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));

	switch ( asyncClass )
	{
		case AsyncClass::AC_THREAD_CREATED:
			debuggerCall->sendGdbCommand("-thread-info");
			break;

		default:
			break;
	}
}

void Visualizator::onResult(const GdbItemTree& tree, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logVisualizator(), "onResult(%s)", qPrintable(tree.buildDescription()));
	const GdbTreeNode* node = nullptr;

	if ( ( node = tree.findNode("/threads") ) )
		return updateThreads( node );
	if ( ( node = tree.findNode("/bkpt") ) )
		return updateDebuggerBreakpoint( node );
}

void Visualizator::onConsoleStreamOutput(const QString& text, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	QStringList lines = text.split('\n');
	for ( int lineIndex = 0; lineIndex < lines.size(); ++lineIndex )
	{
		const QString& line = lines[lineIndex];
		if ( ! line.isEmpty() )
			unitPlayingScene->getMessagesArea()->appendDebuggerMessage(LOG_CONSOLE_OUTPUT, line);
	}
}

void Visualizator::onTargetStreamOutput(const QString& str, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logVisualizator(), "onTargetStreamOutput(%s)", qPrintable(str));
}

void Visualizator::onLogStreamOutput(const QString& str, int& maxDuration)
{
	Q_UNUSED(maxDuration);
	qCDebug(logVisualizator(), "onLogStreamOutput(%s)", qPrintable(str));
}


void Visualizator::updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode)
{
	// Create a debugger breakpoint that parsers the output sent by debugger
	Q_ASSERT(breakpointNode);
	DebuggerBreakpoint* debuggerBreakpoint = new DebuggerBreakpoint( *breakpointNode );

	// Check if the breakpoint already exists in our vector
	int breakpointNumber = debuggerBreakpoint->getNumber();
	if ( breakpointNumber < debuggerBreakpoints.size() && debuggerBreakpoints[breakpointNumber] != nullptr )
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

	// Update the interface?
//	emit breakpointUpdated( debuggerBreakpoints[breakpointNumber] );
}

void Visualizator::updateThreads(const GdbTreeNode* threadsNode)
{
//	m_threadList.clear();

	// Each child node of threadsNode is a thread in execution, gets its information
	for ( int childIndex = 0; childIndex < threadsNode->getChildCount(); ++childIndex )
	{
		// Get the child and its name
		const GdbTreeNode* threadChildNode = threadsNode->getChild(childIndex);

		// Get the properties of this thread
		QString threadId = threadChildNode->findTextValue("id");
		QString targetId = threadChildNode->findTextValue("target-id");
		QString funcName = threadChildNode->findTextValue("frame/func");

		qDebug("  Thread[id=%s][target-id=%s][frame/func=%s]", qPrintable(threadId), qPrintable(targetId), qPrintable(funcName));

#if 0
		ThreadInfo tinfo;
		tinfo.id = atoi(stringToCStr(threadId));
		tinfo.m_name = targetId;
		tinfo.m_func = funcName;
		m_threadList[tinfo.id] = tinfo;
	}

	if(m_inf)
		m_inf->ICore_onThreadListChanged();
#endif
	}
}
