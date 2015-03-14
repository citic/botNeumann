#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QtGlobal>

/** Base class for layouts, scenic objects, spacers and widgets */
class LayoutItem
{
  public:
	/// Constants to access each margin
	enum { marginTop, marginRight, marginBottom, marginLeft, marginCount };

  protected:
	/// Percent of extension (aka proportion) this item will occupy in parent's space
	qreal proportion;
	/// Percent of extension of each margin of this item
	qreal margins[marginCount];
	/// True if this item must maintain aspect ratio when resized
	bool maintainAspectRatio;
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
	virtual void resize(qreal left, qreal top, qreal width, qreal height) = 0;
	/// The proportion of space this item occupies of its layout
	inline qreal getProportion() const { return proportion; }
	inline void setProportion(qreal proportion) { this->proportion = proportion; }
	/// Scenic elements require this method.
	/// This is dirty hack, it is no implemented by other LayoutItems
	virtual void setZ(qreal) { }
	/// Set the percent of margin for all the margins
	void setMargins(qreal sameForAll);
	void setMargins(qreal topBottom, qreal leftRight);
	void setMargins(qreal top, qreal leftRight, qreal bottom);
	void setMargins(qreal top, qreal right, qreal bottom, qreal left);
	/// Locks the aspect ratio of this item
	inline bool isMaintainingAspectRatio() const { return maintainAspectRatio; }
	inline void makeMaintainAspectRatio(bool newState = true) { maintainAspectRatio = newState; }
	/// True if the item can float over other items
	inline bool isFloating() const { return floating; }
	inline void makeFloat(bool newState = true) { floating = newState; }

  protected:
	/// Adjust the given variables reducing the margins
	void applyMargins(qreal& left, qreal& top, qreal& width, qreal& height);
};

#endif // LAYOUTITEM_H
