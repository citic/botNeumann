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
	/// A layout is a layout item also
	virtual void addLayout(Layout* layout, qreal proportion, qreal zValue = 0.0);
	/// Add expansible space
	virtual void addStretch(qreal proportion, qreal zValue = 0.0);
};

#endif // LAYOUT_H
