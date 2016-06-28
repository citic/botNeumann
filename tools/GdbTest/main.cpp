#include "DebuggerCall.h"
#include "UserProgram.h"
#include <QCoreApplication>


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	printf("\nStarting debugger...\n");
	DebuggerCall debuggerCall;
	bool ok = debuggerCall.start();
	if ( ! ok ) return fprintf(stderr, "Could not start gdb\n");

	UserProgram userProgram;
	userProgram.start("/Users/jhc/dev/botNeumann/tools/GdbTest/examples/hello_c");
	app.connect( &userProgram, SIGNAL(userProgramFinished()), &app, SLOT(quit()) );

	return app.exec();
}

