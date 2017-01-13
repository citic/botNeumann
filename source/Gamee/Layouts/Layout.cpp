#include "Layout.h"
#include "Spacer.h"
#include "ScenicElement.h"

Layout::Layout()
{
}

Layout::~Layout()
{
	// Delete all layout items execpt scenic elements, because they are children of the Scene
	for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
		for ( int i = 0; i < itr.value().size(); ++i )
			if ( ! itr.value()[i]->isScenicElement() )
				delete itr.value()[i];
}

void Layout::addItem(LayoutItem* item, qreal proportion, qreal zValue)
{
	Q_ASSERT(item);
	item->setParentItem(this);
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

void Layout::addLayout(Layout* layout, qreal proportion, qreal zValue)
{
	return addItem(layout, proportion, zValue);
}

void Layout::addStretch(qreal proportion, qreal zValue)
{
	Spacer* spacer = new Spacer();
	spacer->setParentItem(this);
	spacer->setProportion(proportion);
	items[zValue].append(spacer);
}

qreal Layout::findZValue() const
{
	const Layout* parentLayout = dynamic_cast<const Layout*>(parentItem);
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
