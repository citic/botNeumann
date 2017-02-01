#ifndef ALIGNEDITEM_H
#define ALIGNEDITEM_H

#include <QGraphicsItem>

/// Abstract interface to provide alignment to any QGraphicsItem
class AlignedItem
{
  protected:
	/// Aligns the contents of this item adjusting its left or right margins automatically
	Qt::Alignment alignment = Qt::AlignJustify;

  public:
	/// Aligns the contents of this item adjusting its left or right margins automatically
	inline void setAlignment(Qt::Alignment alignment) { this->alignment = alignment; }
	inline const Qt::Alignment& getAlignment() const { return alignment; }
	inline void alignLeft() { this->alignment = Qt::AlignLeft | Qt::AlignVCenter; }
	inline void alignCenter() { this->alignment = Qt::AlignHCenter | Qt::AlignVCenter; }
	inline void alignRight() { this->alignment = Qt::AlignRight | Qt::AlignVCenter; }
	/// Resizes the graphic part of the given element and applies the alignment
	void resizeItem(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, bool mapToParent = true);

  protected:
	/// Applies alignment when resizing this element
	virtual void applyAlignment(QGraphicsItem* item, qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight);
};

#endif // ALIGNEDITEM_H
