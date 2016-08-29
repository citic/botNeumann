#ifndef GDBTEST_H
#define GDBTEST_H

#include <QCoreApplication>

class DebuggerCall;
class UserProgram;
class QSocketNotifier;

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
	QSocketNotifier* pseudoterminalActivityMonitor = nullptr;

  public:
	explicit GdbTest(int argc, char *argv[]);
	~GdbTest();
	int run();

  protected:
	/// A pseudoterminal is used to execute gdb
	bool createPseudoterminal();
	void startMonitoringPseudoterminal();

  private slots:
	/// Called each time GDB produces output
	void onGdbOutput(int fileDescriptor);

};

#endif // GDBTEST_H
