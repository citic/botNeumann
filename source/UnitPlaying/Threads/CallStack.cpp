#include "CallStack.h"
#include "GdbItemTree.h"
#include "MemoryFrame.h"
#include "Scene.h"

CallStack::CallStack(size_t startByte, size_t rowSize, qreal zValue, QGraphicsItem* graphicsParentItem, qreal cpuCoreRows)
	: RectLayoutItem(Qt::Vertical, zValue, graphicsParentItem)
	, startByte(startByte)
	, rowSize(rowSize)
	, cpuCoreRows(cpuCoreRows)
{
	Q_ASSERT(cpuCoreRows > 0.0);
}

int CallStack::callFunction(const GdbItemTree& tree)
{
	// Parameters to build a memory frame:
	QGraphicsItem* parent = this;
	size_t rowCount = 0;
	const QString& functionName = tree.findNodeTextValue("/frame/func") + "()";
	bool withGarbage = true;
	bool withLegs = true;

	// Build a memory frame for the new stack frame with the function name (/frame/func) in the roof
	// By default, memory frames are filled of garbage.
	MemoryFrame* functionCall = new MemoryFrame(parent, rowCount, startByte, rowSize, functionName, zValue, withGarbage, withLegs);

	// Add the functionCall to the list
	stackFrames.append(functionCall);

	// Add the function call frame to the scene
	insertItem(functionCall, 1.0, functionCall->getHeightInRows() / cpuCoreRows, zValue + 0.01 * stackFrames.count());
	updateLayoutItem();


	// Raise the memory roof to the CPU core opened door, to make the roof visible only.
	// Arguments and variables will be made visible next.
	qreal finalPercent = 1.0 - functionCall->getHeightInRows() / cpuCoreRows;
	int duration = functionCall->animateMoveTo( finalPercent, /* functionCall->getHeightInRows() * */ 1000, 1000 );

	// ToDo: create parameters and local variables
	return duration;
}
