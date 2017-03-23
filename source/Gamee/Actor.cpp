#include "Actor.h"
#include "VisualizationSpeed.h"

#include <QPropertyAnimation>

Actor::Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: ScenicElement(prefixedSvgElementId, parentItem)
{
}

int Actor::appear(int duration, qreal fromOpacity, qreal toOpacity, int initialDelay)
{
	updateLayoutItem();

	// Adjust animation time
	initialDelay = VisualizationSpeed::getInstance().adjust(initialDelay);
	duration = VisualizationSpeed::getInstance().adjust(duration);
	int totalDuration = initialDelay + duration;

	// Animate the robot while it appears
	QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity", this);
	animation->setDuration(duration);

	// Do not change initial value on delay
	animation->setKeyValueAt(0.0, fromOpacity);
	animation->setKeyValueAt(initialDelay / totalDuration, fromOpacity);
	animation->setKeyValueAt(1.0, toOpacity);

	animation->start();
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
