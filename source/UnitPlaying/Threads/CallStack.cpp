#include "CallStack.h"
#include "GdbItemTree.h"
#include "LogManager.h"
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
	// We start the count of watches for a new function call
	watchCount = 0;

	// Parameters to build a memory frame:
	QGraphicsItem* parent = this;
	size_t rowCount = 2;
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

int CallStack::createLocalVariables(const GdbTreeNode* gdbVariableArray)
{
	// Example of GDB response to `-stack-list-arguments 2 0 0`
	/*
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
	*/

	// For each variable
	for ( int varIndex = 0; varIndex < gdbVariableArray->getChildCount(); ++varIndex )
	{
		// Get the argument or local variable node
		const GdbTreeNode* variableNode = gdbVariableArray->getChild(varIndex);
		Q_ASSERT(variableNode);

		// Get the fields of the variable
		const QString& name = variableNode->findTextValue("name");
		const QString& type = variableNode->findTextValue("type");
		const QString& value = variableNode->findTextValue("value");
		qCCritical(logTemporary()) << "CallStack::createParameters() name:" << name << "type:" << type << "value:" << value;

		// Ignore variables that begin with double underscore (__). These variables are introduced
		// in object code by the programming language standard or compilers. Eg: __PRETTY_FUNCTION__
		if ( name.startsWith("__") )
			continue;

		// Experimental: we set a watch also for local variables
		int threadId = 1;
		const QString& watchName = QString("bn_lv_%1_%2_%3").arg(threadId).arg(stackFrames.count()).arg(++watchCount);

		// Create a mapping
		Q_ASSERT( MemoryMapper::getInstance() );
		MemoryAllocation* variable = MemoryMapper::getInstance()->createLocal(name, type, value, watchName);

		// Finally allocate a graphical variable in the stack frame
		Q_ASSERT(variable);
		Q_ASSERT(stackFrames.count() > 0);
		stackFrames[ stackFrames.count() - 1 ]->allocate(variable);
	}

	return 0;
}
