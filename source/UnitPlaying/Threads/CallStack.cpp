#include "CallStack.h"
#include "GdbItemTree.h"
#include "LogManager.h"
#include "MemoryFrame.h"
#include "MemoryMapper.h"
#include "Scene.h"
#include "VisualizationSpeed.h"

#include <QTimer>

const qreal paddingIncreaseTopBottom = 0.15;
const qreal paddingIncreaseLeftRight = 0.01;

CallStack::CallStack(int threadId, size_t startByte, size_t rowSize, size_t maxSize, qreal zValue, QGraphicsItem* graphicsParentItem)
	: RectLayoutItem(Qt::Vertical, zValue, graphicsParentItem)
	, threadId(threadId)
	, startByte(startByte)
	, maxSize(maxSize)
	, rowSize(rowSize)
{
}

int CallStack::callFunction(const GdbItemTree& tree, int initialDelay)
{
	// Make room for the new stack frame while the cpu core's memory interface is opening
	animatePaddingIncrease(-paddingIncreaseTopBottom, paddingIncreaseLeftRight, paddingIncreaseTopBottom, paddingIncreaseLeftRight, initialDelay);

	// We start the count of watches for a new function call
	watchCounts.append(0);

	// Parameters to build a memory frame:
	QGraphicsItem* parent = this;
	size_t rowCount = 0;
	const QString& functionName = tree.findNodeTextValue("/frame/func") + "()";
	bool withGarbage = true;
	bool withLegs = true;
	qCCritical(logTemporary, "Calling function %s", qPrintable(functionName));

	// Build a memory frame for the new stack frame with the function name (/frame/func) in the roof
	// By default, memory frames are filled of garbage.
	qreal zFunctionCall = zValue + 0.01 * stackFrames.count();
	MemoryFrame* functionCall = new MemoryFrame(parent, rowCount, startByte, rowSize, functionName, zFunctionCall, withGarbage, withLegs, maxSize);

	// Add the functionCall to the list
	stackFrames.append(functionCall);

	// Add the function call frame to the scene
	insertItem(functionCall, 1.0, functionCall->getHeightInRows() / cpuCoreRows, zFunctionCall);

	// Raise the memory roof to the CPU core opened door, to make the roof visible only.
	// The final position is calculated as the empty space on top of the memory frame
	Q_ASSERT(cpuCoreRows > 0.0);
	qreal finalPercent = 1.0 - functionCall->getHeightInRows() / cpuCoreRows;

	// Do the raise up animation of the empty memory frame with its roof and legs only
	// Parameters and local variables will be created and animated later
	return functionCall->animateMoveTo( finalPercent, functionCall->getHeightInRows() * 500, initialDelay );
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

int CallStack::animatePaddingIncrease(qreal topIncrease, qreal rightIncrease, qreal bottomIncrease, qreal leftInecrease, int duration, int initialDelay)
{
	// Increase paddings from the outtermost (bottom) function call to the innermost (top)
	for ( int index = 0; index < stackFrames.count(); ++index )
		stackFrames[index]->animatePaddingIncrease(topIncrease, rightIncrease, bottomIncrease, leftInecrease, duration, initialDelay);

	return duration;
}

int CallStack::createLocalVariables(const GdbTreeNode* gdbVariableArray, int initialDelay)
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
	// Do nothing if no local variables are to be created
	int duration = 0;
	if ( gdbVariableArray->getChildCount() <= 0 )
		return duration;

	// The function call where local variabels will be defined is the topmost one
	Q_ASSERT(stackFrames.count() > 0);
	MemoryFrame* functionCall = stackFrames.top();

	// In order to know if the stack frame grew
	size_t frameRows = functionCall->getRowCount();

	// For each variable
	for ( int varIndex = 0; varIndex < gdbVariableArray->getChildCount(); ++varIndex )
	{
		// Create the variable on the stack
		const GdbTreeNode* variableNode = gdbVariableArray->getChild(varIndex);
		int variableDuration = createLocalVariable(variableNode, functionCall, initialDelay);

		// Adjust duration and the delay for the next variable
		duration += variableDuration;
		initialDelay += variableDuration;
	}

	// The stack frame may have grown, update its proportion
	if ( functionCall->getRowCount() != frameRows )
	{
		// Raise the memory rows and make them visible
		// ToDo: re-open the memory interface on cpu core
		Q_ASSERT(cpuCoreRows > 0.0);
		qreal finalPercent = 1.0 - functionCall->getHeightInRows() / cpuCoreRows;
		functionCall->setMainProportion( functionCall->getHeightInRows() / cpuCoreRows );
		duration += functionCall->animateMoveTo( finalPercent, functionCall->getHeightInRows() * 1000, /*initialDelay*/ 0 );
	}

	return duration;
}

