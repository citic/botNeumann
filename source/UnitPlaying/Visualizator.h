#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include "DebuggerBreakpoint.h"
#include "GdbCommon.h"
#include "GdbResponse.h"
#include "VisualizationContext.h"

#include <QFileInfo>
#include <QTimer>
#include <QVector>

class DebuggerBreakpoint;
class ExecutionThread;
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
class Visualizator : public QObject
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
	/// Schedules a @a stepForward() function call some milliseconds later
	QTimer stepForwardTimer;
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
	virtual ~Visualizator() override;
	/// Get access to the test case number being visualized
	inline int getTestCaseNumber() const { return testCaseNumber; }
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
	/// Emitted when the player solution finished its execution
	/// @remarks Currently it is only issued when player solution finished normally
	void playerSolutionFinished();

  public slots:
	/// Emitted when there is a GdbResponse to process.
	/// Actors are interested on these responses to create an animation in some cases, for example
	/// when a function is called, an animation of a stack frame rising from the floor is done.
	/// @param maxDuration A reference to a integer variable is passed to all actors. The actor
	/// with the largest duration to do the animation must assign this variable. The Visualizator
	/// will wait these amount of milliseconds (the animation is done) until fetching the next
	/// pending GdbResponse
	void onGdbResponse(const GdbResponse* response, int& maxDuration);
	/// Called when there are pending GdbResponses to process.
	/// This method fetches the next pending GdbResponse and notifies actors to do the animation.
	/// It waits until the animation is done. When it is done, it calls itself again to check if
	/// there is more pending GdbResponses, until there is not more.
	/// @return A negative value if no animation is done at all, eg: visualization stopped.
	/// Return 0 if there are no pending responses to process and visualizator is ready to animate.
	/// Return a positive value if there is a current animation in progress.
	int processGdbResponse();
	/// Called when user presses over a breakpoint symbol in order to create or remove it
	/// Visualization controller requires this signal in order to clear the breakpont in
	/// debugger when visualization is running. Internally the GuiBreakpoint object carries
	/// an action atribute that tells if the breakpoint was created or removed
	void breakpointAction(GuiBreakpoint* guiBreakpoint);
	/// Called when user asks to step forward, i.e: execute the next step of the visualization.
	/// Execution loop. Each step of this loop is considered a step of the visualization. When
	/// visualization is paused and player presses the Step button, one visualization step is done
	/// (animated). When visualization is in seeking state, a visualization step is issued each time
	/// there is no pending commands to be sent to GDB.
	/// A visualization step moves around the -exec-next instruction. Each time this command is
	/// sent, some events may happen: a variable is changed, a function is called, a function is
	/// returned...​ The breakpoint system will catch the function calls and dynamic memory changes.
	/// In each visualization step we have to update the remaining elements: execution threads
	/// (line number), stack, data segment, and standard input/output.
	/// @return true on success, false on error
	bool stepForward();
	/// Schedule a next forward call in the given amount of milliseconds if the visualization is
	/// in animating state
	bool scheduleStepForward(int time);

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

  protected:
	/// Async records are used to notify the gdb/mi client of additional changes that have occurred.
	///	Those changes can either be a consequence of gdb/mi commands (e.g., a breakpoint modified)
	///	or a result of target activity (e.g., target stopped). The following is the list of possible
	///	async records:
	///		*running,thread-id="thread"
	///		*stopped,reason="reason",thread-id="id",stopped-threads="stopped",core="core"
	///	@see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration);
	/// Notifications that begin with '+'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration);
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration);
	/// Additional out-of-band notifications:
	///   "^done[,results]": The synchronous operation was successful, results are provided
	///   "^running": Deprecated. Equivalent to ‘^done’
	///   "^connected": gdb has connected to a remote target.
	///   "^error,msg=c-string[,code=c-string]": The operation failed.
	///   "^exit": gdb has terminated.
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Result-Records.html
	void onResult(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	/// gdb internally maintains a number of output streams: the console (~), the target (@), and
	/// the log (&). The console output stream ('~') contains text that should be displayed in the
	/// CLI console window. It contains the textual responses to CLI commands.
	void onConsoleStreamOutput(const QString& text, VisualizationContext context, int& maxDuration);
	/// The target output stream (@) contains any textual output from the running target. This is
	/// only present when GDB's event loop is truly asynchronous, which is currently only the case
	/// for remote targets.
	void onTargetStreamOutput(const QString& str, VisualizationContext context, int& maxDuration);
	/// The log stream contains debugging messages being produced by gdb's internals.
	void onLogStreamOutput(const QString& str, VisualizationContext context, int& maxDuration);

  protected:
	/// Create the object to communicate with GDB
	bool createGdb();
	/// Create the object to map variables between inferior and visualization
	bool createMemoryMapper();
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
	bool processBreakpointHit(const GdbItemTree tree, VisualizationContext context, int& maxDuration);
	/// Called when player solution stopped by program entry point breakpoint
	bool processEntryPoint(const GdbItemTree& tree, DebuggerBreakpoint* breakpoint, int& maxDuration);
	/// Called when player solution stopped on some user-defined breakpoint. Visualization pauses
	bool processUserDefinedBreakpoint();
	/// Called when player solution stopped on some dynamic memory function (malloc, calloc, realloc
	/// or free). If unit has dynamic memory enabled, it may animate some values changing in its
	/// area or pointers in other segments that point to data in heap segment
	bool processDynamicMemoryBreakpoint();
	/// Called when a next-step comand finished in the next source code line
	bool processEndSteppingRange(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	/// A signal was received by the inferior, for example an assert() failed on inferior
	bool processSignalReceived(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	/// Player solution finished normally. If it passed all test cases, player is congratulated and
	/// the level is marked as passed
	bool processExitedNormally(const GdbItemTree& tree, VisualizationContext context, int& maxDuration);
	///
};

#endif // VISUALIZATOR_H
