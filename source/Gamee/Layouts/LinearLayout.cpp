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
				LayoutItem* child = itr.value()[i];

				if ( child->getMainStartProportion() >= 0.0 )
					left = reset + width * child->getMainStartProportion();

				qreal childWidth = width * child->getMainProportion();

				qreal childTop = top;
				if ( child->getCrossStartProportion() > 0.0 )
					childTop += height * child->getCrossStartProportion();

				qreal childHeight = height;
				if ( child->getCrossProportion() > 0.0 )
					childHeight = height * child->getCrossProportion();

				child->resize(left, childTop, childWidth, childHeight);
				left += childWidth;
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
				LayoutItem* child = itr.value()[i];

				if ( child->getMainStartProportion() >= 0.0 )
					top = reset + height * child->getMainStartProportion();

				qreal childHeight = height * child->getMainProportion();

				qreal childLeft = left;
				if ( child->getCrossStartProportion() > 0.0 )
					childLeft += width * child->getCrossStartProportion();

				qreal childWidth = width;
				if ( child->getCrossProportion() > 0.0 )
					childWidth = width * child->getCrossProportion();

				child->resize(childLeft, top, childWidth, childHeight);
				top += childHeight;
			}
		}
	}
}
