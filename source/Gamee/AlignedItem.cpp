#include "AlignedItem.h"

void AlignedItem::applyAlignment(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight)
{
	Q_ASSERT(item);

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

void AlignedItem::resizeItem(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height)
{
	Q_ASSERT(item);
	qreal scaleWidth = width / item->boundingRect().width();
	qreal scaleHeight = height / item->boundingRect().height();
	if ( ! alignment.testFlag(Qt::AlignJustify) )
		applyAlignment(item, left, top, width, height, scaleWidth, scaleHeight);
	item->setTransform(QTransform().scale(scaleWidth, scaleHeight));

	// If this element is a nested child, map coordinates from scene to its parent
	QPointF posChild(left, top);
	QGraphicsItem* parent = item->parentItem();
	if ( parent )
		posChild = parent->mapFromScene(posChild);
	item->setPos(posChild);
}
