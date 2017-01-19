#include "Actor.h"
#include "VisualizationSpeed.h"

#include <QPropertyAnimation>

Actor::Actor(SceneId sceneId, const QString& svgElementId, QGraphicsItem* parentItem)
	: ScenicElement(sceneId, svgElementId, parentItem)
{
}

Actor::Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: ScenicElement(prefixedSvgElementId, parentItem)
{
}

int Actor::appear(int duration, qreal fromOpacity, qreal toOpacity)
{
	updateLayoutItem();

	// Animate the robot while it appears
	QPropertyAnimation* robotAppearsAnimation = new QPropertyAnimation(this, "opacity", this);
	duration = VisualizationSpeed::getInstance().adjust(duration);
	robotAppearsAnimation->setDuration(duration);
	robotAppearsAnimation->setStartValue(fromOpacity);
	robotAppearsAnimation->setEndValue(toOpacity);
	robotAppearsAnimation->start();
	return duration;
}
