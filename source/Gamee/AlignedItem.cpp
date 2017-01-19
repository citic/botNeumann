#include "AlignedItem.h"

void AlignedItem::applyAlignment(qreal& left, qreal& top, qreal width, qreal height, qreal& scaleWidth, qreal& scaleHeight)
{
	QGraphicsItem* thisItem = dynamic_cast<QGraphicsItem*>(this);
	Q_ASSERT(this);

	if ( scaleWidth > scaleHeight )
	{
		//if ( alignment.testFlag(Qt::AlignLeft) ) left += 0; else
		if ( alignment.testFlag(Qt::AlignHCenter) )
			left += 0.5 * (width - scaleHeight * thisItem->boundingRect().width());
		else if ( alignment.testFlag(Qt::AlignRight) )
			left += width - scaleHeight * thisItem->boundingRect().width();
		scaleWidth = scaleHeight;
	}
	else
	{
		//if ( alignment.testFlag(Qt::AlignTop) ) top += 0; else
		if ( alignment.testFlag(Qt::AlignVCenter) )
			top += 0.5 * (height - scaleWidth * thisItem->boundingRect().height());
		else if ( alignment.testFlag(Qt::AlignBottom) )
			top += height - scaleWidth * thisItem->boundingRect().height();
		scaleHeight = scaleWidth;
	}
}
