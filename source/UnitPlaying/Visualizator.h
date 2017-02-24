#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include "GdbResponseListener.h"
#include "DebuggerBreakpoint.h"

#include <QFileInfo>
#include <QTimer>
#include <QVector>

class DebuggerBreakpoint;
class GdbCall;
class GuiBreakpoint;
class PlayerSolution;
class UnitPlayingScene;
class MemoryMapper;

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
	int inferiorProcessId = 0;
	/// Controls when the last animation is done before processing the next GdbCommand
	QTimer animationDone;
	/// When the unit playing scene is paused, not animations are done, but if a step in/out/over
	/// command is issued, the animation resumes until the step is done
	bool inStep = false;
	/// We store the program entry point tree in order to defer the function call animation until
	/// the starting process has enterely finished
	GdbItemTree* entryPointTree = nullptr;
	/// Class in charge of mapping memory blocks (variables) from player solution run and
	/// visualization's memory blocks (variables)
	MemoryMapper* memoryMapper = nullptr;

  public:
	/// Constructor
	/// @param executablePath The absolute path to the player solution's executable file to be run
	/// @param unitPlayingScene is the view, and it is set as the parent QObject for this object
	explicit Visualizator(PlayerSolution* playerSolution, int testCaseNumber, UnitPlayingScene* unitPlayingScene);
	/// Destructor
	~Visualizator();
	/// Start the visualization process
	/// @param preparation If true, only the preparation phase will be executed. The preparation
	/// phase runs GDB, sets the inferior, arguments, user-defined breakpoints, function definition
	/// breakpoints and runs the inferior. If false, only the starting phase is executed. The
	/// starting phase assumes the inferior is stopped by the program entry breakpoint, all
	/// libraries have been loaded. During the second phase, dynamic memory function breakpoints,
	/// standard input/output breakpoints are set. The second phase is automatically called by
	/// Visualizator when the inferior stops at the program entry point.
	/// @return true on success, false otherwise
	bool start(bool preparation = true);
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
	bool updateDebuggerBreakpoint(const GdbTreeNode* breakpointNode, DebuggerBreakpoint::Role role);
	/// A Gdb result indicates that a breakpoint was deleted
	void deleteDebuggerBreakpoint(const GdbTreeNode* breakpointNode);
	/// Deletes the debugger breakpoint with the given number from the debuggerBreakpoints[] array
	/// and from GDB if requested
	bool deleteDebuggerBreakpoint(int breakpointNumber, bool sendGdbDeleteCommand);
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
	virtual void onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration) override;
	/// Notifications that begin with '+'
	///	@see GdbResponseListener::onStatusAsyncOut()
	virtual void onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration) override;
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	///	@see GdbResponseListener::onNotifyAsyncOut()
	virtual void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration) override;
	/// Notifications that begin with '^': ^done, ^connected, ^error, ^exit
	///	@see GdbResponseListener::onResult()
	virtual void onResult(const GdbItemTree& tree, VisualizationContext context, int& maxDuration) override;
	/// Console output stream ('~'): text that should be displayed in the CLI console window
	///	@see GdbResponseListener::onConsoleStreamOutput()
	virtual void onConsoleStreamOutput(const QString& text, VisualizationContext context, int& maxDuration) override;
	/// Target output stream (@): any textual output from the running target
	///	@see GdbResponseListener::onTargetStreamOutput()
	virtual void onTargetStreamOutput(const QString& str, VisualizationContext context, int& maxDuration) override;
	/// The log stream contains debugging messages being produced by gdb's internals.
	///	@see GdbResponseListener::onLogStreamOutput()
	virtual void onLogStreamOutput(const QString& str, VisualizationContext context, int& maxDuration) override;

  protected:
	/// Create the object to communicate with GDB
	bool createGdb();
	/// Start GDB process
	bool startGdb();
	/// Set player solution executable and its arguments in GDB
	bool setInferiorAndArguments();
	/// Set GDB breakpoints for user defined breakpoints
	bool setUserDefinedBreakpoints();
	/// Set GDB breakpoints for the function definitions gathered from ctags
	bool setFunctionDefinitionBreakpoints();
	/// Creates a breakpoint
	bool insertBreakpoint(const QString& position, DebuggerBreakpoint::Role role);
	/// Starts the execution of inferior (player solution) under GDB
	bool startInferior();
	/// Set breakpoints to standard library functions to manage dynamic memory: malloc(), calloc(),
	/// realloc() and free(). Because C++ dynamic memory operators (new, delete, new[], delete[])
	/// internally call the libc standard library functions, we only set breaktpoints for these
	/// libc functions.
	bool setDynamicMemoryBreakpoints();
	/// Create object variables watching changes in IO, using notation bn_io_file
	bool watchStandardInputOutput();
	/// Create object variables watching changes in global variables, using notation bn_gv_##
	bool watchGlobalVariables();
	/// Set watches for the standard input, output and error pointers
	/// Called when player solution stopped for some reason, eg: breakpoint-hit or end-stepping-range
	bool processPlayerSolutionStopped(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	/// Called when player solution stopped for some reason, eg: breakpoint-hit or end-stepping-range
	bool processBreakpointHit(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	/// Called when player solution stopped by program entry point breakpoint
	bool processEntryPoint(const GdbItemTree& tree, DebuggerBreakpoint* breakpoint, int& maxDuration);
};

#endif // VISUALIZATOR_H
