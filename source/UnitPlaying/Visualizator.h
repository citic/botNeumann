#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include "GdbResponseListener.h"

#include <QFileInfo>
#include <QTimer>
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
class Visualizator : public GdbResponseListener
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
	/// Controls when the last animation is done before processing the next GdbCommand
	QTimer animationDone;

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

  signals:
	/// Emitted when there is a GdbResponse to process.
	/// Actors are interested on these responses to create an animation in some cases, for example
	/// when a function is called, an animation of a stack frame rising from the floor is done.
	/// @param maxDuration A reference to a integer variable is passed to all actors. The actor
	/// with the largest duration to do the animation must assign this variable. The Visualizator
	/// will wait these amount of milliseconds (the animation is done) until fetching the next
	/// pending GdbResponse
	void dispatchGdbResponse(const GdbResponse* response, int& maxDuration);

  public slots:
	/// Called when there are pending GdbResponses to process.
	/// This method fetches the next pending GdbResponse and notifies actors to do the animation.
	/// It waits until the animation is done. When it is done, it calls itself again to check if
	/// there is more pending GdbResponses, until there is not more.
	void processGdbResponse();
	/// Called when user presses over a breakpoint symbol in order to create or remove it
	/// Visualization controller requires this signal in order to clear the breakpont in
	/// debugger when visualization is running. Internally the GuiBreakpoint object carries
	/// an action atribute that tells if the breakpoint was created or removed
	void breakpointAction(GuiBreakpoint* guiBreakpoint);

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

  protected:
	///	Notifications that begin with '*', example: *running,thread-id="thread"
	///	@see GdbResponseListener::onExecAsyncOut()
	virtual void onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration) override;
	/// Notifications that begin with '+'
	///	@see GdbResponseListener::onStatusAsyncOut()
	virtual void onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration) override;
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	///	@see GdbResponseListener::onNotifyAsyncOut()
	virtual void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration) override;
	/// Notifications that begin with '^': ^done, ^connected, ^error, ^exit
	///	@see GdbResponseListener::onResult()
	virtual void onResult(const GdbItemTree& tree, int& maxDuration) override;
	/// Console output stream ('~'): text that should be displayed in the CLI console window
	///	@see GdbResponseListener::onConsoleStreamOutput()
	virtual void onConsoleStreamOutput(const QString& text, int& maxDuration) override;
	/// Target output stream (@): any textual output from the running target
	///	@see GdbResponseListener::onTargetStreamOutput()
	virtual void onTargetStreamOutput(const QString& str, int& maxDuration) override;
	/// The log stream contains debugging messages being produced by gdb's internals.
	///	@see GdbResponseListener::onLogStreamOutput()
	virtual void onLogStreamOutput(const QString& str, int& maxDuration) override;
};

#endif // VISUALIZATOR_H
