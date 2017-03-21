#include "CallStack.h"
#include "GdbItemTree.h"
#include "MemoryFrame.h"
#include "Scene.h"

CallStack::CallStack(qreal zValue, QGraphicsItem* graphicsParentItem)
	: RectLayoutItem(Qt::Vertical, zValue, graphicsParentItem)
{
}

int CallStack::callFunction(const GdbItemTree& tree)
{
	// Parameters to build a memory frame:
	QGraphicsItem* parent = parentItem();
	size_t rowCount = 2;
	size_t startByte = 100;
	size_t rowSize = 16;
	const QString& functionName = tree.findNodeTextValue("/frame/func") + "()";
	bool withGarbage = true;

	// Build a memory frame for the new stack frame with the function name (/frame/func) in the roof
	// By default, memory frames are filled of garbage.
	MemoryFrame* functionCall = new MemoryFrame(parent, rowCount, startByte, rowSize, functionName, zValue, withGarbage);

	// Add the functionCall to the list
	stackFrames.append(functionCall);

	int duration = 0;

	addItem(functionCall, 1.0, zValue + 0.01 * stackFrames.count());
	updateLayoutItem();

	// Raise the memory roof to the CPU core opened door, to make the roof visible only. Arguments and variables will be made visible next.
	return duration;
}
