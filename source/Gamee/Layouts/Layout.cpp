#include "Layout.h"
#include "Spacer.h"
#include "ScenicElement.h"

Layout::Layout()
{
}

Layout::~Layout()
{
	removeAllItems(true);
}

void Layout::addItem(LayoutItem* item, qreal proportion, qreal zValue)
{
	return insertItem(item, -1.0, proportion, zValue);
}

void Layout::insertItem(LayoutItem* item, qreal startProportion, qreal proportion, qreal zValue)
{
	Q_ASSERT(item);
	item->setParentLayoutItem(this);
	item->setStartProportion(startProportion);
	item->setProportion(proportion);
	item->setZ(zValue);
	items[zValue].append(item);
}

bool Layout::removeItem(LayoutItem* item, bool deleteObject)
{
	Q_ASSERT(item);
	for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
	{
		if ( itr.value().removeOne(item) )
		{
			if ( deleteObject )
				delete item;
			return true;
		}
	}

	return false;
}

void Layout::removeAllItems(bool removeItemsFromScene)
{
	// Delete all layout items execpt scenic elements, because they are children of the Scene
	for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
	{
		for ( int index = 0; index < itr.value().size(); ++index )
		{
			if ( itr.value()[index]->isScenicElement() )
			{
				if ( removeItemsFromScene )
					itr.value()[index]->removeFromScene();
			}
			else
				delete itr.value()[index];
		}
	}

	items.clear();
}

void Layout::addLayout(Layout* layout, qreal proportion, qreal zValue)
{
	return addItem(layout, proportion, zValue);
}

void Layout::addStretch(qreal proportion, qreal zValue)
{
	Spacer* spacer = new Spacer();
	spacer->setParentLayoutItem(this);
	spacer->setProportion(proportion);
	items[zValue].append(spacer);
}

qreal Layout::findZValue() const
{
	const Layout* parentLayout = dynamic_cast<const Layout*>(parentLayoutItem);
	return parentLayout ? parentLayout->findZValue(this): -1.0;
}

qreal Layout::findZValue(const LayoutItem* item) const
{
	for (ItemsType::const_iterator itr = items.constBegin(); itr != items.constEnd(); ++itr )
		for ( int i = 0; i < itr.value().size(); ++i )
			if ( item == itr.value()[i] )
				return itr.key();

	return -1.0;
}
