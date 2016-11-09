#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include "GdbCommon.h"
#include "GdbResponse.h"
#include <QObject>
#include <QFileInfo>
#include <QVector>

class DebuggerBreakpoint;
class GdbCall;
class GuiBreakpoint;
class UnitPlayingScene;

/**
	@brief Controller class in charge of controlling the visualization process.

	The visualizator mediates between the model (DebuggerCall) and the views (UnitPlayingScene and
	its helping classes).
*/
class Visualizator : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Visualizator)

  protected:
	/// Full path to the player's executable to be debugged
	QFileInfo executablePath;
	/// @todo Allow user to write executable parameters
	QString userProgramArguments;
	/// The model is a debugger session with the executable of the player
	/// @todo Using GdbCall interface directly, replace it by DebuggerCall interface
	GdbCall* debuggerCall;
	/// The state of GDB being controlled by this object
	GdbState state = STATE_STOPPED;
	/// The view is the unit playing scene and its components
	UnitPlayingScene* unitPlayingScene;
	/// The list of breakpoints reported by debugger. They are identified by integers. The indexes
	/// in the array match the integers used by the debugger
	QVector<DebuggerBreakpoint*> debuggerBreakpoints;
	/// Process id of the inferior retrieved from gdb. Needed?
	int inferiorProcessId;

  public:
	/// Constructor
	/// @param executablePath The absolute path to the player solution's executable file to be run
	/// @param unitPlayingScene is the view, and it is set as the parent QObject for this object
	explicit Visualizator(const QFileInfo& executablePath, UnitPlayingScene* unitPlayingScene);
	/// Destructor
	~Visualizator();
	/// Start the visualization process
	/// @return true on success, false otherwise
	bool start();
	/// Gets the state of the gbd instance being controlled by this object
	inline GdbState getState() const { return state; }
	/// Convenience functions to get the state
	inline bool isStopped() const { return state == STATE_STOPPED; }
	inline bool isRunning() const { return state == STATE_RUNNING; }
	inline bool isFinished() const { return state == STATE_FINISHED; }

  public slots:
	/// Called when GdbCall has responses
	void onGdbResponse(const GdbResponse* response);
	/// Called when user presses over a breakpoint symbol in order to create or remove it
	/// Visualization controller requires this signal in order to clear the breakpont in
	/// debugger when visualization is running. Internally the GuiBreakpoint object carries
	/// an action atribute that tells if the breakpoint was created or removed
	void breakpointAction(GuiBreakpoint* guiBreakpoint);

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
	bool onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass);
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
	void onConsoleStreamOutput(const QString& text);
	/// The target output stream (@) contains any textual output from the running target. This is
	/// only present when GDB's event loop is truly asynchronous, which is currently only the case
	/// for remote targets.
	void onTargetStreamOutput(const QString& str);
	/// The log stream contains debugging messages being produced by gdb's internals.
	void onLogStreamOutput(const QString& str);

  protected:
	/// A Gdb result brought an updated list of threads, refresh them
	void updateThreads(const GdbTreeNode* threadsNode);
	/// A Gdb result indicates that a new breakpoint was added
	void updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode);
	/// Returns the index of the debugger breakpoint that matches the given GUI breakpoint. The
	/// comparison is made by filename and line number. Returns -1 if no matches are found.
	/// @remark Search is made sequential, therefore O(n) where n is the number of debugger
	/// breakpoints stored in the vector
	int findDebuggerBreakpointIndex(const GuiBreakpoint& guiBreakpoint) const;
};

#endif // VISUALIZATOR_H
