#include "GameMenuScene.h"
#include "ScenicElement.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QPushButton>

GameMenuScene::GameMenuScene(QObject* parent, qreal width, qreal height)
	: Scene(parent, "game_menu", width, height)
{
	ScenicElement* gameTitle = new ScenicElement(":/game_title.svg", canvas);
	gameTitle->setPos(25.0, 50.0);

	setupButtons();
}

GameMenuScene::~GameMenuScene()
{
}

#include <QApplication>

void GameMenuScene::setupButtons()
{
	// Default stylesheet for buttons
	QString styleSheet("QPushButton { color: rgb(25, 212, 207); background-color: rgba(64, 144, 144, 50); font-size: 20px; padding: 15px; border-radius: 15px; border: solid 1px; font: 40px \"Tenby Five\"; }");
	qApp->setStyleSheet(styleSheet);

	trainingButton = new QPushButton(tr("&Training"));
	missionsButton = new QPushButton(tr("&Missions"));
	collaborationButton = new QPushButton(tr("&Collaboration"));
	createButton = new QPushButton(tr("C&reate"));

	QGraphicsLinearLayout* buttonsLayout = new QGraphicsLinearLayout(Qt::Vertical);
	buttonsLayout->addItem(addWidget(trainingButton));
	buttonsLayout->addItem(addWidget(missionsButton));
	buttonsLayout->addItem(addWidget(collaborationButton));
	buttonsLayout->addItem(addWidget(createButton));

	globalLayout = new QGraphicsLinearLayout(Qt::Horizontal);
	globalLayout->addStretch();
	globalLayout->setItemSpacing(0, width() * 0.72);
	globalLayout->addItem(buttonsLayout);

	graphicsWidget = new QGraphicsWidget();
	graphicsWidget->setLayout(globalLayout);
	addItem(graphicsWidget);

	connect(trainingButton, SIGNAL(clicked()), this, SLOT(trainingPressed()));
	connect(missionsButton, SIGNAL(clicked()), this, SLOT(missionsPressed()));
	connect(collaborationButton, SIGNAL(clicked()), this, SLOT(collaborationPressed()));
	connect(createButton, SIGNAL(clicked()), this, SLOT(createPressed()));
}

#include <QDebug>

void GameMenuScene::trainingPressed()
{
	qDebug() << "Training: not implemented yet!";
}

void GameMenuScene::missionsPressed()
{
	qDebug() << "Missions: not implemented yet";
}

void GameMenuScene::collaborationPressed()
{
	qDebug() << "Collaboration: not implemented yet";
}

void GameMenuScene::createPressed()
{
	qDebug() << "Create: not implemented yet";
}

void GameMenuScene::resize(qreal width, qreal height)
{
	Scene::resize(width, height);
	globalLayout->setItemSpacing(0, width * 0.72);
	graphicsWidget->resize(width, height);
}
