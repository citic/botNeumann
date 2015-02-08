#include "GameScene.h"
#include "LinearLayout.h"
#include "SvgButton.h"

GameScene::GameScene(const QString& sceneName, Stage* stage, QGraphicsItem* parent)
	: Scene(sceneName, stage, parent)
	, backButton(nullptr)
	, gameTitle(nullptr)
	, infoButton(nullptr)
	, sceneTitle(nullptr)
//	, playerStatus(nullptr)
	, configButton(nullptr)
//	, rewardsButton(nullptr)
{
}

GameScene::~GameScene()
{
}

void GameScene::createStandardMenu(const QString& title)
{
	// A global layout is required
	if ( this->layout == nullptr ) layout = new LinearLayout(Qt::Vertical);

	// Create an exclusive layout for the standard menu
	LinearLayout* menuLayout = new LinearLayout(Qt::Horizontal);
	this->layout->addLayout(menuLayout, 0.09);
	const qreal buttonWidthPercent = 0.06;

	// Back button
	this->backButton = SvgButton::createImageButton(":/button_back.svg", this);
	menuLayout->addItem(backButton, buttonWidthPercent);
	connect(backButton, SIGNAL(pressed()), this, SLOT(backButtonPressed()));

	// Game title
	this->gameTitle = new ScenicElement(":/game_title.svg", this);
	menuLayout->addItem(gameTitle, 0.32);
	menuLayout->addStretch(0.084);

	// Info button
	this->infoButton = SvgButton::createImageButton("://button_information.svg", this);
	menuLayout->addItem(infoButton, buttonWidthPercent);
	connect(infoButton, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));

	// Scene title
	this->sceneTitle = SvgButton::createLabelButton(title, "", this);
	menuLayout->addItem(sceneTitle, 0.17);
	connect(sceneTitle, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));
	menuLayout->addStretch(0.084);

	// Player status
//	this->playerStatus = new PlayerStatus(this);
//	menuLayout->addItem(playerStatus, 0.17);
//	connect(playerStatus, SIGNAL(pressed()), this, SLOT(playerStatusPressed()));
	menuLayout->addStretch(0.17);

	// Config button
	this->configButton = SvgButton::createImageButton("://button_config.svg", this);
	menuLayout->addItem(configButton, buttonWidthPercent);
	connect(configButton, SIGNAL(pressed()), this, SLOT(configButtonPressed()));

	// Rewards button
//	this->rewardsButton = SvgButton::createImageButton("://button_rewards.svg", this);
//	menuLayout->addItem(rewardsButton, buttonWidthPercent);
//	connect(rewardsButton, SIGNAL(pressed()), this, SLOT(rewardsButtonPressed()));
}

#include <QDebug>

void GameScene::infoButtonPressed()
{
	qDebug() << "Information asked";
}

void GameScene::playerStatusPressed()
{
	qDebug() << "Player management dialog not implemented in this version";
}

void GameScene::configButtonPressed()
{
	qDebug() << "Configuration asked";
}

void GameScene::rewardsButtonPressed()
{
	qDebug() << "Rewards asked";
}

