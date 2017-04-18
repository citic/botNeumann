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
	Layout::resize(left, top, width, height);
	applyMargins(left, top, width, height);

	if ( orientation == Qt::Horizontal )
	{
		qreal reset = left;
		for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
		{
			left = reset;
			for ( int i = 0; i < itr.value().size(); ++i )
			{
				LayoutItem* item = itr.value()[i];
				if ( item->getMainStartProportion() >= 0.0 )
					left = reset + width * item->getMainStartProportion();
				qreal extend = width * item->getMainProportion();
				item->resize(left, top, extend, height);
				left += extend;
			}
		}
	}
	else
	{
		qreal reset = top;
		for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
		{
			top = reset;
			for ( int i = 0; i < itr.value().size(); ++i )
			{
				LayoutItem* item = itr.value()[i];
				if ( item->getMainStartProportion() >= 0.0 )
					top = reset + height * item->getMainStartProportion();
				qreal extend = height * item->getMainProportion();
				item->resize(left, top, width, extend);
				top += extend;
			}
		}
	}
}
