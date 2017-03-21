#include "RectLayoutItem.h"

RectLayoutItem::RectLayoutItem(Qt::Orientation orientation, qreal zValue, QGraphicsItem* graphicsParentItem, bool clipChildren)
	: QGraphicsRectItem(graphicsParentItem)
	, LinearLayout(orientation)
	, zValue(zValue)
{
	// By default all RectLayoutItems clip children
	if ( clipChildren )
		setFlag(ItemClipsChildrenToShape);
}

void RectLayoutItem::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Update the LinearLayout part of this object
	LinearLayout::resize(left, top, width, height);

	// Update the QGraphicsRectIem part of this object
	applyMargins(left, top, width, height);
	setRect(left, top, width, height);
}
