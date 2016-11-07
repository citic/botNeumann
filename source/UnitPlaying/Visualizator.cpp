#include "Visualizator.h"
#include "DebuggerBreakpoint.h"
#include "GdbCall.h"
#include "GuiBreakpoint.h"
#include "MessagesArea.h"
#include "UnitPlayingScene.h"
#include <QDebug>

Visualizator::Visualizator(const QFileInfo& executablePath, UnitPlayingScene* unitPlayingScene)
	: QObject(unitPlayingScene)
	, executablePath(executablePath)
	, debuggerCall( nullptr )
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
	connect( debuggerCall, SIGNAL(onGdbResponse(const GdbResponse*)), this, SLOT(onGdbResponse(const GdbResponse*)) );
	connect( debuggerCall, SIGNAL(onGdbLogMessage(GdbLogType,QString)), unitPlayingScene->getMessagesArea(), SLOT(appendDebuggerMessage(GdbLogType,QString)));

	bool result = debuggerCall->start();
	if ( ! result )
	{
		qCritical("Visualizator: could not start gdb\n");
		return false;
	}

	qDebug("Visualizator: gdb started");

	// Ask GDB run user program
	if ( debuggerCall->sendGdbCommand(QString("-file-exec-and-symbols \"%1\"").arg(executablePath.filePath())) == GDB_ERROR )
		qFatal("Visualizator: Failed to run user program: '%s'", qPrintable(executablePath.filePath()));

	// Give inferior parameters to GDB
	if ( userProgramArguments.length() > 0 )
		debuggerCall->sendGdbCommand(QString("-exec-arguments %1").arg(userProgramArguments));

	// Get current user-defined breakpoints on GUI, if any as strings "filename:lineNumber"
	const QList<GuiBreakpoint*>& editorBreakpoints = unitPlayingScene->retrieveBreakpoints();
	foreach(const GuiBreakpoint* guiBreakpoint, editorBreakpoints)
	{
		const QString& breakpointFileLine = guiBreakpoint->buildFileLineString();
		if ( debuggerCall->sendGdbCommand( "-break-insert " + breakpointFileLine ) == GDB_ERROR )
			qWarning( "Visualizator: Error: -break-insert %s", qPrintable(breakpointFileLine) );
	}

	// Always stop execution at main function
	if ( debuggerCall->sendGdbCommand("-break-insert -f main") == GDB_ERROR )
		qFatal("Visualizator: Failed to set breakpoint at main() function");

	// ToDo: Extract source filenames. Not required by botNeumann++ for the moment

	// Ask GDB to start execution of user program (inferior), only if it is not running already
	if ( debuggerCall->getState() == GdbCall::STATE_RUNNING )
		qFatal("Visualizator: gdb already running");
	else
		debuggerCall->sendGdbCommand("-exec-run");
/*
	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );
*/
	return result;
}


void Visualizator::onGdbResponse(const GdbResponse* response)
{
	Q_ASSERT(response);
	switch ( response->getType() )
	{
		case GdbResponse::EXEC_ASYNC_OUTPUT: onExecAsyncOut(response->getItemTree(), response->getReason()); break;
		case GdbResponse::STATUS_ASYNC_OUTPUT: onStatusAsyncOut(response->getItemTree(), response->getReason()); break;
		case GdbResponse::NOTIFY_ASYNC_OUTPUT: onNotifyAsyncOut(response->getItemTree(), response->getReason()); break;
		case GdbResponse::LOG_STREAM_OUTPUT: onLogStreamOutput(response->getText()); break;
		case GdbResponse::TARGET_STREAM_OUTPUT: onTargetStreamOutput(response->getText()); break;
		case GdbResponse::CONSOLE_STREAM_OUTPUT: onConsoleStreamOutput(response->getText()); break;
		case GdbResponse::RESULT: onResult(response->getItemTree()); break;

		default: break;
	}
}

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
			debuggerCall->sendGdbCommand( QString("-break-insert %1").arg(guiBreakpoint->buildFileLineString()) );
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

void Visualizator::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("Visualizator::onExecAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void Visualizator::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("Visualizator::onStatusAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void Visualizator::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("Visualizator::onNotifyAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void Visualizator::onResult(const GdbItemTree& tree)
{
	qDebug("Visualizator::onResult(%s)", qPrintable(tree.buildDescription()));
	const GdbTreeNode* node = nullptr;

	if ( ( node = tree.findNode("/threads") ) )
		return updateThreads( node );
	if ( ( node = tree.findNode("/bkpt") ) )
		return updateDebuggerBreakpoint( node );
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

void Visualizator::onConsoleStreamOutput(const QString& text)
{
	QStringList lines = text.split('\n');
	for ( int lineIndex = 0; lineIndex < lines.size(); ++lineIndex )
	{
		const QString& line = lines[lineIndex];
		if ( ! line.isEmpty() )
			unitPlayingScene->getMessagesArea()->appendDebuggerMessage(LOG_CONSOLE_OUTPUT, line);
	}
}

void Visualizator::onTargetStreamOutput(const QString& str)
{
	qDebug("Visualizator::onTargetStreamOutput(%s)", qPrintable(str));
}

void Visualizator::onLogStreamOutput(const QString& str)
{
	qDebug("Visualizator::onLogStreamOutput(%s)", qPrintable(str));
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
