#include "Actor.h"

#include <QPropertyAnimation>

Actor::Actor(QGraphicsItem* parentItem)
	: ScenicElement(parentItem)
{
}

Actor::Actor(const QString& fileName, QGraphicsItem* parentItem)
	: ScenicElement(fileName, parentItem)
{
}

void Actor::appear(qreal fromOpacity, qreal toOpacity)
{
	// Animate the robot while it appears
	QPropertyAnimation* robotAppearsAnimation = new QPropertyAnimation(this, "opacity", this);
	// ToDo: Duration must be controlled by the user slider
	robotAppearsAnimation->setDuration(1000);
	robotAppearsAnimation->setStartValue(fromOpacity);
	robotAppearsAnimation->setEndValue(toOpacity);
	robotAppearsAnimation->start();
}
