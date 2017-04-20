#include "LinearLayoutActor.h"
#include "Actor.h"
#include "LabelButton.h"
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
	{
		for ( int index = 0; index < itr.value().size(); ++index )
		{
			Actor* actor = dynamic_cast<Actor*>(itr.value()[index]);
			if ( actor )
			{
				int actorDuration = actor->animateAppear(duration, initialDelay, fromOpacity, toOpacity);
				if ( actorDuration > maxDuration )
					maxDuration = actorDuration;
			}
			else
			{
				// ToDo: Ugly fix: LabelButton should be also animated, but it does not inherit Actor
				LabelButton* label = dynamic_cast<LabelButton*>(itr.value()[index]);
				if ( label )
				{
					int labelDuration = label->animateAppear(duration, initialDelay, fromOpacity, toOpacity);
					if ( labelDuration > maxDuration )
						maxDuration = labelDuration;
				}
			}
		}
	}

	return maxDuration;
}

int LinearLayoutActor::animateMoveTo(qreal endProportion, int duration, int initialDelay)
{
	QPropertyAnimation* animation = createMoveToAnimation("mainStart", endProportion, mainStart, duration, initialDelay);
	Q_ASSERT(animation);
	animation->start();
	return VisualizationSpeed::getInstance().adjust(duration);
}

int LinearLayoutActor::animateMoveToPos(qreal endMainProportion, qreal endCrossProportion, int duration, int initialDelay)
{
	QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup(this);
	groupAnimation->addAnimation( createMoveToAnimation("mainStart", endMainProportion, mainStart, duration, initialDelay) );
	groupAnimation->addAnimation( createMoveToAnimation("crossStart", endCrossProportion, crossStart, duration, initialDelay) );

	groupAnimation->start();
	return VisualizationSpeed::getInstance().adjust(duration);
}

QPropertyAnimation* LinearLayoutActor::createMoveToAnimation(const char* propertyName, qreal endProportion, qreal startProportion, int duration, int initialDelay)
{
	// Adjust animation time
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Create an animation and set its duration
	QPropertyAnimation* animation = new QPropertyAnimation(this, propertyName, this);
	animation->setDuration(totalDuration);

	// Stay at the current proportion at the beginning of the animation
	animation->setKeyValueAt(0.0, startProportion);

	// After the delay is finished, start from the current porportion
	if ( totalDuration > 0 )
		animation->setKeyValueAt(qreal(initialDelay) / totalDuration, startProportion);

	// At the end of the animation, reach the desired end proportion
	animation->setKeyValueAt(1.0, endProportion);
	return animation;
}

int LinearLayoutActor::animatePaddingIncrease(qreal topIncrease, qreal rightIncrease, qreal bottomIncrease, qreal leftIncrease, int duration, int initialDelay)
{
	QParallelAnimationGroup* groupAnimation = new QParallelAnimationGroup(this);
	groupAnimation->addAnimation( createPaddingAnimation("paddingTop", topIncrease, getPaddingTop(), duration, initialDelay) );
	groupAnimation->addAnimation( createPaddingAnimation("paddingRight", rightIncrease, getPaddingRight(), duration, initialDelay) );
	groupAnimation->addAnimation( createPaddingAnimation("paddingBottom", bottomIncrease, getPaddingBottom(), duration, initialDelay) );
	groupAnimation->addAnimation( createPaddingAnimation("paddingLeft", leftIncrease, getPaddingLeft(), duration, initialDelay) );

	groupAnimation->start();
	return VisualizationSpeed::getInstance().adjust(duration);
}

QPropertyAnimation* LinearLayoutActor::createPaddingAnimation(const char* propertyName, qreal increaseFactor, qreal initialValue, int duration, int initialDelay)
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
