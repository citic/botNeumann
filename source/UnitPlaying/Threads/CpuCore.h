#ifndef CPUCORE_H
#define CPUCORE_H

#include "MemorySegment.h"

class ExecutionThread;
class Prop;

class CpuCore : public MemorySegment
{
	Q_DISABLE_COPY(CpuCore)

  protected:
	/// Number or index of this CPU core, starting from 0
	int cpuCoreNumber = -1;
	/// If this core is busy, an execution thread will be running on it. Otherwise, this is idle.
	ExecutionThread* executionThread = nullptr;
	/// The CPU graphical representation
	Prop* workstation = nullptr;

  public:
	/// Constructor
	explicit CpuCore(int cpuCoreNumber, Unit& unit, Scene* scene);
	/// Destructor
	virtual ~CpuCore();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;
	/// @return true if this CpuCore is empty, that is, no execution threads are running on it
	inline bool isIdle() const { return executionThread == nullptr; }
	/// Give access to the execution thread running on this CPU core, nullptr if idle
	inline ExecutionThread* getThread() { return executionThread; }
	/// Assigns an execution thread to be run on this CPU thread. The execution thread is shown
	/// on the graphical interface, reprsented as a robot
	void runThread(ExecutionThread* thread);

  protected:
	/// Builds the CPU core as a workstation where a execution thread (robot) can work
	void buildCpuCore();
};

#endif // CPUCORE_H
