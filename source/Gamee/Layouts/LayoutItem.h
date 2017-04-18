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
	LayoutItem* parentLayoutItem = nullptr;
	/// Percent of the parent where this item starts. A negative value means it starts just after
	/// the previous element that has been added to the parent. Inspired in CSS3 flexbox layout
	qreal mainStart = -1.0;
	/// The start proportion on the cross axis. Inspired in CSS3 flexbox layout
	qreal crossStart = -1.0;
	/// Percent of extension (aka proportion) this item will occupy in parent's space in the same
	/// direction of the parent's main direction. Inspired in CSS3 flexbox layout
	qreal mainProportion = 0.0;
	/// Percent on the cross axis. Inspired in CSS3 flexbox layout
	qreal crossProportion = 0.0;
	/// Percent of extension of each margin of this item
	qreal margins[marginCount] = { 0 };
	/// Current dimensions of this item set in the last resize event
	qreal layoutLeft = 0.0;
	qreal layoutTop = 0.0;
	qreal layoutWidth = 0.0;
	qreal layoutHeight = 0.0;

  public:
	/// Constructor
	explicit LayoutItem(qreal mainProportion = 1.0);
	/// Destructor
	virtual ~LayoutItem();
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isActor() const { return false; }
	/// Called on QGraphicItem in order to remove them from the scene
	virtual void removeFromScene() { }
	/// Resize this item and all its child items
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height);
	/// Force to update this element, in the same fashion when a resize is done
	virtual void updateLayoutItem();
	/// The parent item that contains this one
	inline LayoutItem* getParentLayoutItem() const { return parentLayoutItem; }
	inline void setParentLayoutItem(LayoutItem* parent) { this->parentLayoutItem = parent; }
	/// The proportion in parent where this item starts in the parent's direction
	inline qreal getMainStartProportion() const { return mainStart; }
	inline void setMainStartProportion(qreal startProportion) { this->mainStart = startProportion; }
	/// The proportion of space this item occupies of its layout in parent's direction
	inline qreal getMainProportion() const { return mainProportion; }
	inline void setMainProportion(qreal proportion) { this->mainProportion = proportion; }
	/// The proportion in parent where this item starts
	inline qreal getCrossStartProportion() const { return crossStart; }
	inline void setCrossStartProportion(qreal startProportion) { this->crossStart = startProportion; }
	/// The proportion of space this item occupies of its layout
	inline qreal getCrossProportion() const { return crossProportion; }
	inline void setCrossProportion(qreal proportion) { this->crossProportion = proportion; }
	/// Scenic elements require this method.
	/// This is dirty hack, it is no implemented by other LayoutItems
	virtual void setZ(qreal) = 0;
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
	// Get access to the position of this item
	inline qreal getLayoutLeft() const { return layoutLeft; }
	inline qreal getLayoutTop() const { return layoutTop; }
	inline qreal getLayoutWidth() const { return layoutWidth; }
	inline qreal getLayoutHeight() const { return layoutHeight; }

  protected:
	/// Adjust the given variables reducing the margins
	void applyMargins(qreal& left, qreal& top, qreal& width, qreal& height);
	/// Given a margin and the size of the edge it applies, this method returns the actual amount
	/// of pixels that margin requires. This function considers a margin m between -1 < m < 1 as
	/// a percentage. Otherwise as an absolute amount of pixels
	qreal getPixelsForMargin(int margin, qreal edge) const;
};

#endif // LAYOUTITEM_H
