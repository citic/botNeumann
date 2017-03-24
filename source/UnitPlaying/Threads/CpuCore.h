#ifndef CPUCORE_H
#define CPUCORE_H

#include "MemorySegment.h"

#include <QObject>

class ExecutionThread;
class Actor;
class Prop;

class CpuCore : public QObject, public MemorySegment
{
	Q_OBJECT
	Q_DISABLE_COPY(CpuCore)

  protected:
	/// Number or index of this CPU core, starting from 0
	int cpuCoreNumber = -1;
	/// If this core is busy, an execution thread will be running on it. Otherwise, this is idle.
	ExecutionThread* executionThread = nullptr;
	/// The graphical representation of a CPU core (a workstation)
	Prop* workstation = nullptr;
	/// The graphical representation of the interface between the CPU core and the process memory
	/// A door in the floor is used to bring memory frames/rows up and down
	Actor* memoryInterface = nullptr;

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
	/// @return the duration in milliseconds of the animation
	int runThread(ExecutionThread* thread);
	/// Removes the current execution thread from this CpuCore
	/// The execution thread object keeps alive
	/// @param removeThreadCallStack Send true to remove the call stack of current thread being run
	/// in this cpu core, when a thread is killed or animation is stopped
	/// @return the duration in milliseconds of the animation, -1 if no thread was being run
	int removeThread(bool removeThreadCallStack);

  public slots:
	/// Animates the memory interface to open
	/// @return the duration in milliseconds of the animation
	int openMemoryInterface();
	/// Animates the memory interface to close
	/// @return the duration in milliseconds of the animation
	int closeMemoryInterface();

  protected:
	/// Builds the CPU core as a workstation where a execution thread (robot) can work
	void buildCpuCore();
};

#endif // CPUCORE_H
