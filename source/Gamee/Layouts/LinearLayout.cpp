#include "LinearLayout.h"

LinearLayout::LinearLayout(Qt::Orientation orientation)
	: orientation(orientation)
{
}

LinearLayout::~LinearLayout()
{
}

#include <QDebug>

void LinearLayout::resize(qreal left, qreal top, qreal width, qreal height)
{
	qDebug() << "LInearLayout::resize(" << left << ',' << top << ',' << width << ',' << height << ')';
	// Reduce the margins from the given space
	applyMargins(left, top, width, height);

	// This if-else will be refactored, but after knowing it is working
	if ( orientation == Qt::Horizontal )
		for ( int i = 0; i < items.size(); ++i )
		{
			qreal extend = width * items[i]->getProportion();
			items[i]->resize(left, top, extend, height);
			left += extend;
		}
	else
		for ( int i = 0; i < items.size(); ++i )
		{
			qreal extend = height * items[i]->getProportion();
			items[i]->resize(left, top, width, extend);
			top += extend;
		}
}
