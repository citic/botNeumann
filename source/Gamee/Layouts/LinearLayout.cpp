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
				if ( item->getStartProportion() >= 0.0 )
					left = reset + width * item->getStartProportion();
				qreal extend = width * item->getProportion();
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
				if ( item->getStartProportion() >= 0.0 )
					top = reset + height * item->getStartProportion();
				qreal extend = height * item->getProportion();
				item->resize(left, top, width, extend);
				top += extend;
			}
		}
	}
}

// LinearLayoutActor class -----------------------------------------------------------------------

#include "VisualizationSpeed.h"
#include <QPropertyAnimation>

LinearLayoutActor::LinearLayoutActor(Qt::Orientation orientation)
	: LinearLayout(orientation)
{
}

int LinearLayoutActor::animateMoveTo(qreal endProportion, int duration, int initialDelay)
{
	// Adjust animation time
	initialDelay = VisualizationSpeed::getInstance().adjust(initialDelay);
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Create an animation and set its duration
	QPropertyAnimation* moveToAnimation = new QPropertyAnimation(this, "startProportion", this);
	moveToAnimation->setDuration(totalDuration);

	// Do not change initial value on delay
	moveToAnimation->setKeyValueAt(0.0, this->startProportion);
	moveToAnimation->setKeyValueAt(qreal(initialDelay) / totalDuration, this->startProportion);
	moveToAnimation->setKeyValueAt(1.0, endProportion);

	moveToAnimation->start();
	return totalDuration;
}

void LinearLayoutActor::updateStartProportion(qreal startProportion)
{
	setStartProportion(startProportion);
	Q_ASSERT(parentLayoutItem);
	parentLayoutItem->updateLayoutItem();
}
