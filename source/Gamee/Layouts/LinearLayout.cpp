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
		qreal reset = left;
		for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
		{
			left = reset;
			for ( int i = 0; i < itr.value().size(); ++i )
			{
				qreal extend = width * itr.value()[i]->getProportion();
				itr.value()[i]->resize(left, top, extend, height);
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
				qreal extend = height * itr.value()[i]->getProportion();
				itr.value()[i]->resize(left, top, width, extend);
				top += extend;
			}
		}
	}
}
