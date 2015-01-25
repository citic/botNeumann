#include "Scene.h"
#include "Stage.h"
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>

Scene::Scene(const QString& sceneName, Stage* stage, QGraphicsItem* parent)
	: QGraphicsRectItem(0.0f, 0.0f, stage->width(), stage->height(), parent)
	, sceneName(sceneName)
	, stage(stage)
{
	setPen(Qt::NoPen);
	setBrush(Qt::NoBrush);
	setBackground();
	stage->getGraphicsScene().addItem(this);
}

Scene::~Scene()
{
}

void Scene::setBackground(const QString& filename)
{
	background = new QGraphicsSvgItem(getResourcePathFor(filename), this);
}

QString Scene::getResourcePathFor(const QString& assertName) const
{
	return ":/" + sceneName + '/' + sceneName + '/' + assertName;
}

void Scene::resize(qreal width, qreal height)
{
	setRect(0, 0, width, height);

	qreal sw = width / background->boundingRect().width();
	qreal sh = height / background->boundingRect().height();
	background->setTransform(QTransform().scale(sw, sh));
}
