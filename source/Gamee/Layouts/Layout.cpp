#include "Layout.h"
#include "Actor.h"
#include "Spacer.h"

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
	item->setMainStartProportion(startProportion);
	item->setMainProportion(proportion);
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
			if ( itr.value()[index]->isActor() )
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
	spacer->setMainProportion(proportion);
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

int Layout::animateAppear(int duration, int initialDelay, qreal fromOpacity, qreal toOpacity)
{
	// The duration of the animation is the max duration of children
	int maxDuration = 0;

	// Animate all children appearing at the same time (concurrently)
	for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
	{
		for ( int index = 0; index < itr.value().size(); ++index )
		{
			int itemDuration = itr.value()[index]->animateAppear(duration, initialDelay, fromOpacity, toOpacity);
			if ( itemDuration > maxDuration )
				maxDuration = itemDuration;
		}
	}

	return maxDuration;
}
