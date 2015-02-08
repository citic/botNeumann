#ifndef LAYOUT_H
#define LAYOUT_H

#include "LayoutItem.h"
#include <QList>

class QGraphicsItem;
class ScenicElement;

/** Base class for layouts */
class Layout : public LayoutItem
{
  protected:
	/// Sub-items in order of this layout
	QList<LayoutItem*> items;

  public:
	/// Constructor
	Layout();
	/// Destructor
	virtual ~Layout();
	/// Add the given item to the sequence of items
	virtual void addItem(LayoutItem* item, qreal proportion);
	/// Add any Qt graphics item, including QGraphicsWidget
//	virtual void addItem(QGraphicsItem* widget);
	/// A layout is a layout item also
	virtual void addLayout(Layout* layout, qreal proportion);
	/// Add expansible space
	virtual void addStretch(qreal proportion);
};

#endif // LAYOUT_H
