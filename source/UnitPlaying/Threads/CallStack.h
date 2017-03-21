#ifndef CALLSTACK_H
#define CALLSTACK_H

#include "RectLayoutItem.h"

#include <QStack>

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
	/// The stack of frames or function calls
	QStack<MemoryFrame*> stackFrames;

  public:
	/// Constructor
	CallStack(qreal zValue, QGraphicsItem* graphicsParentItem);
};

#endif // CALLSTACK_H
