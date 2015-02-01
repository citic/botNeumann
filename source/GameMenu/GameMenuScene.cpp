#include "GameMenuScene.h"
#include "LinearLayout.h"
#include "ScenicElement.h"
#include "Stage.h"
#include "SvgButton.h"
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
	setupButtons(rightLayout);

	LinearLayout* globalLayout = new LinearLayout(Qt::Horizontal);
	globalLayout->addLayout(leftLayout, 0.7);
	globalLayout->addLayout(rightLayout, 0.3);
	setLayout(globalLayout);
}

GameMenuScene::~GameMenuScene()
{
}

void GameMenuScene::setupButtons(LinearLayout* rightLayout)
{
	trainingButton = SvgButton::createLabelButton(tr("Training"), ":/game_menu/game_menu/button_background.svg", this);
	missionsButton = SvgButton::createLabelButton(tr("Missions"), ":/game_menu/game_menu/button_background.svg", this);
	collaborationButton = SvgButton::createLabelButton(tr("Collaboration"), ":/game_menu/game_menu/button_background.svg", this);
	createButton = SvgButton::createLabelButton(tr("Create"), ":/game_menu/game_menu/button_background.svg", this);

//	rightLayout->addItem( createPlayerStatus(), 1.0 / 6.0 );
	rightLayout->addItem( trainingButton, 1.0 / 6.0 );
	rightLayout->addItem( missionsButton, 1.0 / 6.0 );
	rightLayout->addItem( collaborationButton, 1.0 / 6.0 );
	rightLayout->addItem( createButton, 1.0 / 6.0 );
//	rightLayout->addLayout( createConfigButtons(), 1.0 / 6.0 );

//	QString styleSheet("QPushButton { color: rgb(25, 212, 207); background-color: rgba(64, 144, 144, 50); font-size: 20px; padding: 15px; border-radius: 15px; border: solid 1px; font: 40px \"Tenby Five\"; }");
	connect(trainingButton, SIGNAL(pressed()), this, SLOT(trainingPressed()));
	connect(missionsButton, SIGNAL(pressed()), this, SLOT(missionsPressed()));
	connect(collaborationButton, SIGNAL(pressed()), this, SLOT(collaborationPressed()));
	connect(createButton, SIGNAL(pressed()), this, SLOT(createPressed()));
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
