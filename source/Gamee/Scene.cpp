#include "Scene.h"
#include "Actor.h"
#include "Layout.h"
#include "Stage.h"

#include <QDebug>
#include <QGraphicsRectItem>
#include <QTime>
#include <QTimer>

Scene::Scene(const QString& sceneName, Stage* stage, QGraphicsItem* parent)
	: QGraphicsRectItem(0.0, 0.0, stage->width(), stage->height(), parent)
	, sceneName(sceneName)
	, stage(stage)
{
	setPen(Qt::NoPen);
	setBrush(Qt::NoBrush);
	stage->getGraphicsScene().addItem(this);
}

Scene::~Scene()
{
	delete layout;
}

void Scene::setBackground(const QString& svgElementId)
{
	background = new Actor(svgElementId, this);

  #if ! GAMIFICATION
	background->setAcceptDrops(true);
  #endif
}

void Scene::setLayout(Layout* layout)
{
	if ( layout != this->layout )
		delete this->layout;

	this->layout = layout;
}

void Scene::resize(qreal width, qreal height)
{
  #ifdef BN_LAYOUT_TIMING
	QTime resizeTime;
	resizeTime.start();
  #endif

	// This scene is a invisible QGraphicsRectItem, resize it, and its background
	setRect(0.0, 0.0, width, height);
	Q_ASSERT(this->background);
	background->resize(0.0, 0.0, width, height);

	// Resize any child elements
	if ( layout ) layout->resize(0.0, 0.0, width, height);

  #ifdef BN_LAYOUT_TIMING
	QTimer::singleShot(0, this, [resizeTime]() { qDebug() << "Scene resize time:" << resizeTime.elapsed() / 1000.0 << "s"; } );
  #endif
}
