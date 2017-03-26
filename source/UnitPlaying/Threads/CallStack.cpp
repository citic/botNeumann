#include "CallStack.h"
#include "GdbItemTree.h"
#include "MemoryFrame.h"
#include "MemoryMapper.h"
#include "Scene.h"
#include "VisualizationSpeed.h"

CallStack::CallStack(size_t startByte, size_t rowSize, qreal zValue, QGraphicsItem* graphicsParentItem)
	: RectLayoutItem(Qt::Vertical, zValue, graphicsParentItem)
	, startByte(startByte)
	, rowSize(rowSize)
{
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
	Q_ASSERT(cpuCoreRows > 0.0);
	qreal finalPercent = 1.0 - functionCall->getHeightInRows() / cpuCoreRows;
	int duration = functionCall->animateMoveTo( finalPercent, /* functionCall->getHeightInRows() * */ 1000, 1000 );

	// ToDo: create parameters and local variables
	return duration;
}

int CallStack::animateAppear(int initialDelay)
{
	// Adjust animation time
	initialDelay = VisualizationSpeed::getInstance().adjust(initialDelay);
	int maxDuration = 0;

	// We animate each stack frame separately starting with the last one
	for ( int index = stackFrames.count() - 1; index >= 0; --index )
	{
		// Get the current percent where the stack frame starts
		MemoryFrame* stackFrame = stackFrames[index];

		// Move the stack to the basement, and raise it again
		int stackFrameDelay = initialDelay + 250 * (stackFrames.count() - index);
		int duration = stackFrame->animateMoveTo(stackFrame->getSavedStartProportion(), stackFrameDelay);

		// Animations are done in parallel
		if ( duration > maxDuration )
			maxDuration = duration;
	}

	return initialDelay + maxDuration;
}

int CallStack::animateDisappear(int initialDelay)
{
	// Adjust animation time
	initialDelay = VisualizationSpeed::getInstance().adjust(initialDelay);
	int maxDuration = 0;

	// We animate each stack frame separately starting with the first one
	for ( int index = 0; index < stackFrames.count(); ++index )
	{
		// Get the current percent where the stack frame starts
		MemoryFrame* stackFrame = stackFrames[index];

		// Move the stack to the basement, and raise it again
		stackFrame->saveStartProportion();
		int stackFrameDelay = initialDelay + 250 * index;
		int duration = stackFrame->animateMoveTo(1.0, stackFrameDelay);

		// Animations are done in parallel
		if ( duration > maxDuration )
			maxDuration = duration;
	}

	return initialDelay + maxDuration;
}

#include "LogManager.h"
int CallStack::createParameters(const GdbItemTree& tree)
{
	// Example of GDB response to `-stack-list-arguments 2 0 0`
	/*
		^done,
		stack-args=
		[
			frame=
			{
				level="0",
				args=
				[
					{
						name="this",
						type="InputArgument * const",
						value="0x60b580 <global_program_name>"
					},
					{
						name="number",
						type="ull",
						value="0"
					},
					{
						name="value",
						type="const char *",
						value="0x406dc6 \"all_inclusive\""
					}
				]
			}
		]
		(gdb)
	*/

	// Get the array of arguments from the node tree
	const GdbTreeNode* argumentsNode = tree.findNode("/stack-args/#1/args");
	Q_ASSERT( argumentsNode );

	// For each argument
	for ( int argIndex = 0; argIndex < argumentsNode->getChildCount(); ++argIndex )
	{
		// Get the argument node
		const GdbTreeNode* argumentNode = argumentsNode->getChild(argIndex);
		Q_ASSERT(argumentNode);

		// Get the fields of the argument
		const QString& argumentName = argumentNode->findTextValue("name");
		const QString& argumentType = argumentNode->findTextValue("type");
		const QString& argumentValue = argumentNode->findTextValue("value");

		// Ignore variables that begin with double underscore (__). These variables are introduced
		// in object code by the programming language standard or compilers. Eg: __PRETTY_FUNCTION__
		if ( argumentName.startsWith("__") )
			continue;

		Q_ASSERT( MemoryMapper::getInstance() );
		qCCritical(logTemporary()) << "CallStack::createParameters() name:" << argumentName << "type:" << argumentType << "value:" << argumentValue;
	}

	return 0;
}
