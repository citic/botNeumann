#include "Actor.h"
#include "VisualizationSpeed.h"

#include <QPropertyAnimation>

Actor::Actor(QGraphicsItem* parentItem)
	: ScenicElement(parentItem)
{
}

Actor::Actor(const QString& fileName, QGraphicsItem* parentItem)
	: ScenicElement(fileName, parentItem)
{
}

int Actor::appear(int duration, qreal fromOpacity, qreal toOpacity)
{
	// Animate the robot while it appears
	QPropertyAnimation* robotAppearsAnimation = new QPropertyAnimation(this, "opacity", this);
	duration = VisualizationSpeed::getInstance().adjust(duration);
	robotAppearsAnimation->setDuration(duration);
	robotAppearsAnimation->setStartValue(fromOpacity);
	robotAppearsAnimation->setEndValue(toOpacity);
	robotAppearsAnimation->start();
	return duration;
}
