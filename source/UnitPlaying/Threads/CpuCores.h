#ifndef CPUCORES_H
#define CPUCORES_H

#include "MemorySegment.h"

#include <QObject>
#include <QVector>

class CpuCore;
class DebuggerBreakpoint;
class ExecutionThread;
class GdbTreeNode;
class GdbItemTree;
class GdbCall;

class CpuCores : public QObject, public MemorySegment
{
	Q_OBJECT
	Q_DISABLE_COPY(CpuCores)

  protected:
	/// A horizontal layout to place the CPU cores
	LinearLayout* cpuCoresLayout = nullptr;
	/// Available CPU cores to have execution threads (robots) working
	QVector<CpuCore*> cpuCores;
	/// A horizontal layout to place idle execution threads between the CPU cores and data segment
	LinearLayout* idleThreadsLayout = nullptr;
	/// All running execution threads in player's solution (inferior)
	QVector<ExecutionThread*> executionThreads;
	/// Count the number of idle threads that have are waiting for a cpu core
	int idleThreadsCount = 0;

  public:
	/// Constructor
	explicit CpuCores(Unit& unit, Scene* scene, QObject* parent = nullptr);
	/// Destructor
	virtual ~CpuCores();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;
	/// Debugger reports the creation of a new execution thread, show it
	/// @return the duration in milliseconds of the animation
	int createThread(int id);
	/// Debugger reports an execution thread exited, remove it from scene
	/// @return the duration in milliseconds of the animation
	int removeThread(int id);
	/// A Gdb result brought an updated list of threads, refresh them
	void updateThreads(const GdbTreeNode* threadsNode, int& maxDuration);
	/// GDB generated any response that has a '/frame/' subtree. E.g: stopped by
	/// reason="end-stepping-range", or "reason="breakpoint-hit". The '/frame' subtree tells the
	/// line being executed by the thread. We update it
	/// @param breakpointLineNumber The line number where the breakpoint was originally set. GDB
	/// modifies breakpoints to the first executable line in the body. We want to highlight the
	/// line where the function is declared during the function call, instead.
	void updateThreadFrame(const GdbItemTree& tree, int& maxDuration, int breakpointLineNumber);
	/// Checks the call stack depth of the execution thread that generated the given tree. If there
	/// was a function call or return, animates it. It do not update the line number on the
	/// execution thread actor and code editor
	/// @return true if a function called or returned, false if we are running at the same function
	bool checkForFunctionCallOrReturn(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration, bool checkCall);
	/// Allocate local variables in some thread that is running
	virtual bool allocate(MemoryAllocation* memoryAllocation) override;
	/// Deallocate local variables in some thread that is running
	virtual bool deallocate(MemoryAllocation* memoryAllocation) override;
	/// Returns a list of threads that are waiting for some input/output operation.
	void getThreadsWaitingForIO(QList<ExecutionThread*>& inputQueue, QList<ExecutionThread*>& outputQueue);

  signals:
	/// Emitted when an execution thread was updated from GDB
	void executionThreadUpdated(ExecutionThread* executionThread, int& maxDuration);

  public slots:
	/// Called when visualization has finished in order to remove execution threads
	void clearAnimation();

  protected:
	/// Creates a CPU core (workstation) for each CPU core requested in .botnu file
	void createCpuCores();
	/// @return The index of the next available (free) CPU core to execute a thread, -1 if all cores
	/// are busy executing threads
	int findFirstIdleCpuCore() const;
	/// Finds the thread with the given id using linear search
	/// @param threadIndex If a pointer is given, the pointed variable will be updated with the
	/// index where the found thread is located
	ExecutionThread* findThread(int id, int* threadIndex = nullptr) const;
	/// Finds the thread from any tree that contains a '/thread-id' node
	/// @param threadIndex If a pointer is given, the pointed variable will be updated with the
	/// index where the found thread is located
	ExecutionThread* findThread(const GdbItemTree& tree, int* threadIndex = nullptr) const;
	/// Highlights the given line in thread's actor and code editor
	/// This method is called as an ugly fix for function call two-line steps. See updateThreadFrame
	void updateThreadLine(ExecutionThread* thread, int lineNumber);
};

#endif // CPUCORES_H
