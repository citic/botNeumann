#include "DebuggerCall.h"
#include "UserProgram.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	// The executable to be debugged must be provided by parameter
	if ( argc < 2 )
		return printf("Usage: GdbTest executable-path\n");

	QCoreApplication app(argc, argv);

	DebuggerCall debuggerCall;
	bool ok = debuggerCall.start();
	if ( ! ok ) return fprintf(stderr, "main: could not start gdb\n");
	printf("main: gdb started\n");

	UserProgram userProgram;
	userProgram.start(argv[1]);
	app.connect( &userProgram, SIGNAL(toolFinished()), &app, SLOT(quit()) );

	return app.exec();
}
