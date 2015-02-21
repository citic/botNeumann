#include "Prop.h"

Prop::Prop(QGraphicsItem* parentItem)
	: ScenicElement(parentItem)
{
}

Prop::Prop(const QString& filename, QGraphicsItem* parentItem)
: ScenicElement(filename, parentItem)
{
}

Prop::~Prop()
{
}
