#include "Actor.h"

Actor::Actor(QGraphicsItem* parentItem)
	: ScenicElement(parentItem)
{
}

Actor::Actor(const QString& fileName, QGraphicsItem* parentItem)
	: ScenicElement(fileName, parentItem)
{
}
