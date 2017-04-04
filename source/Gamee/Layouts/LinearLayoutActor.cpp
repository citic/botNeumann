#include "LinearLayoutActor.h"
#include "Actor.h"
#include "VisualizationSpeed.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

LinearLayoutActor::LinearLayoutActor(Qt::Orientation orientation)
	: LinearLayout(orientation)
{
}

int LinearLayoutActor::animateAppear(int duration, int initialDelay, qreal fromOpacity, qreal toOpacity)
{
	// The duration of the animation is the max duration of children
	int maxDuration = 0;

	// Animate all children appearing at the same time (concurrently)
	for (ItemsType::iterator itr = items.begin(); itr != items.end(); ++itr )
		for ( int index = 0; index < itr.value().size(); ++index )
		{
			Actor* actor = dynamic_cast<Actor*>(itr.value()[index]);
			if ( actor )
			{
				int actorDuration = actor->animateAppear(duration, initialDelay, fromOpacity, toOpacity);
				if ( actorDuration > maxDuration )
					maxDuration = actorDuration;
			}
		}

	return maxDuration;
}

int LinearLayoutActor::animateMoveTo(qreal endProportion, int duration, int initialDelay)
{
	// Adjust animation time
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Create an animation and set its duration
	QPropertyAnimation* moveToAnimation = new QPropertyAnimation(this, "startProportion", this);
	moveToAnimation->setDuration(totalDuration);

	// Do not change initial value on delay
	moveToAnimation->setKeyValueAt(0.0, this->startProportion);
	if ( totalDuration > 0 )
		moveToAnimation->setKeyValueAt(qreal(initialDelay) / totalDuration, this->startProportion);
	moveToAnimation->setKeyValueAt(1.0, endProportion);

	moveToAnimation->start();
	return duration;
}

int LinearLayoutActor::animateMarginIncrease(qreal topIncrease, qreal rightIncrease, qreal bottomIncrease, qreal leftIncrease, int duration, int initialDelay)
{
	QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup(this);
	groupAnimation->addAnimation( createMarginAnimation("marginTop", topIncrease, getMarginTop(), duration, initialDelay) );
	groupAnimation->addAnimation( createMarginAnimation("marginRight", rightIncrease, getMarginRight(), duration, initialDelay) );
	groupAnimation->addAnimation( createMarginAnimation("marginBottom", bottomIncrease, getMarginBottom(), duration, initialDelay) );
	groupAnimation->addAnimation( createMarginAnimation("marginLeft", leftIncrease, getMarginLeft(), duration, initialDelay) );

	groupAnimation->start();
	return VisualizationSpeed::getInstance().adjust(duration);
}

QPropertyAnimation* LinearLayoutActor::createMarginAnimation(const char* propertyName, qreal increaseFactor, qreal initialValue, int duration, int initialDelay)
{
	// Adjust animation time
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Create an animation and set its duration
	QPropertyAnimation* animation = new QPropertyAnimation(this, propertyName, this);
	animation->setDuration(totalDuration);

	// Do not change initial value on delay
	animation->setKeyValueAt(0.0, initialValue);
	if ( totalDuration > 0 )
		animation->setKeyValueAt(qreal(initialDelay) / totalDuration, initialValue);
	animation->setKeyValueAt(1.0, initialValue + increaseFactor);

	return animation;
}

void LinearLayoutActor::updateParentLayoutItem()
{
	Q_ASSERT(parentLayoutItem);
	parentLayoutItem->updateLayoutItem();
}
