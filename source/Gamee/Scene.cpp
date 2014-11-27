#include "Scene.h"
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>

Scene::Scene(QObject* parent, const QString& sceneName, qreal width, qreal height)
	: QGraphicsScene(parent)
	, sceneName(sceneName)
{
	canvas = addRect(0, 0, width, height);
	canvas->setPen(Qt::NoPen);
	canvas->setBrush(Qt::NoBrush);
	setBackground();
}

Scene::~Scene()
{
}

void Scene::setBackground(const QString& filename)
{
	background = new QGraphicsSvgItem(getResourcePathFor(filename), canvas);
}

QString Scene::getResourcePathFor(const QString& assertName) const
{
	return ":/" + sceneName + '/' + sceneName + '/' + assertName;
}

void Scene::resize(qreal width, qreal height)
{
	// Update canvas and background
	setSceneRect(0, 0, width, height);
	canvas->setRect(0, 0, width, height);

	qreal sw = width / background->boundingRect().width();
	qreal sh = height / background->boundingRect().height();
	background->setTransform(QTransform().scale(sw, sh));
}
