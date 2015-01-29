#include "GameMenuScene.h"
#include "LinearLayout.h"
#include "ScenicElement.h"
#include "Stage.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QPushButton>

GameMenuScene::GameMenuScene(Stage* stage, QGraphicsItem* parent)
	: Scene("game_menu", stage, parent)
{
	LinearLayout* leftLayout = new LinearLayout(Qt::Vertical);
	ScenicElement* gameTitle = new ScenicElement(":/game_title.svg", this);
	gameTitle->setMargins(0.1);
	leftLayout->addItem(gameTitle, 0.2);

	LinearLayout* centralLayout = new LinearLayout(Qt::Horizontal);
	leftLayout->addLayout(centralLayout, 0.8);
	ScenicElement* robot = new ScenicElement(":/game_menu/game_menu/robot.svg", this);
	robot->setMargins(0.1);
	centralLayout->addItem(robot, 0.5);
	robot->makeMaintainAspectRatio(true);
	robot->makeFloat(true);
	centralLayout->addStretch(0.5);

	LinearLayout* rightLayout = new LinearLayout(Qt::Vertical);

	LinearLayout* globalLayout1 = new LinearLayout(Qt::Horizontal);
	globalLayout1->addLayout(leftLayout, 2.0 / 3.0);
	globalLayout1->addLayout(rightLayout, 1.0 / 3.0);
	setLayout(globalLayout1);

/*	rightLayout->addItem( createPlayerStatus(), 1.0 / 6.0 );
	rightLayout->addItem( createButton(tr("Training")), 1.0 / 6.0 );
	rightLayout->addItem( createButton(tr("Missions")), 1.0 / 6.0 );
	rightLayout->addItem( createButton(tr("Collaboration")), 1.0 / 6.0 );
	rightLayout->addItem( createButton(tr("Create")), 1.0 / 6.0 );
	rightLayout->addLayout( createConfigButtons(), 1.0 / 6.0 );
*/
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
	QGraphicsScene& graphicsScene = stage->getGraphicsScene();

	trainingButton = new QPushButton(tr("&Training"));
	missionsButton = new QPushButton(tr("&Missions"));
	collaborationButton = new QPushButton(tr("&Collaboration"));
	createButton = new QPushButton(tr("C&reate"));

	QGraphicsLinearLayout* buttonsLayout = new QGraphicsLinearLayout(Qt::Vertical);
	buttonsLayout->addItem(graphicsScene.addWidget(trainingButton));
	buttonsLayout->addItem(graphicsScene.addWidget(missionsButton));
	buttonsLayout->addItem(graphicsScene.addWidget(collaborationButton));
	buttonsLayout->addItem(graphicsScene.addWidget(createButton));

	globalLayout = new QGraphicsLinearLayout(Qt::Horizontal);
	globalLayout->addStretch();
	globalLayout->setItemSpacing(0, stage->width() * 0.72);
	globalLayout->addItem(buttonsLayout);

	graphicsWidget = new QGraphicsWidget();
	graphicsWidget->setLayout(globalLayout);
	graphicsScene.addItem(graphicsWidget);

	connect(trainingButton, SIGNAL(clicked()), this, SLOT(trainingPressed()));
	connect(missionsButton, SIGNAL(clicked()), this, SLOT(missionsPressed()));
	connect(collaborationButton, SIGNAL(clicked()), this, SLOT(collaborationPressed()));
	connect(createButton, SIGNAL(clicked()), this, SLOT(createPressed()));
}

void GameMenuScene::trainingPressed()
{
	emit newSceneAsked(SceneId::training);
}

void GameMenuScene::missionsPressed()
{
	emit newSceneAsked(SceneId::missions);
}

void GameMenuScene::collaborationPressed()
{
	emit newSceneAsked(SceneId::collaboration);
}

void GameMenuScene::createPressed()
{
	emit newSceneAsked(SceneId::create);
}

void GameMenuScene::resize(qreal width, qreal height)
{
	Scene::resize(width, height);
	globalLayout->setItemSpacing(0, width * 0.72);
	graphicsWidget->resize(width, height);
}
