#include "GdbTest.h"
#include "DebuggerCall.h"
#include "UserProgram.h"

GdbTest::GdbTest(int argc, char *argv[])
	: QCoreApplication(argc, argv)
	, userProgramPath( argv[1] )
{
	createPseudoterminal();
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
	if ( ! ok ) return fprintf(stderr, "GdbTest: could not start gdb\n");
	printf("gdbtest: gdb started\n");

	Q_ASSERT(userProgram == nullptr);
	userProgram = new UserProgram(this);
	userProgram->start( this->userProgramPath );
	connect( userProgram, SIGNAL(toolFinished()), this, SLOT(quit()) );

	return QCoreApplication::exec();
}


#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
bool GdbTest::createPseudoterminal()
{
	// Open an unused pseudoterminal master device, identifed by the returned file descriptor
	// O_RDWR: open device for both reading and writing
	// O_NOCTTY: ask the pseudoterminal do not control the debugger or user program, we will do it
	pseudoterminalDeviceFileDescriptor = posix_openpt(O_RDWR | O_NOCTTY);

	// Tell to OS that this process is the owner of the pseudoterminal, and we need to read and
	// write permissions over the file descriptor. This step is unncessary on modern OSs, but it
	// tood practice to keep it for compatibility
	if( grantpt(pseudoterminalDeviceFileDescriptor) != EXIT_SUCCESS )
	{
		fprintf(stderr, "GdbTest: Failed to grant pseudoterminal %i\n", pseudoterminalDeviceFileDescriptor);
		close(pseudoterminalDeviceFileDescriptor);
		return false;
	}

	// New pseudoterminals are locked by default, in order  perform whatever initialization before
	// programs start running on it. When initialization is done, pseudoterminal must be unlocked
	if( unlockpt(pseudoterminalDeviceFileDescriptor) != EXIT_SUCCESS )
	{
		fprintf(stderr, "GdbTest: Failed to unlock pseudoterminal %i\n", pseudoterminalDeviceFileDescriptor);
		close(pseudoterminalDeviceFileDescriptor);
		return false;
	}

	// If the pseudoterminal was created successfully, it will have a name like /dev/pts/nn where
	// nn is a number that identifies the pseudoterminal
	printf("GdbTest: using pseudoterminal: %s\n", ptsname(pseudoterminalDeviceFileDescriptor));
	return true;
}
