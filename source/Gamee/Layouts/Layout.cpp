#include "Layout.h"
#include "Spacer.h"
#include "ScenicElement.h"

Layout::Layout()
{
}

Layout::~Layout()
{
	// Delete all layout items execpt scenic elements, because they are children of the Scene
	for ( int i = 0; i < items.size(); ++i )
		if ( ! dynamic_cast<ScenicElement*>(items[i]) )
			delete items[i];
}

void Layout::addItem(LayoutItem* item, qreal proportion)
{
	Q_ASSERT(item);
	item->setProportion(proportion);
	items.append(item);
}

void Layout::addLayout(Layout* layout, qreal proportion)
{
	return addItem(layout, proportion);
}

void Layout::addStretch(qreal proportion)
{
	Spacer* spacer = new Spacer();
	spacer->setProportion(proportion);
	items.append(spacer);
}
