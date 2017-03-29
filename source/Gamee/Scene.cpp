#include "Scene.h"
#include "Actor.h"
#include "Layout.h"
#include "Stage.h"

#include <QGraphicsRectItem>

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
}

void Scene::setLayout(Layout* layout)
{
	if ( layout != this->layout )
		delete this->layout;

	this->layout = layout;
}

void Scene::resize(qreal width, qreal height)
{
	// This scene is a invisible QGraphicsRectItem, resize it, and its background
	setRect(0.0, 0.0, width, height);
	background->resize(0.0, 0.0, width, height);

	// Resize any child elements
	if ( layout ) layout->resize(0.0, 0.0, width, height);
}
