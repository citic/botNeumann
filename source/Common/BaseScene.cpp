#include "BaseScene.h"
#include <QtDebug>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>

BaseScene::BaseScene(QObject* parent, const QString& sceneName, qreal width, qreal height)
	: QGraphicsScene(parent)
	, sceneName(sceneName)
{
	canvas = addRect(0, 0, width, height);
	canvas->setPen(Qt::NoPen);
	canvas->setBrush(Qt::NoBrush);
	setBackground();
}

BaseScene::~BaseScene()
{
	qDebug() << sceneName << "scene deleted";
}

void BaseScene::setBackground(const QString& filename)
{
	const QString& resource = ":/" + sceneName + '/' + sceneName + '/' + filename;
	background = new QGraphicsSvgItem(resource, canvas);
	//scene.addItem(background);
}
