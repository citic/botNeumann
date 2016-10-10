#include "Visualizator.h"
#include "GdbCall.h"
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
	delete debuggerCall;
}

bool Visualizator::start()
{
	Q_ASSERT(debuggerCall == nullptr);
	debuggerCall = new GdbCall(this);

	connect( debuggerCall, SIGNAL(onGdbResponse(const GdbResponse*)), this, SLOT(onGdbResponse(const GdbResponse*)) );
	bool result = debuggerCall->start();
	if ( ! result )
	{
		qCritical("Visualizator: could not start gdb\n");
		return false;
	}

	qDebug("Visualizator: gdb started\n");

	// Ask GDB run user program
	if ( debuggerCall->sendGdbCommand(QString("-file-exec-and-symbols \"%1\"").arg(executablePath.filePath())) == GDB_ERROR )
		qFatal("Visualizator: Failed to run user program: '%s'", qPrintable(executablePath.filePath()));

	// Give inferior parameters to GDB
	if ( userProgramArguments.length() > 0 )
		debuggerCall->sendGdbCommand(QString("-exec-arguments %1").arg(userProgramArguments));

	// ToDo: restore breakpoints from configuration
	// else set breakpoint to main function
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
}

void Visualizator::onConsoleStreamOutput(const QString& str)
{
	qDebug("Visualizator::onConsoleStreamOutput(%s)", qPrintable(str));
}

void Visualizator::onTargetStreamOutput(const QString& str)
{
	qDebug("Visualizator::onTargetStreamOutput(%s)", qPrintable(str));
}

void Visualizator::onLogStreamOutput(const QString& str)
{
	qDebug("Visualizator::onLogStreamOutput(%s)", qPrintable(str));
}
