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

  protected:
	/// Aligns the contents of this item adjusting its left or right margins automatically
	Qt::Alignment alignment;

  public:
	/// Constructor
	explicit ScenicElement(QGraphicsItem* parentItem)
		: GraphicsType(parentItem)
		, alignment(Qt::AlignJustify)
	{
	}
	/// Creates an actor with the given svg filename
	explicit ScenicElement(const QString& textOrFilename, QGraphicsItem* parentItem)
		: GraphicsType(textOrFilename, parentItem)
		, alignment(Qt::AlignJustify)
	{
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
		applyMargins(left, top, width, height);
		qreal scaleWidth = width / GraphicsType::boundingRect().width();
		qreal scaleHeight = height / GraphicsType::boundingRect().height();
		if ( ! alignment.testFlag(Qt::AlignJustify) )
			applyAlignment(left, top, width, height, scaleWidth, scaleHeight);
		GraphicsType::setPos(left, top);
		GraphicsType::prepareGeometryChange();
		GraphicsType::setTransform(QTransform().scale(scaleWidth, scaleHeight));
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
