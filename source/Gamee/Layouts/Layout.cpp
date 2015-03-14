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
	item->setProportion(proportion);
	item->setZ(zValue);
	items[zValue].append(item);
}

void Layout::addLayout(Layout* layout, qreal proportion, qreal zValue)
{
	return addItem(layout, proportion, zValue);
}

void Layout::addStretch(qreal proportion, qreal zValue)
{
	Spacer* spacer = new Spacer();
	spacer->setProportion(proportion);
	items[zValue].append(spacer);
}
