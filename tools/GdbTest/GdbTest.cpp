#include "GdbTest.h"
#include "GdbCall.h"
#include "UserProgram.h"

GdbTest::GdbTest(int argc, char *argv[])
	: QCoreApplication(argc, argv)
	, userProgramPath( argv[1] )
{
	parseUserProgramArguments(argc, argv);
}

GdbTest::~GdbTest()
{
	delete gdbCall;
	delete userProgram;
}

int GdbTest::run()
{
	Q_ASSERT(gdbCall == nullptr);
	gdbCall = new GdbCall(this);
	connect( gdbCall, SIGNAL(onGdbResponse(const GdbResponse*)), this, SLOT(onGdbResponse(const GdbResponse*)) );
	bool ok = gdbCall->start();
	if ( ! ok ) return fprintf(stderr, "GdbTest: could not start gdb\n");
	printf("GdbTest: gdb started\n");

	// Ask GDB run user program
	if ( gdbCall->sendGdbCommand(QString("-file-exec-and-symbols %1").arg(userProgramPath)) == GDB_ERROR )
		qFatal("GdbTest: Failed to run user program: '%s'", qPrintable(userProgramPath));

	// Give inferior parameters to GDB
	if ( userProgramArguments.length() > 0 )
		gdbCall->sendGdbCommand(QString("-exec-arguments %1").arg(userProgramArguments));

	// ToDo: restore breakpoints from configuration
	// else set breakpoint to main function
	if ( gdbCall->sendGdbCommand("-break-insert -f main") == GDB_ERROR )
		qFatal("GdbTest: Failed to set breakpoint at main() function");

	// ToDo: Extract source filenames. Not required by botNeumann++ for the moment

	// Ask GDB to start execution of user program (inferior), only if it is not running already
	if ( gdbCall->getState() == GdbCall::STATE_RUNNING )
		qFatal("GdbTest: gdb already running");
	else
		gdbCall->sendGdbCommand("-exec-run");

	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );

	return QCoreApplication::exec();
}

void GdbTest::onGdbResponse(const GdbResponse* response)
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

void GdbTest::parseUserProgramArguments(int argc, char* argv[])
{
	for ( int index = 2; index < argc; ++index )
		userProgramArguments += argv[index];
}

void GdbTest::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("GdbTest::onExecAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void GdbTest::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("GdbTest::onStatusAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void GdbTest::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass)
{
	qDebug("GdbTest::onNotifyAsyncOut(%s) %s", qPrintable(tree.buildDescription()), GdbResponse::mapReasonToString(asyncClass));
}

void GdbTest::onResult(const GdbItemTree& tree)
{
	qDebug("GdbTest::onResult(%s)", qPrintable(tree.buildDescription()));
}

void GdbTest::onConsoleStreamOutput(const QString& str)
{
	qDebug("GdbTest::onConsoleStreamOutput(%s)", qPrintable(str));
}

void GdbTest::onTargetStreamOutput(const QString& str)
{
	qDebug("GdbTest::onTargetStreamOutput(%s)", qPrintable(str));
}

void GdbTest::onLogStreamOutput(const QString& str)
{
	qDebug("GdbTest::onLogStreamOutput(%s)", qPrintable(str));
}
