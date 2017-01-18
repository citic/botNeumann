#ifndef LAYOUT_H
#define LAYOUT_H

#include "LayoutItem.h"
#include <QHash>
#include <QList>

/** Base class for layouts */
class Layout : public LayoutItem
{
	Q_DISABLE_COPY(Layout)

  protected:
	/// Items are stored in layers. Each layer has un z-index or z-value
	/// When the layout is resized, each layer is resized independently
	typedef QHash< qreal, QList<LayoutItem*> > ItemsType;
	/// Items are stored in layers. Each layer has un z-index or z-value
	/// Sub-items in order of this layout
	ItemsType items;

  public:
	/// Constructor
	Layout();
	/// Destructor
	virtual ~Layout();
	/// Add the given item to the sequence of items
	virtual void addItem(LayoutItem* item, qreal proportion, qreal zValue = 0.0);
	/// Add any Qt graphics item, including QGraphicsWidget
//	virtual void addItem(QGraphicsItem* widget);
	/// Removes the item from the layout, and the scene
	/// @param deleteObject send true if the object should be deleted from dynamic memory
	/// @return true if object was removed, false if it was not a child of this layout
	virtual bool removeItem(LayoutItem* item, bool deleteObject);
	/// Deletes all items
	/// @param removeFromScene if true, QGraphicsItems will be removed from the scene and deleted
	void removeAllItems();
	/// A layout is a layout item also
	virtual void addLayout(Layout* layout, qreal proportion, qreal zValue = 0.0);
	/// Add expansible space
	virtual void addStretch(qreal proportion, qreal zValue = 0.0);
	/// Find the zValue of this item in its parent, -1.0 if not added or not parent
	/// @remarks The search is done in linear time
	/// @remarks Implemented but not used, as well as parentItem
	qreal findZValue() const;
	/// Find the zValue of the given item, -1.0 if not found
	/// @remarks The search is done in linear time
	qreal findZValue(const LayoutItem* item) const;
};

#endif // LAYOUT_H
