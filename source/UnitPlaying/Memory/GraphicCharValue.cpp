#include "GraphicCharValue.h"
#include "Actor.h"
#include "ExecutionThread.h"
#include "ExecutionThreadActor.h"
#include "Scene.h"
#include "VisualizationSpeed.h"

#include <QTimer>

// Duration in milliseconds of a char traveling from stdin to the thread or reverse direction
const int durationBufferThread = 1000;
// Duration waiting for remaining characters arrive to the thread or leaving it
const int durationWaitingOthers = 250;
// Duration of a "visual" delay to allow user to watch all characteres together
const int durationVisualDelay = 500;
// Duration of disappear animation
const int durationDisAppear = 250;

// Start proportion of a character in the standard output buffer. These values are saved when
// written characters are created in the output buffer, because they are moved to the thread and
// later they will return to the buffer
qreal mainStartInOutputBuffer = 0.0;
qreal crossStartInOutputBuffer = 0.0;


GraphicCharValue::GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value)
	: GraphicValue(typeChar, graphicsParent, zValue, value)
{
	buildGraphicValue();
}

int GraphicCharValue::animateRead(int index, int length, int ioBufferCapacity, ExecutionThread* targetThread, Scene* scene)
{
	// If this chacter is hidden, it will have to wait to appear in the stdin tube
	int duration = 0;
	if ( index > ioBufferCapacity )
		duration += (index - ioBufferCapacity) * 250;

	// Move this charater to its final position inside the tube
	// If this character was read (index < length) it will be moved to the position 0 where it will
	// be moved to the execution thread. Otherwise it will be only moved through the stdin tube to
	// the index - length position. Eg: thread read 4 chars. Chars in positions 0 through 3 will
	// end at position 0 before being extracted. Char in position 4 will end in position 0 after the
	// animation is done, char in position 5 will end in position 1, and so on...
	int finalPositionInBuffer = index < length ? 0 : index - length;

	// Animate this character to reach its final position
	qreal finalPercent = qreal(finalPositionInBuffer) / ioBufferCapacity;
	duration += animateMoveTo( finalPercent, (index - finalPositionInBuffer) * 250, duration );

	// If this char was not read, it will not extract towards the thread. Its animation is done
	if ( index >= length )
		return duration;

	// This character will be extracted from stdin tube and float towards the execution thread.
	// When it reaches the position 0 in the tube, it must be repareted to the scene, because it
	// it currently clipped by its stdin buffer parent
	this->scene = scene;
	this->executionThread = targetThread;
	this->index = index;
	this->length = length;
	QTimer::singleShot( duration, this, &GraphicCharValue::animateMoveToThread );

	// Add the duration that the move to thread animation will take later
	int moveToThreadDuration
		= durationBufferThread	// traveling from stdin to the thread
		+ (length - index) * durationWaitingOthers	// waiting for remaining characters arrive to the thread
		+ durationVisualDelay // delay to allow user to watch all characteres together
		+ durationDisAppear; // duration of disappear animation

	duration += VisualizationSpeed::getInstance().adjust(moveToThreadDuration);
	return duration;
}

int GraphicCharValue::animateMoveToThread()
{
	// We need to move this character accros the scene. Currently it is owned by the stdin buffer
	int duration = 0;
	reparentTo(scene);

	// Animate this character moving towards the thread position
	duration += animateMoveToPos(calculateVerticalScenePercent(), calculateHorizontalScenePercent(), durationBufferThread, duration);

	// Wait until other characters arrive to the thread
	duration += VisualizationSpeed::getInstance().adjust((length - index) * durationWaitingOthers + durationVisualDelay);

	// Disappear this character after the read is complete
	duration += animateAppear(durationDisAppear, duration, 1.0, 0.0);
	QTimer::singleShot( duration, this, &GraphicCharValue::removeCharFromScene );
	return duration;
}

