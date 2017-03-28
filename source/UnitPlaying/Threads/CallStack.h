#ifndef CALLSTACK_H
#define CALLSTACK_H

#include "RectLayoutItem.h"

#include <QStack>

class GdbItemTree;
class GdbTreeNode;
class MemoryFrame;

/**
	Manages the stack of function calls for an execution thread. This class is able to do
	function call operations such as:

	- raise a stack frame when a function is called (from the CPU core's memory interface)
	- dive the stack frame (function call) when the function returns,
	- show a stack frame on top of others temporally when user selects it or during parameter-passing
*/
class CallStack : public RectLayoutItem
{
  protected:
	/// The memory address of the first byte assigned for stack memory to this execution thread
	size_t startByte = 0;
	/// The maximum size in bytes that the call stack can grow
	size_t maxSize = 0;
	/// The size in bytes of each memory row of the stack segment
	size_t rowSize = 0;
	/// The stack of frames or function calls
	QStack<MemoryFrame*> stackFrames;
	/// Default number of rows in CPU cores
	qreal cpuCoreRows = 0.0;
	/// Counts the number of local variable watches for each stack frame
	int watchCount = 0;

  public:
	/// Constructor
	CallStack(size_t startByte, size_t rowSize, size_t maxSize, qreal zValue, QGraphicsItem* graphicsParentItem);
	/// Set the rows of the CPU where this call stack should be displayed
	inline void setCpuCoreRows(qreal cpuCoreRows) { this->cpuCoreRows = cpuCoreRows; }
	/// Animate a function call
	/// @param tree The GDB's tree generated by a breakpoint hit at the called function body
	/// @return The milliseconds required by the animation
	/// @remarks Be sure that cpuCoreRows is not zero before calling this method
	int callFunction(const GdbItemTree& tree, int initialDelay = 0);
	/// Animate this set of stack frames appearing from the basement. This method is called when
	/// an execution thread was sleeping and CPU was assigned again
	/// @param initialDelay The animation will start this amount of milliseconds later of this
	/// function call. It may be used to wait until the CPU's memory interface is completely open
	/// @return The amount of milliseconds of the complete animation
	int animateAppear(int initialDelay = 0);
	/// Animate the entire fame stack diving into the basement
	/// @see animateAppear()
	int animateDisappear(int initialDelay = 0);
	/// Create parameters in the top-most function call
	/// @param gdbVariableArray Gdb node result that lists the local variables to be shown
	/// @param threadId We require the thread id to create watches
	int createLocalVariables(const GdbTreeNode* gdbVariableArray, int threadId);
};

#endif // CALLSTACK_H
