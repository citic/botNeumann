#include "ScenicElement.h"

ScenicElement::ScenicElement(QGraphicsItem* parentItem)
	: QGraphicsSvgItem(parentItem)
{
}

ScenicElement::ScenicElement(const QString& fileName, QGraphicsItem* parentItem)
	: QGraphicsSvgItem(fileName, parentItem)
{
}

void ScenicElement::resize(qreal left, qreal top, qreal width, qreal height)
{
	applyMargins(left, top, width, height);
	setPos(left, top);
	qreal sw = width / boundingRect().width();
	qreal sh = height / boundingRect().height();
	setTransform(QTransform().scale(sw, sh));
}
