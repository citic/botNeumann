#include "RectLayoutItem.h"

#include <QBrush>
#include <QPen>

RectLayoutItem::RectLayoutItem(Qt::Orientation orientation, qreal zValue, QGraphicsItem* graphicsParentItem, bool clipChildren)
	: QGraphicsRectItem(graphicsParentItem)
	, LinearLayout(orientation)
	, zValue(zValue)
{
	// By default all RectLayoutItems clip children
	if ( clipChildren )
		setFlag(ItemClipsChildrenToShape);

	// By default, no border nor fill
	setPen(Qt::NoPen);
	setBrush(Qt::NoBrush);
}

void RectLayoutItem::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Update the LinearLayout part of this object
	LinearLayout::resize(left, top, width, height);

	// Update the QGraphicsRectIem part of this object
	applyMargins(left, top, width, height);
	setRect(left, top, width, height);
}