qreal GraphicCharValue::calculateHorizontalScenePercent() const
{
	// Calculate the final position in execution thread's area. Some reference values:
	Q_ASSERT(executionThread);
	qreal sceneWidth = scene->getLayout()->getLayoutWidth();
	qreal threadLeft = executionThread->getLayoutLeft();
	qreal threadWidth = executionThread->getLayoutWidth();
	qreal charWidth = podMiddle->getLayoutWidth();

	// Horizontal position is centered in the execution thread
	qreal horizontalPercent = ( threadLeft + 0.5 * (threadWidth - length * charWidth) ) / sceneWidth;

	// Add the horizontal displacement according to the index of this character in the animation
	// 0.85 is an adjust to keep characters tight together
	horizontalPercent += index * charWidth / sceneWidth * 0.85;
	return horizontalPercent;
}

qreal GraphicCharValue::calculateVerticalScenePercent() const
{
	// Calculate the final position in execution thread's area. Some reference values:
	qreal sceneHeight = scene->getLayout()->getLayoutHeight();
	qreal actorTop = executionThread->getActor()->getLayoutTop();
	qreal actorHeight = executionThread->getActor()->getLayoutHeight();

	// Vertical position is the actor top + 75% of actors height's percent
	return (actorTop + actorHeight * 0.75 ) / sceneHeight;
}

void GraphicCharValue::placeInThread(int index, int length, ExecutionThread* thread, Scene* scene)
{
	// Copy given parameters
	this->index = index;
	this->length = length;
	this->executionThread = thread;
	this->scene = scene;

	// We save the current position in the scene. It will be used later to animate this character
	// returning to the output buffer
	mainStartInOutputBuffer = mainStart;
	crossStartInOutputBuffer = crossStart;

	// Add the character to the scene in its positions
//	setMainProportion( calculateVerticalScenePercent() );
//	setCrossProportion( calculateHorizontalScenePercent() );
	// Ugly fix: if we animate the final position, it works
	animateMoveToPos( calculateVerticalScenePercent(), calculateHorizontalScenePercent(), 0 );
}

int GraphicCharValue::animateWrite(InputOutputBuffer* targetBuffer)
{
	// Animate this character appearing, not all at the same time
	int duration = VisualizationSpeed::getInstance().adjust( durationDisAppear * index );
	duration += animateAppear(durationDisAppear, duration);

	// Animate this character moving towards the thread position
	duration += animateMoveToPos(mainStartInOutputBuffer, crossStartInOutputBuffer, durationBufferThread * 1.5, duration);

	// Temporary
	QTimer::singleShot( duration + 1000, this, &GraphicCharValue::removeCharFromScene );

	// Done
	(void)targetBuffer;
	return duration;
}

void GraphicCharValue::removeCharFromScene()
{
	removeAllItems(true);
}

#include "LabelButton.h"
bool GraphicCharValue::reparentTo(Scene* newParent)
{
	// ToDo: This method has code that must be spread to ancestors
	// Do not call setParent(). It comes from QObject
	Q_ASSERT(newParent);
	this->scene = newParent;

	// Reparent all children from GraphicValue
	graphicsParent = newParent;
	if ( podLeft ) podLeft->setParentItem(newParent);
	Q_ASSERT(podMiddle);
	podMiddle->setParentItem(newParent);
	if ( podRight ) podRight->setParentItem(newParent);
	if ( valueLabel ) valueLabel->setParentItem(newParent);

	// LinearLayoutActor::savedStartProportion ?
	// Layout::items?

	// LayoutItem:
	// Remove from old layout
	Layout* parentLayout = dynamic_cast<Layout*>( parentLayoutItem );
	Q_ASSERT(parentLayout);
	parentLayout->removeItem(this, false);

	mainStart = podMiddle->getLayoutTop() / scene->getLayout()->getLayoutHeight();
	qreal proportion = podMiddle->getLayoutHeight() / scene->getLayout()->getLayoutHeight();
	scene->getLayout()->insertItem(this, mainStart, proportion, zValue );

	setCrossStartProportion( podMiddle->getLayoutLeft() / scene->getLayout()->getLayoutWidth() );
	setCrossProportion( podMiddle->getLayoutWidth() / scene->getLayout()->getLayoutWidth() );

	scene->getLayout()->updateLayoutItem();
	return true;
}
