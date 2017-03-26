#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include "LinearLayout.h"

class CpuCore;
class DebuggerBreakpoint;
class ExecutionThreadActor;
class GdbCall;
class GdbItemTree;
class GdbTreeNode;
class Scene;
class Spacer;
class CallStack;

class QColor;

enum ThreadState
{
	threadNew,        // initial state
	threadActive,     // running
	threadIdle,       // wating or blocked
	threadTerminated, // exited
};

/** An ExecutionThread is a graphical object to represent an execution thread running on the
	user program (inferior). An ExecutionThread that is running is shown in one of the available
	CpuCores. A sleeping ExecutionThread is not shown in any CpuCore.

	Execution threads are represented with two parts. 1: A robot shows the line number of the source
	code that the execution thread is running. 2: One or more stack frames represent the function
	calls that the execution thread is running. The top-most frame is the running function call.
**/
class ExecutionThread : public LinearLayout
{
	Q_DISABLE_COPY(ExecutionThread)

  protected:
	/// The number of execution thread, reported by Gdb
	int id = -1;
	/// The source file that generated the code that this thread is executing
	QString filename;
	/// The previous source file that this thread was executing before the last update
	/// It is used by code editors to clear highlighted lines
	QString previousFilename;
	/// The line number in that file being executed
	int lineNumber = -1;
	/// The previous line number that this thread was executing before the last update
	/// It is used by code editors to clear highlighted lines
	int previousLineNumber = -1;
	/// The function being currently executed by this thread
	QString functionName;
	/// The memory address of the first byte assigned for stack memory to this execution thread
	size_t startByte = 0;
	/// The size in bytes of each memory row of the stack segment
	size_t rowSize = 0;
	/// Number of stack frames (function calls) currently running on inferior. This number is
	/// reported by GDB. It is used to detect function calls and returns
	int callStackDepth = 0;
	/// Gdb uses an inverted integer to identify functions called level. The current function being
	/// executed (innermost call) is level=0. We can inspect other function in the stack chaging
	/// the level to a higher number. We trace the level being inspected
	int inspectedLevel = 0;

	/// True if this thread is idle, i.e: there are not enough cores to run this thread
	/// An idle thread is supposed to be shown in the area for idle threads in the middle of the
	/// CPU cores and data segment. Active (busy) execution threads are assigned to a CPU core
	/// and have a call stack. Idle threads have a call stack as well, but it is not shown for
	/// screen space limitations. Therefore, idle execution threads only contains the actor (robot)
	ThreadState state = threadNew;
	/// Intially threads are children of the scene. They are re-parented to a cpu core when they
	/// are active, or the idleThreads layout when they are idle
	Scene* scene;
	/// If this execution thread is assigned to a CPU core, this will point to it. If this thread
	/// is detached from the CPU core (for example, is set to sleep), this pointer will be nullptr
	CpuCore* cpuCore = nullptr;
	/// If this thread is idle, this will be its parent
	LinearLayout* idleThreads = nullptr;
	/// The robot used to represent the execution thread
	ExecutionThreadActor* actor = nullptr;
	/// The stack of function calls (frames) that this thread has executed
	/// The call stack is visible if this thread is assigned to a CPU core, otherwise they are
	/// invisible on the visualization, but not deleted. The call stack gets deleted only when the
	/// execution thread is finished
	CallStack* callStack = nullptr;

  public: // States
	/// Build an execution thread and adds it to the scene in invisible mode. In order to make it
	/// visible, call @a run() or @a sleep()
	explicit ExecutionThread(size_t startByte, size_t rowSize, Scene* scene, int id);
	/// Makes this thread to run in the given CPU core
	/// Both actor and call stack will be displayed on the CPU core
	/// @remarks Detaches the thread from any previous zone of the scene
	/// @return Duration of the appearing animation
	int run(CpuCore* cpuCore);
	/// Makes this thread idle. It will wait for a free CPU core in the idle threads section
	/// @remarks Detaches the thread from any previous zone of the scene
	/// @return Duration of the animation
	int sleep(LinearLayout* idleThreads, int idleThreadNumber);
	/// Terminates this execution thread, destroying its actor and call stack
	/// If the given parameter is true, an animation will be done
	/// @return Duration of the appearing animation
	int terminate();

  public: // Accessors
	/// Get access to the members
	inline int getId() const { return id; }
	inline const QString& getFilename() const { return filename; }
	inline int getLineNumber() const { return lineNumber; }
	inline const QString& getPreviousFilename() const { return previousFilename; }
	inline int getPreviousLineNumber() const { return previousLineNumber; }
	/// Returns the color of this thread, in order to highlight its running line
	const QColor& getHighlightColor() const;
	/// Get the default (reference) width in pixels of the actor (robot)
	qreal getActorReferenceWidth() const;
	/// @return A pointer to the CPU core, if this thread is running, nullptr otherwise
	inline CpuCore* getCpuCore() const { return cpuCore; }
	/// Get access to the call stack
	inline CallStack* getCallStack() const { return callStack; }
	/// True if this thread is being executed on some cpu core
	inline bool isActive() const { return state == threadActive; }

  public:
	/// Updates this execution thread from Gdb information.
	/// @return true if there was a location change
	bool updateFromDebugger(const GdbTreeNode* threadNode);
	/// Called from code editor when the location change is within the player solution. The line
	/// number on the actor (robot) is updated and the duration of the animation is returned. Also
	/// makes a backup of the location (filename and line number)
	int locationUpdateAccepted();
	/// Reset filename and line number to a non valid location. Called before terminating an
	/// execution thread in order to remove its highlight from code editro
	void clearLocation();
	/// Called when player solution stopped by a function body breakpoint
	bool processFunctionCall(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration);

  protected:
	/// Build the robot and the call stack as objects in memory. They are not added to the layout
	/// system yet, until the @a run() or @a sleep() functions are called
	void buildExecutionThread();
	/// Detach the execution thread from the cpu core or idle thread layout. Thread will return to
	/// to the original "new" state. This method is called when the thread changes its state, before
	/// assigning it to a new region of the scene
	/// @return Duration of the animation
	int detach();
	/// Update the location (the source code file and line) that this execution thread is running
	/// If the line number has changed, updates the number on the robot
	/// @return true if the new location should be reflected in code editor. Return false if
	/// the thread has no changed its location
	/// @remarks The returned value should be used to ask the CodeEditor to highlight the line
	/// being executed by this tread using its robot color
	bool updateLocation(const GdbTreeNode* threadNode);
	/// Updates the callStackDepth integer doing an inquiry to the debugger
	bool updateCallStackDepth(GdbCall* debuggerCall);
	/// Animate parameter passing to the new function call
	int passParameters(GdbCall* debuggerCall);
};

#endif // EXECUTIONTHREAD_H
