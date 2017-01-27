#include "Actor.h"
#include "VisualizationSpeed.h"

#include <QPropertyAnimation>

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

int Actor::transitionFaces(const QStringList& faces, QTimeLine::Direction direction, int duration)
{
	setFaces(faces);
	return transitionFaces(direction, duration);
}

int Actor::transitionFaces(QTimeLine::Direction direction, int duration)
{
	// ElementId is already a property in QGraphicsSvgItem. It will be used to transition
	// to another graphic in steps changing the face
	Q_ASSERT(faces.count() >= 3);

	// Adjust duration according to the user preference set on the speed slider
	duration = VisualizationSpeed::getInstance().adjust(duration);

	// A timeline will call at regular intervals a function
	timeLine.setDuration(duration);
	timeLine.setDirection(direction);
	timeLine.setCurveShape( QTimeLine::LinearCurve );

	// Change the face at regular intervals of time, calling a method for doing this job
	timeLine.setFrameRange(0, faces.count() - 1);
	connect( &timeLine, SIGNAL(frameChanged(int)), this, SLOT(changeFace(int)) );

	timeLine.start();
	return duration;
}

void Actor::changeFace(int frame)
{
	Q_ASSERT( frame < faces.count() );
	setElementId( faces[frame] );
}
