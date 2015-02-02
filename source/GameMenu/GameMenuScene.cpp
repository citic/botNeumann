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
	robot->setMargins(0.2);
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
//	rightLayout->addItem( createPlayerStatus(), 1.0 / 6.0 );
	rightLayout->addStretch(1.0 / 6.0);

	// Create the buttons for each game mode and configuration
	QString buttonBackground(":/game_menu/game_menu/button_background.svg");
	SvgButton* trainingButton = SvgButton::createLabelButton(tr("Training"), buttonBackground, this);
	SvgButton* missionsButton = SvgButton::createLabelButton(tr("Missions"), buttonBackground, this);
	SvgButton* collaborationButton = SvgButton::createLabelButton(tr("Collaboration"), buttonBackground, this);
	SvgButton* createButton = SvgButton::createLabelButton(tr("Create"), buttonBackground, this);

	// Add the menu buttons to the layout
	rightLayout->addItem( trainingButton, 1.0 / 6.0 );
	rightLayout->addItem( missionsButton, 1.0 / 6.0 );
	rightLayout->addItem( collaborationButton, 1.0 / 6.0 );
	rightLayout->addItem( createButton, 1.0 / 6.0 );

	// React to each button change
	connect(trainingButton, SIGNAL(pressed()), this, SLOT(trainingButtonPressed()));
	connect(missionsButton, SIGNAL(pressed()), this, SLOT(missionsButtonPressed()));
	connect(collaborationButton, SIGNAL(pressed()), this, SLOT(collaborationButtonPressed()));
	connect(createButton, SIGNAL(pressed()), this, SLOT(createButtonPressed()));

	setupConfigButtons(rightLayout);
}

void GameMenuScene::setupConfigButtons(LinearLayout* parentLayout)
{
	SvgButton* infoButton = SvgButton::createImageButton("://button_information.svg", this);
	SvgButton* rewardsButton = SvgButton::createImageButton("://button_rewards.svg", this);
	SvgButton* configButton = SvgButton::createImageButton("://button_config.svg", this);

	LinearLayout* configButtonLayout = new LinearLayout(Qt::Horizontal);
	configButtonLayout->addItem(infoButton, 1.0 / 3.0);
	configButtonLayout->addItem(rewardsButton, 1.0 / 3.0);
	configButtonLayout->addItem(configButton, 1.0 / 3.0);
	parentLayout->addLayout( configButtonLayout, 1.0 / 6.0 );

	infoButton->setMargins(0.15, 0.0, 0.15, 0.4);
	rewardsButton->setMargins(0.15, 0.2, 0.15, 0.2);
	configButton->setMargins(0.15, 0.4, 0.15, 0.0);

	connect(infoButton, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));
	connect(rewardsButton, SIGNAL(pressed()), this, SLOT(rewardsButtonPressed()));
	connect(configButton, SIGNAL(pressed()), this, SLOT(configButtonPressed()));
}

void GameMenuScene::trainingButtonPressed()
{
	emit newSceneAsked(SceneId::training);
}

void GameMenuScene::missionsButtonPressed()
{
	emit newSceneAsked(SceneId::missions);
}

void GameMenuScene::collaborationButtonPressed()
{
	emit newSceneAsked(SceneId::collaboration);
}

void GameMenuScene::createButtonPressed()
{
	emit newSceneAsked(SceneId::create);
}

#include <QDebug>

void GameMenuScene::infoButtonPressed()
{
	qDebug() << "Information asked";
}

void GameMenuScene::rewardsButtonPressed()
{
	qDebug() << "Rewards asked";
}

void GameMenuScene::configButtonPressed()
{
	qDebug() << "Configuration asked";
}
