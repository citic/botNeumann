#ifndef GDBTEST_H
#define GDBTEST_H

#include <QCoreApplication>

class DebuggerCall;
class UserProgram;

/**
 * Controller class for the entire application
 */
class GdbTest : public QCoreApplication
{
	Q_OBJECT

  protected:
	QString userProgramPath;
	DebuggerCall* debuggerCall = nullptr;
	UserProgram* userProgram = nullptr;
	int pseudoterminalDeviceFileDescriptor = 0;

  public:
	explicit GdbTest(int argc, char *argv[]);
	~GdbTest();
	int run();

  protected:
	bool createPseudoterminal();
};

#endif // GDBTEST_H
