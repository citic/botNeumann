#include "GdbTest.h"
#include "DebuggerCall.h"
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
	debuggerCall = new DebuggerCall(this);
	bool ok = debuggerCall->start();
	if ( ! ok ) return fprintf(stderr, "gdbtest: could not start gdb\n");
	printf("gdbtest: gdb started\n");

	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );

	return QCoreApplication::exec();
}
