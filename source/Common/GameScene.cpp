#include "CodeSegment.h"
#include "GameScene.h"
#include "LabelButton.h"
#include "LinearLayout.h"
#include "MainWindow.h"
#include "Prop.h"
#include "Stage.h"
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
	, codeEditorToggle(nullptr)
{
}

GameScene::~GameScene()
{
}

void GameScene::createStandardMenu(const QString& title, bool enableCodeEditorToggle)
{
	// A global layout is required
	if ( this->layout == nullptr ) layout = new LinearLayout(Qt::Vertical);

	// Create an exclusive layout for the standard menu
	LinearLayout* menuLayout = new LinearLayout(Qt::Horizontal);
	this->layout->addLayout(menuLayout, standardMenuProportion);
	const qreal buttonWidthPercent = 0.06;

	// Back button
	this->backButton = new SvgButton(":/button_back.svg", this);
	this->backButton->alignCenter();
	menuLayout->addItem(backButton, buttonWidthPercent);
	connect(backButton, SIGNAL(pressed()), this, SLOT(backButtonPressed()));

	// Game title
	this->gameTitle = new Prop(":/game_title.svg", this);
	this->gameTitle->alignLeft();
	menuLayout->addItem(gameTitle, 0.32);
	menuLayout->addStretch(0.084);

	// Info button
	this->infoButton = new SvgButton(":/button_information.svg", this);
	this->infoButton->alignCenter();
	menuLayout->addItem(infoButton, buttonWidthPercent);
	connect(infoButton, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));

	// Scene title
	this->sceneTitle = new LabelButton(title, this);
	this->sceneTitle->alignLeft();
	menuLayout->addItem(sceneTitle, 0.17);
	connect(sceneTitle, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));
	menuLayout->addStretch(0.084);

	// Player status
//	this->playerStatus = new PlayerStatus(this);
//	menuLayout->addItem(playerStatus, 0.17);
//	connect(playerStatus, SIGNAL(pressed()), this, SLOT(playerStatusPressed()));
	menuLayout->addStretch(0.16 - enableCodeEditorToggle * buttonWidthPercent);

	// Code editor
	if ( enableCodeEditorToggle )
	{
		this->codeEditorToggle = new SvgButton(":/button_code_editor.svg", this);
		this->codeEditorToggle->alignCenter();
		menuLayout->addItem(codeEditorToggle, buttonWidthPercent);
		connect(codeEditorToggle, SIGNAL(pressed()), this, SLOT(codeSegmentTogglePressed()));
	}

	// Config button
	this->configButton = new SvgButton(":/button_config.svg", this);
	this->configButton->alignCenter();
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

void GameScene::codeSegmentTogglePressed()
{
	qDebug() << "Code Editor toggled";
}

void GameScene::rewardsButtonPressed()
{
	qDebug() << "Rewards asked";
}

