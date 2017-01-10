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
	Q_ASSERT(userValue >= 0 && userValue <= maxDelayFactor);
	this->delayFactor = (maxDelayFactor - userValue) / 100.0;
	emit speedChanged( getDelayFactor() );
}

void VisualizationSpeed::setSeeking(bool seeking)
{
	if ( this->seeking != seeking )
	{
		this->seeking = seeking;
		emit speedChanged( getDelayFactor() );
	}
}
