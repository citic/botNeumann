#include "AlignedItem.h"
#include "VisualizationSpeed.h"

#include <QObject>
#include <QPropertyAnimation>

void AlignedItem::applyAlignment(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight)
{
	Q_ASSERT(item);

	// Justified alignment uses the whole width and height, we do not need to inject space
	if ( alignment.testFlag(Qt::AlignJustify) )
		return;

	if ( scaleWidth > scaleHeight )
	{
		//if ( alignment.testFlag(Qt::AlignLeft) ) left += 0; else
		if ( alignment.testFlag(Qt::AlignHCenter) )
			left += 0.5 * (width - scaleHeight * item->boundingRect().width());
		else if ( alignment.testFlag(Qt::AlignRight) )
			left += width - scaleHeight * item->boundingRect().width();
		scaleWidth = scaleHeight;
	}
	else
	{
		//if ( alignment.testFlag(Qt::AlignTop) ) top += 0; else
		if ( alignment.testFlag(Qt::AlignVCenter) )
			top += 0.5 * (height - scaleWidth * item->boundingRect().height());
		else if ( alignment.testFlag(Qt::AlignBottom) )
			top += height - scaleWidth * item->boundingRect().height();
		scaleHeight = scaleWidth;
	}
}

void AlignedItem::resizeItem(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, bool mapToParent)
{
	Q_ASSERT(item);
	qreal scaleWidth = width / item->boundingRect().width();
	qreal scaleHeight = height / item->boundingRect().height();
	applyAlignment(item, left, top, width, height, scaleWidth, scaleHeight);
	item->setTransform( QTransform().shear(shearX, shearY).scale(scaleWidth, scaleHeight) );

	// If this element is a nested child, map coordinates from scene to its parent
	QPointF posChild(left, top);
	QGraphicsItem* parent = item->parentItem();
	if ( mapToParent && parent )
		posChild = parent->mapFromScene(posChild);
	item->setPos(posChild);
}

int AlignedItem::animateAppear(QObject* object, int duration, int initialDelay, qreal fromOpacity, qreal toOpacity)
{
	Q_ASSERT(object);

	// Adjust animation time
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Animate the robot while it appears
	QPropertyAnimation* animation = new QPropertyAnimation(object, "opacity", object);
	animation->setDuration(totalDuration);

	// Do not change initial value on delay
	animation->setKeyValueAt(0.0, fromOpacity);
	if ( totalDuration > 0 )
		animation->setKeyValueAt(initialDelay / totalDuration, fromOpacity);
	animation->setKeyValueAt(1.0, toOpacity);

	animation->start();
	return duration;
}
