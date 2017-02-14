#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include "GdbResponseListener.h"

#include <QFileInfo>
#include <QTimer>
#include <QVector>

class DebuggerBreakpoint;
class GdbCall;
class GuiBreakpoint;
class PlayerSolution;
class UnitPlayingScene;

/**
	@brief Controller class in charge of controlling the visualization process.

	The visualizator mediates between the model (DebuggerCall) and the views (UnitPlayingScene and
	its helping classes).

	A visualizator object animates the execution of the player solution running against just one
	test case at time. If other test case should be animated, a new Visualization object must be
	created. Because an Unit can have several test cases, a different visualization may be
	produced from each single test case. UnitPlayingScene shows test cases and user may choose
	to visualize another. By default, the first test case of the unit is visualized each time
	the visualization starts.
*/
class Visualizator : public GdbResponseListener
{
	Q_OBJECT
	Q_DISABLE_COPY(Visualizator)

  protected:
	/// A visualizator animates the execution of the player solution against a test case
	PlayerSolution* playerSolution = nullptr;
	/// The test case to run the player solution against to
	int testCaseNumber = 1;
	/// The animation of player solution running the test case is done into a view
	UnitPlayingScene* unitPlayingScene = nullptr;
	/// The model is a debugger session with the executable of the player
	/// @todo Using GdbCall interface directly, replace it by DebuggerCall interface
	GdbCall* debuggerCall = nullptr;
	/// The state of GDB being controlled by this object
	GdbState gdbState = STATE_STOPPED;
	/// The list of breakpoints reported by debugger. They are identified by integers. The indexes
	/// in the array match the integers used by the debugger
	QVector<DebuggerBreakpoint*> debuggerBreakpoints;
	/// Process id of the inferior retrieved from gdb. Needed?
	int inferiorProcessId;
	/// Controls when the last animation is done before processing the next GdbCommand
	QTimer animationDone;
	/// When the unit playing scene is paused, not animations are done, but if a step in/out/over
	/// command is issued, the animation resumes until the step is done
	bool inStep = false;

  public:
	/// Constructor
	/// @param executablePath The absolute path to the player solution's executable file to be run
	/// @param unitPlayingScene is the view, and it is set as the parent QObject for this object
	explicit Visualizator(PlayerSolution* playerSolution, int testCaseNumber, UnitPlayingScene* unitPlayingScene);
	/// Destructor
	~Visualizator();
	/// Start the visualization process
	/// @return true on success, false otherwise
	bool start();
	/// Stops the visualization process
	/// @return true on success, false otherwise
	bool stop();
	/// Pauses the visualization process
	/// @return true on success, false otherwise
	bool pause();
	/// Resumes the visualization process if it is paused
	/// @return true on success, false otherwise
	bool resume();

  signals:
	/// Emitted when there is a GdbResponse to process.
	/// Actors are interested on these responses to create an animation in some cases, for example
	/// when a function is called, an animation of a stack frame rising from the floor is done.
	/// @param maxDuration A reference to a integer variable is passed to all actors. The actor
	/// with the largest duration to do the animation must assign this variable. The Visualizator
	/// will wait these amount of milliseconds (the animation is done) until fetching the next
	/// pending GdbResponse
	void dispatchGdbResponse(const GdbResponse* response, int& maxDuration);
	/// Emitted when the visualization has finished the starting phase and it is now animating
	void animationStarted();

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
	/// Called when user asks to step into, i.e: execute the next instruction and if it is a
	/// function call, enter into the function body
	/// @return true on success, false on error
	inline bool stepInto() { return step("-exec-step", "Step into"); }
	/// Called when user wants to exit from current function call and return to caller
	/// @return true on success, false on error
	inline bool stepOut() { return step("-exec-finish", "Step out"); }
	/// Called when user asks to step over, i.e: execute the next instruction but not entering
	/// in function calls
	/// @return true on success, false on error
	inline bool stepOver() { return step("-exec-next", "Step over"); }

  protected:
	/// Build a string with the arguments to call inferior (player solution) under GDB. It contains
	/// the arguments for the test case. In Unix also redirection of standard input/output may be
	/// done in order to capture the output from player solution while it is being animated
	QString buildInferiorArguments();
	/// A Gdb result indicates that a new breakpoint was added
	void updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode);
	/// Returns the index of the debugger breakpoint that matches the given GUI breakpoint. The
	/// comparison is made by filename and line number. Returns -1 if no matches are found.
	/// @remark Search is made sequential, therefore O(n) where n is the number of debugger
	/// breakpoints stored in the vector
	int findDebuggerBreakpointIndex(const GuiBreakpoint& guiBreakpoint) const;
	/// Called when any of the step commands is called: stepInto, stepOut, or stepOver
	/// @param gdbCommand The GDB command that will be sent, e.g: "-exec-step"
	/// @param description The action to be logged, e.g: "Step into"
	/// @return true on success, false on error
	bool step(const QString& gdbCommand, const QString& description);

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
