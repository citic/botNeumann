#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QtGlobal>

/** Base class for layouts, scenic objects, spacers and widgets */
class LayoutItem
{
	Q_DISABLE_COPY(LayoutItem)

  public:
	/// Constants to access each margin
	enum { marginTop, marginRight, marginBottom, marginLeft, marginCount };

  protected:
	/// Pointer to the parent item that contains this one
	/// @remarks Implemented but not used, as well as Layout::findZValue()
	LayoutItem* parentItem = nullptr;
	/// Percent of extension (aka proportion) this item will occupy in parent's space
	qreal proportion;
	/// Percent of extension of each margin of this item
	qreal margins[marginCount];
	/// Current dimensions of this item set in the last resize event
	qreal layoutLeft = 0.0;
	qreal layoutTop = 0.0;
	qreal layoutWidth = 0.0;
	qreal layoutHeight = 0.0;
	/// True if this item is floating over other items
	bool floating;

  public:
	/// Constructor
	explicit LayoutItem(qreal proportion = 1.0);
	/// Destructor
	virtual ~LayoutItem();
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isScenicElement() const { return false; }
	/// Resize this item and all its child items
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height);
	/// Force to update this element, in the same fashion when a resize is done
	virtual void updateLayoutItem();
	/// The parent item that contains this one
	inline LayoutItem* getParentItem() const { return parentItem; }
	inline void setParentItem(LayoutItem* parent) { this->parentItem = parent; }
	/// The proportion of space this item occupies of its layout
	inline qreal getProportion() const { return proportion; }
	inline void setProportion(qreal proportion) { this->proportion = proportion; }
	/// Scenic elements require this method.
	/// This is dirty hack, it is no implemented by other LayoutItems
	virtual void setZ(qreal) { }
	/// Set the percent of margin for all the margins
	inline void setMargins(qreal sameForAll) { setMargins(sameForAll, sameForAll); }
	inline void setMargins(qreal topBottom, qreal leftRight) { setMargins(topBottom, leftRight, topBottom, leftRight); }
	inline void setMargins(qreal top, qreal leftRight, qreal bottom) { setMargins(top, leftRight, bottom, leftRight); }
	void setMargins(qreal top, qreal right, qreal bottom, qreal left);
	/// Set individual margins
	inline void setMarginTop(qreal value) { margins[marginTop] = value; }
	inline void setMarginRight(qreal value) { margins[marginRight] = value; }
	inline void setMarginBottom(qreal value) { margins[marginBottom] = value; }
	inline void setMarginLeft(qreal value) { margins[marginLeft] = value; }
	/// Get access to each margin
	inline qreal getMarginTop() const { return margins[marginTop]; }
	inline qreal getMarginRight() const { return margins[marginRight]; }
	inline qreal getMarginBottom() const { return margins[marginBottom]; }
	inline qreal getMarginLeft() const { return margins[marginLeft]; }
	/// True if the item can float over other items
	inline bool isFloating() const { return floating; }
	inline void makeFloat(bool newState = true) { floating = newState; }

  protected:
	/// Adjust the given variables reducing the margins
	void applyMargins(qreal& left, qreal& top, qreal& width, qreal& height);
	/// Given a margin and the size of the edge it applies, this method returns the actual amount
	/// of pixels that margin requires. This function considers a margin m between -1 < m < 1 as
	/// a percentage. Otherwise as an absolute amount of pixels
	qreal getPixelsForMargin(int margin, qreal edge) const;
};

#endif // LAYOUTITEM_H
