#include "GraphicCharValue.h"
#include "ExecutionThread.h"
#include "ExecutionThreadActor.h"
#include "LogManager.h"
#include "Scene.h"
#include "VisualizationSpeed.h"

#include "Actor.h"
#include "LabelButton.h"

#include <QTimer>

// Duration in milliseconds of a char traveling from stdin to the thread or reverse direction
const int durationBufferThread = 1000;
// Duration waiting for remaining characters arrive to the thread or leaving it
const int durationWaitingOthers = 250;
// Duration of a "visual" delay to allow user to watch all characteres together
const int durationVisualDelay = 500;
// Duration of disappear animation
const int durationDisappear = 250;

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
		+ durationDisappear; // duration of disappear animation

	duration += VisualizationSpeed::getInstance().adjust(moveToThreadDuration);
	return duration;
}

int GraphicCharValue::animateMoveToThread()
{
	// We need to move this character accros the scene. Currently it is owned by the stdin buffer
	int duration = 0;
	reparentTo(scene);

	// Calculate the final position in execution thread's area
	Q_ASSERT(executionThread);
	qreal mainPercent = executionThread->getActor()->getLayoutTop() / scene->getLayout()->getLayoutHeight();
	qreal crossPercent = executionThread->getLayoutLeft() / scene->getLayout()->getLayoutWidth();

	// Add the horizontal displacement according to the index of this character in the animation
	// 0.85 is an adjust to keep characters tight together
	crossPercent += index * podMiddle->getLayoutWidth() / scene->getLayout()->getLayoutWidth() * 0.85;

	// Animate this character moving towards the thread position
	duration += animateMoveToPos(mainPercent, crossPercent, durationBufferThread, duration);

	// Wait until other characters arrive to the thread
	duration += VisualizationSpeed::getInstance().adjust((length - index) * durationWaitingOthers + durationVisualDelay);

	// ToDo: move this to IOBuffer
	// Disappear this character after the read is complete
	duration += animateAppear(durationDisappear, duration, 1.0, 0.0);
	QTimer::singleShot( duration, this, &GraphicCharValue::removeCharFromScene );
	return duration;
}

void GraphicCharValue::removeCharFromScene()
{
	removeAllItems(true);
}

bool GraphicCharValue::reparentTo(Scene* newParent)
{
	Q_ASSERT(newParent);
	this->scene = newParent;

	// Get coordinates in the new parent
	Q_ASSERT(podMiddle);
	const QPointF& scenePos = newParent->mapFromItem( graphicsParent, podMiddle->pos() );
	qCCritical(logTemporary, "Reaparent char to scene from (%lf, %lf) to (%lf, %lf)", podMiddle->pos().x(), podMiddle->pos().y(), scenePos.x(), scenePos.y());

	// Do not call setParent(). It comes from QObject

	// Reparent all children from GraphicValue
	graphicsParent = newParent;
	if ( podLeft ) podLeft->setParentItem(newParent);
	podMiddle->setParentItem(newParent);
	if ( podRight ) podRight->setParentItem(newParent);
	if ( valueLabel ) valueLabel->setParentItem(newParent);

	// LinearLayoutActor::savedStartProportion ?

	// Layout::items?

	// LayoutItem

	// Remove from old layout
	Layout* parentLayout = dynamic_cast<Layout*>( parentLayoutItem );
	Q_ASSERT(parentLayout);
	parentLayout->removeItem(this, false);

	mainStart = podMiddle->getLayoutTop() / scene->getLayout()->getLayoutHeight();
	qreal proportion = podMiddle->getLayoutHeight() / scene->getLayout()->getLayoutHeight();
	scene->getLayout()->insertItem(this, mainStart, proportion, zValue );

	setCrossStartProportion( podMiddle->getLayoutLeft() / scene->getLayout()->getLayoutWidth() );
	setCrossProportion( podMiddle->getLayoutWidth() / scene->getLayout()->getLayoutWidth() );
	/*
	LayoutItem* parentLayoutItem = nullptr;
	qreal proportion = 0.0;
	qreal margins[marginCount] = { 0 };
	qreal layoutLeft = 0.0;
	qreal layoutTop = 0.0;
	qreal layoutWidth = 0.0;
	qreal layoutHeight = 0.0;
	bool floating = false;
	*/
	scene->getLayout()->updateLayoutItem();
	return true;
}
