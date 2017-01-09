#include "VisualizationSpeed.h"

VisualizationSpeed::VisualizationSpeed(QObject *parent)
	: QObject(parent)
{
}

VisualizationSpeed& VisualizationSpeed::getInstance()
{
	static VisualizationSpeed visualizationSpeed;
	return visualizationSpeed;
}

void VisualizationSpeed::updateSpeed(int userValue)
{
	Q_ASSERT(userValue >= 0 && userValue <= maxSpeedFactor);
	this->speedFactor = (maxSpeedFactor - userValue) / 100.0;
	emit speedChanged(speedFactor);
}
