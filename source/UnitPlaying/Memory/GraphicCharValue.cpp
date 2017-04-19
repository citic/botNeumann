#include "GraphicCharValue.h"
#include "ExecutionThread.h"
#include "Scene.h"

#include "Actor.h"
#include "LabelButton.h"

#include <QTimer>

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

	// If this character was not read, it will be only moved through the stdin tube
	if ( index >= length )
	{
		// To animate them arriving, we place them at the first non visible position
		qreal finalPercent = qreal( index - length ) / ioBufferCapacity;
		duration += animateMoveTo( finalPercent, (index - length) * 250, duration );
		return duration;
	}

	// This character will float towards the execution thread
	this->scene = scene;
	QTimer::singleShot( index * 250, this, &GraphicCharValue::reparentToScene );

	Q_UNUSED(targetThread);

	return duration;
}

#include "LogManager.h"
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

	mainStart = 0.5; //podMiddle->getLayoutTop() / scene->getLayout()->getLayoutHeight();
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

bool GraphicCharValue::reparentToScene()
{
	return reparentTo(scene);
}
