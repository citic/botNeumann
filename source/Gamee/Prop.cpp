#include "Prop.h"

Prop::Prop(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: Actor(prefixedSvgElementId, parentItem)
{
}

Prop::~Prop()
{
}
