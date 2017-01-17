#ifndef SCENICELEMENT_H
#define SCENICELEMENT_H

#include "Layout.h"
#include <QGraphicsItem>

/// Some disabled elements are shown transparent, this is the default opacity
const qreal defaultDisabledOpacity = 0.2;

template <typename GraphicsType>
class ScenicElement : public GraphicsType, public LayoutItem
{
	Q_DISABLE_COPY(ScenicElement)

  protected:
	/// Aligns the contents of this item adjusting its left or right margins automatically
	Qt::Alignment alignment = Qt::AlignJustify;
	/// Manages the position and size of children items of this item. When an item is added to this
	/// item, they become children and they are managed and destroyed by QGraphicsItem class.
	/// Changes applied to this QGraphicsItem, such as position and placing affect the children
	/// But the initial positioning of children and their size, is managed by this layout
	Layout* layout = nullptr;

  public:
	/// Constructor
	explicit ScenicElement(QGraphicsItem* parentItem)
		: GraphicsType(parentItem)
	{
	}
	/// Creates an actor with the given svg filename
	explicit ScenicElement(const QString& textOrFilename, QGraphicsItem* parentItem)
		: GraphicsType(textOrFilename, parentItem)
	{
	}
	/// Destructor
	virtual ~ScenicElement()
	{
		delete layout;
	}
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isScenicElement() const override { return true; }
	/// Forwards the z-value used by layout items to QGraphicItem family
	virtual void setZ(qreal z) override { GraphicsType::setZValue(z); }
	/// Aligns the contents of this item adjusting its left or right margins automatically
	inline void setAlignment(Qt::Alignment alignment) { this->alignment = alignment; }
	inline const Qt::Alignment& getAlignment() const { return alignment; }
	inline void alignLeft() { this->alignment = Qt::AlignLeft | Qt::AlignVCenter; }
	inline void alignCenter() { this->alignment = Qt::AlignHCenter | Qt::AlignVCenter; }
	inline void alignRight() { this->alignment = Qt::AlignRight | Qt::AlignVCenter; }
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override
	{
		// Resize children before changing the values (left, top, width, height)
		if ( layout ) layout->resize(left, top, width, height);

		// Update the LayoutItem part of this object
		LayoutItem::resize(left, top, width, height);
		applyMargins(left, top, width, height);

		// Resize the QGraphicaItem part of this object
		qreal scaleWidth = width / GraphicsType::boundingRect().width();
		qreal scaleHeight = height / GraphicsType::boundingRect().height();
		if ( ! alignment.testFlag(Qt::AlignJustify) )
			applyAlignment(left, top, width, height, scaleWidth, scaleHeight);
		GraphicsType::prepareGeometryChange();
		GraphicsType::setTransform(QTransform().scale(scaleWidth, scaleHeight));

		// If this element is a nested child, map coordinates from scene to its parent
		QPointF posChild(left, top);
		QGraphicsItem* parent = GraphicsType::parentItem();
		if ( parent->parentItem() )
			posChild = parent->mapFromScene(left, top);
		GraphicsType::setPos(posChild);		
	}

  protected:
	void applyAlignment(qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight)
	{
		if ( scaleWidth > scaleHeight )
		{
			//if ( alignment.testFlag(Qt::AlignLeft) ) left += 0; else
			if ( alignment.testFlag(Qt::AlignHCenter) )
				left += 0.5 * (width - scaleHeight * GraphicsType::boundingRect().width());
			else if ( alignment.testFlag(Qt::AlignRight) )
				left += width - scaleHeight * GraphicsType::boundingRect().width();
			scaleWidth = scaleHeight;
		}
		else
		{
			//if ( alignment.testFlag(Qt::AlignTop) ) top += 0; else
			if ( alignment.testFlag(Qt::AlignVCenter) )
				top += 0.5 * (height - scaleWidth * GraphicsType::boundingRect().height());
			else if ( alignment.testFlag(Qt::AlignBottom) )
				top += height - scaleWidth * GraphicsType::boundingRect().height();
			scaleHeight = scaleWidth;
		}
	}
};

#endif // SCENICELEMENT_H
