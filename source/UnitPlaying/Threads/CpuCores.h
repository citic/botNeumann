#ifndef CPUCORES_H
#define CPUCORES_H

#include "GdbResponseListener.h"
#include "MemorySegment.h"

#include <QVector>

class CpuCore;
class DebuggerBreakpoint;
class ExecutionThread;
class GdbCall;

class CpuCores : public GdbResponseListener, public MemorySegment
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
	/// Allocate local variables in some thread that is running
	virtual bool allocate(MemoryAllocation* memoryAllocation) override;
	/// Deallocate local variables in some thread that is running
	virtual bool deallocate(MemoryAllocation* memoryAllocation) override;
	/// Called when player solution stopped by a function body breakpoint
	bool processFunctionCall(const GdbItemTree& tree, GdbCall* debuggerCall, int& maxDuration);

  signals:
	/// Emitted when an execution thread was updated from GDB
	void executionThreadUpdated(const ExecutionThread* executionThread);

  public slots:
	/// Called when visualization has finished in order to remove execution threads
	void clearAnimation();

  protected:
	/// Creates a CPU core (workstation) for each CPU core requested in .botnu file
	void createCpuCores();
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	///	@see GdbResponseListener::onNotifyAsyncOut()
	virtual void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizationContext context, int& maxDuration) override;
	/// Notifications that begin with '^': ^done, ^connected, ^error, ^exit
	///	@see GdbResponseListener::onResult()
	virtual void onResult(const GdbItemTree& tree, VisualizationContext context, int& maxDuration) override;
	/// Debugger reports the creation of a new execution thread, show it
	/// @return the duration in milliseconds of the animation
	int createThread(int id);
	/// Debugger reports an execution thread exited, remove it from scene
	/// @return the duration in milliseconds of the animation
	int removeThread(int id);
	/// @return The index of the next available (free) CPU core to execute a thread, -1 if all cores
	/// are busy executing threads
	int findFirstIdleCpuCore() const;
	/// A Gdb result brought an updated list of threads, refresh them
	void updateThreads(const GdbTreeNode* threadsNode, int& maxDuration);
	/// Finds the thread with the given id using linear search
	/// @param threadIndex If a pointer is given, the pointed variable will be updated with the
	/// index where the found thread is located
	ExecutionThread* findThread(int id, int* threadIndex = nullptr) const;
};

#endif // CPUCORES_H
