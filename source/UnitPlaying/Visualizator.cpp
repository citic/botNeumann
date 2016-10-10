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
}

bool Visualizator::start()
{
	Q_ASSERT(debuggerCall == nullptr);
	debuggerCall = new GdbCall(this);

	connect( debuggerCall, SIGNAL(onGdbResponse(const GdbResponse*)), this, SLOT(onGdbResponse(const GdbResponse*)) );
	bool ok = debuggerCall->start();
	if ( ! ok )
	{
		qCritical("Visualizator: could not start gdb\n");
		return false;
	}

	qDebug("Visualizator: gdb started\n");

	// Ask GDB run user program
	if ( debuggerCall->sendGdbCommand(QString("-file-exec-and-symbols %1").arg(executablePath.filePath())) == GDB_ERROR )
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
	return debuggerCall->start();
}
