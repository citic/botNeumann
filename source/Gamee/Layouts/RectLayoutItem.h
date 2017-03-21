#ifndef RECTLAYOUTITEM_H
#define RECTLAYOUTITEM_H

#include "AlignedItem.h"
#include "LinearLayout.h"

#include <QGraphicsRectItem>

/**
	A RectLayoutItem is an invisible QGraphicsRectItem that:

	* can be placed on any part of the scene using the LayoutItem system
	* manages placing of its children using a linear layout
	* can clip its children
*/
class RectLayoutItem : public QGraphicsRectItem, public LinearLayout, public AlignedItem
{
	Q_DISABLE_COPY(RectLayoutItem)

  protected:
	/// zValue where the characteres will be placed on the scene
	qreal zValue = 0.0;

  public:
	/// Constructor
	explicit RectLayoutItem(Qt::Orientation orientation, qreal zValue, QGraphicsItem* graphicsParentItem, bool clipChildren = true);
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
	/// Sets the Z-index provided by layouts to the QGraphicsItem system
	virtual void setZ(qreal zValue) override { setZValue(zValue); }
};

#endif // RECTLAYOUTITEM_H
