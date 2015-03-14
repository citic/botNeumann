#ifndef SCENICELEMENT_H
#define SCENICELEMENT_H

#include "LayoutItem.h"
#include <QGraphicsItem>

/// Some disabled elements are shown transparent, this is the default opacity
const qreal defaultDisabledOpacity = 0.2;

template <typename GraphicsType>
class ScenicElement : public GraphicsType, public LayoutItem
{
	Q_DISABLE_COPY(ScenicElement)

  public:
	/// Constructor
	explicit ScenicElement(QGraphicsItem* parentItem) : GraphicsType(parentItem) { }
	/// Creates an actor with the given svg filename
	explicit ScenicElement(const QString& textOrFilename, QGraphicsItem* parentItem)
		: GraphicsType(textOrFilename, parentItem) { }
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isScenicElement() const override { return true; }
	/// Forwards the z-value used by layout items to QGraphicItem family
	virtual void setZ(qreal z) override { GraphicsType::setZValue(z); }
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override
	{
		applyMargins(left, top, width, height);
		GraphicsType::setPos(left, top);
		GraphicsType::prepareGeometryChange();
		qreal sw = width / GraphicsType::boundingRect().width();
		qreal sh = height / GraphicsType::boundingRect().height();
		GraphicsType::setTransform(QTransform().scale(sw, sh));
	}
};

#endif // SCENICELEMENT_H
