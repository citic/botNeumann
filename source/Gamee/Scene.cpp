#include "Layout.h"
#include "Prop.h"
#include "Scene.h"
#include "Stage.h"

#include <QGraphicsRectItem>

Scene::Scene(const QString& sceneName, Stage* stage, QGraphicsItem* parent)
	: QGraphicsRectItem(0.0, 0.0, stage->width(), stage->height(), parent)
	, sceneName(sceneName)
	, stage(stage)
	, layout(nullptr)
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
	background = new Prop(svgElementId, this);
}

void Scene::setLayout(Layout* layout)
{
	if ( layout != this->layout )
		delete this->layout;

	this->layout = layout;
}

void Scene::resize(qreal width, qreal height)
{
	setRect(0.0, 0.0, width, height);

	qreal sw = width / background->boundingRect().width();
	qreal sh = height / background->boundingRect().height();
	background->setTransform(QTransform().scale(sw, sh));

	if ( layout ) layout->resize(0.0, 0.0, width, height);
}
