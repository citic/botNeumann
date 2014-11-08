#include "Scene.h"
#include <QtDebug>
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
	qDebug() << sceneName << "scene deleted";
}

void Scene::setBackground(const QString& filename)
{
	const QString& resource = ":/" + sceneName + '/' + sceneName + '/' + filename;
	background = new QGraphicsSvgItem(resource, canvas);
	//background->boundingRect().setSize(QSizeF(width, height));
	//scene.addItem(background);
}

void Scene::resize(qreal width, qreal height)
{
	// Update canvas and background
	setSceneRect(0, 0, width - 1, height - 1);
	canvas->setRect(0, 0, width - 1, height - 1);

	qreal sw = width / background->boundingRect().width();
	qreal sh = height / background->boundingRect().height();
	background->setTransform(QTransform().scale(sw, sh));
}
