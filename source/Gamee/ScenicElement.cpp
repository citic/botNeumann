#include "ScenicElement.h"

ScenicElement::ScenicElement(QGraphicsItem* parentItem)
	: QGraphicsSvgItem(parentItem)
{
}

ScenicElement::ScenicElement(const QString& fileName, QGraphicsItem* parentItem)
	: QGraphicsSvgItem(fileName, parentItem)
{
}
