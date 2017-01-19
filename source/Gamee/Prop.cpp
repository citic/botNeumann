#include "Prop.h"

Prop::Prop(SceneId sceneId, const QString& svgElementId, QGraphicsItem* parentItem)
	: ScenicElement(sceneId, svgElementId, parentItem)
{
}

Prop::Prop(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: ScenicElement(prefixedSvgElementId, parentItem)
{
}

Prop::~Prop()
{
}