int CallStack::createLocalVariable(const GdbTreeNode* variableNode, MemoryFrame* functionCall, int initialDelay)
{
	Q_ASSERT(variableNode);
	Q_ASSERT(functionCall);

	// Get the fields of the variable
	const QString& name = variableNode->findTextValue("name");
	const QString& type = variableNode->findTextValue("type");
	const QString& value = variableNode->findTextValue("value");
	qCCritical(logTemporary()) << "CallStack::createParameters() name:" << name << "type:" << type << "value:" << value;

	// Ignore variables that begin with double underscore (__). These variables are introduced
	// in object code by the programming language standard or compilers. Eg: __PRETTY_FUNCTION__
	if ( name.startsWith("__") )
		return 0;

	// Experimental: we set a watch also for local variables
	const QString& watchName = buildWatchName();

	// Create a mapping
	Q_ASSERT( MemoryMapper::getInstance() );
	MemoryAllocation* variable = MemoryMapper::getInstance()->createLocal(name, type, value, watchName);

	// Finally allocate a graphical variable in the stack frame
	Q_ASSERT(variable);
	return functionCall->allocate(variable, initialDelay);
}

QString CallStack::buildWatchName(int variableNumber)
{
	if ( variableNumber <= 0 )
		variableNumber = ++watchCounts.top();
	return QString("bn_lv_%1_%2_%3").arg(threadId).arg(stackFrames.count()).arg(variableNumber);
}

int CallStack::returnFunction(int initialDelay)
{
	// The function call to be returned
	Q_ASSERT( stackFrames.count() > 0 );
	MemoryFrame* functionCall = stackFrames.top();
	Q_ASSERT( functionCall );
	qCCritical(logTemporary, "Return from function %d", stackFrames.count());

	// Aniate it moving into the memory interface of the cpu core
	int duration = functionCall->animateMoveTo( 1.0, functionCall->getHeightInRows() * 500, initialDelay );

	// Move the remaining function calls to the front in order to use the room that the finished
	// function left while the cpu core's memory interface is closing
	animatePaddingIncrease(paddingIncreaseTopBottom, -paddingIncreaseLeftRight, -paddingIncreaseTopBottom, -paddingIncreaseLeftRight, initialDelay, duration);

	// Remove local variables from MemoryMapper and the stack frame when animation is finished
	QTimer::singleShot( initialDelay + duration, this, SLOT(removeFunctionCall()) );

	// Done
	return duration;
}

bool CallStack::removeFunctionCall()
{
	// Remove the topmost watch counter
	Q_ASSERT( watchCounts.count() > 0 );
	int watchCount = watchCounts.pop();

	// Delete watches for local variables in memory mapper
	MemoryMapper* memoryMapper = MemoryMapper::getInstance();
	Q_ASSERT( memoryMapper );
	for ( int index = 1; index <= watchCount; ++index )
		memoryMapper->removeWatch( buildWatchName(index) );

	// Removes the topmost stackframe from the scene and memory
	Q_ASSERT( stackFrames.count() > 0 );
	stackFrames.pop()->removeAllItems(true);

	// Done
	return true;
}
