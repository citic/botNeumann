#include "LinearLayout.h"

LinearLayout::LinearLayout(Qt::Orientation orientation)
	: orientation(orientation)
{
}

LinearLayout::~LinearLayout()
{
}

void LinearLayout::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Reduce the margins from the given space
	applyMargins(left, top, width, height);

	if ( orientation == Qt::Horizontal )
	{
		for ( int i = 0; i < items.size(); ++i )
		{
			qreal extend = width * items[i]->getProportion();
			items[i]->resize(left, top, extend, height);
			left += extend;
		}
	}
	else
	{
		for ( int i = 0; i < items.size(); ++i )
		{
			qreal extend = height * items[i]->getProportion();
			items[i]->resize(left, top, width, extend);
			top += extend;
		}
	}
}
