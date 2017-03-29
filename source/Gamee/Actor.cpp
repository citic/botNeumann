#include "Actor.h"
#include "Layout.h"
#include "SvgRendererManager.h"
#include "VisualizationSpeed.h"

#include <QGraphicsScene>
#include <QPropertyAnimation>

// Construction -----------------------------------------------------------------------------------

Actor::Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: QGraphicsSvgItem(parentItem)
{
	if ( ! prefixedSvgElementId.isEmpty() )
	{
		setRenderer( mapSceneIdFromPrefix(prefixedSvgElementId) );
		setElementId(prefixedSvgElementId);
	}
}

Actor::Actor(const QString& svgFilename, QGraphicsItem* parentItem, bool loadFile)
	: QGraphicsSvgItem(svgFilename, parentItem)
{
	Q_UNUSED(loadFile);
}

Actor::~Actor()
{
	delete layout;
}


// Layout and graphic item ------------------------------------------------------------------------

void Actor::removeFromScene()
{
	scene()->removeItem(this);
	this->deleteLater();
}

void Actor::setRenderer(SceneId sceneId)
{
	setSharedRenderer( SvgRendererManager::findRenderer(sceneId) );
}

SceneId Actor::mapSceneIdFromPrefix(const QString& prefixedText)
{
	if ( prefixedText.startsWith("up_") ) return sceneUnitPlaying;
	if ( prefixedText.startsWith("ge_") ) return sceneGeneral;
	if ( prefixedText.startsWith("gm_") ) return sceneGameMenu;
	if ( prefixedText.startsWith("us_") ) return sceneUnitSelection;

	return sceneGeneral;
}


void Actor::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Resize children before changing the values (left, top, width, height)
	if ( layout ) layout->resize(left, top, width, height);

	// Update the LayoutItem part of this object
	LayoutItem::resize(left, top, width, height);
	applyMargins(left, top, width, height);

	// Resize the QGraphicaItem part of this object
	resizeItem(this, left, top, width, height);
}


// Animations ------------------------------------------------------------------------------------

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
	if ( totalDuration > 0 )
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
