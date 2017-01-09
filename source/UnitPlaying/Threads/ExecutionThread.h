#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include "LinearLayout.h"

class Actor;
class Scene;

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
	/// To reparent children to this scene
	Scene* scene;
	/// The number of execution thread, reported by Gdb
	int id = -1;
	/// The robot used to represent the execution thread
	Actor* robot = nullptr;

  public:
	/// Constructor
	explicit ExecutionThread(Scene* scene, int id);
	/// Animate the apparition of the robot
	/// @return The duration of the animation in milliseconds
	int animateAppear();

  protected:
	/// Build the robot
	void buildExecutionThread();
};

#endif // EXECUTIONTHREAD_H
