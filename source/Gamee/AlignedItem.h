#ifndef ALIGNEDITEM_H
#define ALIGNEDITEM_H

#include <QGraphicsItem>

/// Abstract interface to provide alignment to any QGraphicsItem
class AlignedItem
{
  protected:
	/// Aligns the contents of this item adjusting its left or right paddings automatically
	Qt::Alignment alignment = Qt::AlignJustify;
	/// For items that require to be horizontally skewed (shear or slanting)
	qreal shearX = 0.0;
	/// For items that require to be vertically skewed (shear or slanting)
	qreal shearY = 0.0;

  public:
	/// Aligns the contents of this item adjusting its left or right paddings automatically
	inline void setAlignment(Qt::Alignment alignment) { this->alignment = alignment; }
	inline const Qt::Alignment& getAlignment() const { return alignment; }
	inline void alignLeft() { this->alignment = Qt::AlignLeft | Qt::AlignVCenter; }
	inline void alignCenter() { this->alignment = Qt::AlignHCenter | Qt::AlignVCenter; }
	inline void alignRight() { this->alignment = Qt::AlignRight | Qt::AlignVCenter; }
	/// Set the horizontal and vertical skew. shearX and shearY are valuees between [-1, 1]
	inline void setShearX(qreal shearX, qreal shearY) { this->shearX = shearX; this->shearY = shearY; }
	/// Resizes the graphic part of the given element and applies the alignment
	void resizeItem(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, bool mapToParent = true);

  protected:
	/// Applies alignment when resizing this element
	virtual void applyAlignment(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight);
};

#endif // ALIGNEDITEM_H
