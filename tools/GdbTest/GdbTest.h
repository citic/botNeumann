#ifndef GDBTEST_H
#define GDBTEST_H

#include "GdbResponse.h"
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
	QString userProgramArguments;
	GdbCall* gdbCall = nullptr;
	UserProgram* userProgram = nullptr;

  public:
	explicit GdbTest(int argc, char *argv[]);
	~GdbTest();
	int run();

  public slots:
	/// Called when GdbCall has responses
	void onGdbResponse(const GdbResponse* response);

  protected:
	/// Extracts inferior parameters from command line arguments
	void parseUserProgramArguments(int argc, char *argv[]);

  protected:
	/// Async records are used to notify the gdb/mi client of additional changes that have occurred.
	///	Those changes can either be a consequence of gdb/mi commands (e.g., a breakpoint modified)
	///	or a result of target activity (e.g., target stopped). The following is the list of possible
	///	async records:
	///		*running,thread-id="thread"
	///		*stopped,reason="reason",thread-id="id",stopped-threads="stopped",core="core"
	///	@see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass);
	/// Notifications that begin with '+'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass);
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass);
	/// Additional out-of-band notifications:
	///   "^done[,results]": The synchronous operation was successful, results are provided
	///   "^running": Deprecated. Equivalent to ‘^done’
	///   "^connected": gdb has connected to a remote target.
	///   "^error,msg=c-string[,code=c-string]": The operation failed.
	///   "^exit": gdb has terminated.
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Result-Records.html
	void onResult(const GdbItemTree& tree);
	/// gdb internally maintains a number of output streams: the console (~), the target (@), and
	/// the log (&). The console output stream ('~') contains text that should be displayed in the
	/// CLI console window. It contains the textual responses to CLI commands.
	void onConsoleStreamOutput(const QString& str);
	/// The target output stream (@) contains any textual output from the running target. This is
	/// only present when GDB's event loop is truly asynchronous, which is currently only the case
	/// for remote targets.
	void onTargetStreamOutput(const QString& str);
	/// The log stream contains debugging messages being produced by gdb's internals.
	void onLogStreamOutput(const QString& str);
};

#endif // GDBTEST_H
