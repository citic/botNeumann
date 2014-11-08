#include "Actor.h"

Actor::Actor(QGraphicsItem* parent)
	: QGraphicsSvgItem(parent)
{
}

Actor::Actor(const QString& fileName, QGraphicsItem* parentItem)
	: QGraphicsSvgItem(fileName, parentItem)
{
}
