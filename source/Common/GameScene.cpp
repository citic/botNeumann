#include "CodeSegment.h"
#include "Actor.h"
#include "GameScene.h"
#include "LabelButton.h"
#include "LinearLayout.h"
#include "LogManager.h"
#include "MainWindow.h"
#include "Stage.h"
#include "SvgButton.h"

GameScene::GameScene(const QString& sceneName, Stage* stage, QGraphicsItem* parent)
	: Scene(sceneName, stage, parent)
{
}

GameScene::~GameScene()
{
}

void GameScene::createStandardMenu(const QString& title, bool enableCodeEditorToggle, double verticalProportion)
{
	// A global layout is required
	if ( this->layout == nullptr )
		this->layout = new LinearLayout(Qt::Vertical);

	// Create an exclusive layout for the standard menu
	LinearLayout* menuLayout = new LinearLayout(Qt::Horizontal);
	this->layout->addLayout(menuLayout, verticalProportion);
	const qreal buttonWidthPercent = 0.06;

	// Back button
  #ifdef BN_NOGAMIFICATION
	menuLayout->addStretch(buttonWidthPercent);
  #else
	this->backButton = new SvgButton("ge_button_back", this);
	this->backButton->alignCenter();
	menuLayout->addItem(backButton, buttonWidthPercent);
	connect(backButton, SIGNAL(pressed()), this, SLOT(backButtonPressed()));
  #endif

	// Game title
	this->gameTitle = new Actor("ge_game_title", this);
	this->gameTitle->setPaddings(0.1, 0.0);
	this->gameTitle->alignLeft();
	menuLayout->addItem(gameTitle, 0.32);
	menuLayout->addStretch(0.084);

	// Info button
	this->infoButton = new SvgButton("ge_button_information", this);
	this->infoButton->alignCenter();
	menuLayout->addItem(infoButton, buttonWidthPercent);
	connect(infoButton, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));

	// Scene title
	this->sceneTitle = new LabelButton(title, this);
	this->sceneTitle->setPaddings(0.15, 0.0, 0.05);
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
		this->codeEditorToggle = new SvgButton("ge_button_code_editor", this);
		this->codeEditorToggle->alignCenter();
		menuLayout->addItem(codeEditorToggle, buttonWidthPercent);
		connect(codeEditorToggle, SIGNAL(pressed()), this, SLOT(codeSegmentTogglePressed()));
	}

	// Config button
	this->configButton = new SvgButton("ge_button_config", this);
	this->configButton->alignCenter();
	menuLayout->addItem(configButton, buttonWidthPercent);
	connect(configButton, SIGNAL(pressed()), this, SLOT(configButtonPressed()));

	// Rewards button
//	this->rewardsButton = SvgButton::createImageButton("ge_button_rewards", this);
//	menuLayout->addItem(rewardsButton, buttonWidthPercent);
//	connect(rewardsButton, SIGNAL(pressed()), this, SLOT(rewardsButtonPressed()));
}

void GameScene::infoButtonPressed()
{
	qCDebug(logNotImplemented) << "Information asked";
}

void GameScene::playerStatusPressed()
{
	qCDebug(logNotImplemented) << "Player management dialog not implemented in this version";
}

void GameScene::configButtonPressed()
{
	qCDebug(logNotImplemented) << "Configuration asked";
}

void GameScene::codeSegmentTogglePressed()
{
	qCDebug(logNotImplemented) << "Code Editor toggled";
}

void GameScene::rewardsButtonPressed()
{
	qCDebug(logNotImplemented) << "Rewards asked";
}

