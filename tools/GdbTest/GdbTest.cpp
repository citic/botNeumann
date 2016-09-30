#include "GdbTest.h"
#include "GdbCall.h"
#include "UserProgram.h"

GdbTest::GdbTest(int argc, char *argv[])
	: QCoreApplication(argc, argv)
	, userProgramPath( argv[1] )
{
}

GdbTest::~GdbTest()
{
	delete debuggerCall;
	delete userProgram;
}

int GdbTest::run()
{
	Q_ASSERT(debuggerCall == nullptr);
	debuggerCall = new GdbCall(this);
	debuggerCall->setListener(this);
	bool ok = debuggerCall->start();
	if ( ! ok ) return fprintf(stderr, "GdbTest: could not start gdb\n");
	printf("GdbTest: gdb started\n");

	// Ask GDB run user program
	if ( debuggerCall->sendGdbCommand(QString("-file-exec-and-symbols %1").arg(userProgramPath)) == GDB_ERROR )
		qFatal("GdbTest: Failed to run user program: '%s'", qPrintable(userProgramPath));

	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );

	return QCoreApplication::exec();
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
