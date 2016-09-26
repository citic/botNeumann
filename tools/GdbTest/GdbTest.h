#ifndef GDBTEST_H
#define GDBTEST_H

#include <QCoreApplication>

class GdbCall;
class UserProgram;

/**
 * Controller class for the entire application
 */
class GdbTest : public QCoreApplication
{
	Q_OBJECT

  protected:
	QString userProgramPath;
	GdbCall* debuggerCall = nullptr;
	UserProgram* userProgram = nullptr;

  public:
	explicit GdbTest(int argc, char *argv[]);
	~GdbTest();
	int run();
};

#endif // GDBTEST_H
