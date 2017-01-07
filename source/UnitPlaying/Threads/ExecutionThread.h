#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include "Actor.h"

/** An ExecutionThread is a graphical object to represent an execution thread running on the
	user program (inferior). An ExecutionThread that is running is shown in one of the available
	CpuCores. A sleeping ExecutionThread is not shown in any CpuCore
**/
class ExecutionThread : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(ExecutionThread)

  protected:
	/// The number of execution thread, reported by Gdb
	int id = -1;

  public:
	/// Constructor
	explicit ExecutionThread(int id, QGraphicsItem* parentItem);
};

#endif // EXECUTIONTHREAD_H
