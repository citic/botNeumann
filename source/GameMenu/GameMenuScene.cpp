#include "BotNeumannApp.h"
#include "GameMenuScene.h"
#include "LinearLayout.h"
#include "LogManager.h"
#include "PlayerManager.h"
#include "PlayerManagerDialog.h"
#include "PlayerStatus.h"
#include "Prop.h"
#include "Stage.h"
#include "SvgButton.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QPushButton>

GameMenuScene::GameMenuScene(Stage* stage, QGraphicsItem* parent)
	: Scene("game_menu", stage, parent)
	, trainingButton(nullptr)
	, missionsButton(nullptr)
	, collaborationButton(nullptr)
	, createButton(nullptr)
{
	LinearLayout* leftLayout = new LinearLayout(Qt::Vertical);
	Prop* gameTitle = new Prop(":/game_title.svg", this);
	gameTitle->setMargins(0.1);
	leftLayout->addItem(gameTitle, 0.2);

	LinearLayout* centralLayout = new LinearLayout(Qt::Horizontal);
	leftLayout->addLayout(centralLayout, 0.8);
	Prop* robot = new Prop(":/game_menu/robot.svg", this);
	robot->setMargins(0.2);
	centralLayout->addItem(robot, 0.5);
	robot->alignCenter();
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
	// A label that shows the nickname of the current player. Press it to change player
	playerStatus = new PlayerStatus(this);
	playerStatus->setMargins(0.2);
	rightLayout->addItem(playerStatus, 1.0 / 6.0);
	connect(playerStatus, SIGNAL(pressed()), this, SLOT(playerControlPressed()));
	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	connect(playerManager, SIGNAL(playerChanged(Player*)), this, SLOT(playerChanged(Player*)));

	// Create the buttons for each game mode and configuration
	QString buttonBackground(":/game_menu/button_background.svg");
	trainingButton = new SvgButton(buttonBackground, this, tr("Training"));
	missionsButton = new SvgButton(buttonBackground, this, tr("Missions"));
	collaborationButton = new SvgButton(buttonBackground, this, tr("Collaboration"));
	createButton = new SvgButton(buttonBackground, this, tr("Create"));

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

	// Disable if there is not an active player
	playerChanged( BotNeumannApp::getInstance()->getCurrentPlayer() );

	// Create the info, rewards and config buttons
	setupConfigButtons(rightLayout);
}

void GameMenuScene::setupConfigButtons(LinearLayout* parentLayout)
{
	SvgButton* infoButton = new SvgButton(":/button_information.svg", this);
	SvgButton* rewardsButton = new SvgButton(":/button_rewards.svg", this);
	SvgButton* configButton = new SvgButton(":/button_config.svg", this);

	LinearLayout* configButtonLayout = new LinearLayout(Qt::Horizontal);
	configButtonLayout->addItem(infoButton, 1.0 / 3.0);
	configButtonLayout->addItem(rewardsButton, 1.0 / 3.0);
	configButtonLayout->addItem(configButton, 1.0 / 3.0);
	parentLayout->addLayout( configButtonLayout, 1.0 / 6.0 );

	infoButton->setMargins(0.15, 0.0, 0.15, 0.4);
	rewardsButton->setMargins(0.15, 0.2, 0.15, 0.2);
	configButton->setMargins(0.15, 0.4, 0.15, 0.0);

	infoButton->alignCenter();
	rewardsButton->alignCenter();
	configButton->alignCenter();

	connect(infoButton, SIGNAL(pressed()), this, SLOT(infoButtonPressed()));
	connect(rewardsButton, SIGNAL(pressed()), this, SLOT(rewardsButtonPressed()));
	connect(configButton, SIGNAL(pressed()), this, SLOT(configButtonPressed()));

	// ToDo: implement these buttons, disabled meanwhile
	infoButton->setEnabled(false);
	rewardsButton->setEnabled(false);
	configButton->setEnabled(false);
}

void GameMenuScene::playerControlPressed()
{
	// Show the PlayerManagerDialog. It will inform the PlayerManager, and the latter emits
	// playerChanged() which is connected with this scene
	PlayerManagerDialog( stage->parentWidget() ).exec();
}

void GameMenuScene::trainingButtonPressed()
{
	emit showUnitSelectionScene("training", true);
}

void GameMenuScene::missionsButtonPressed()
{
	emit showUnitSelectionScene("missions", true);
}

void GameMenuScene::collaborationButtonPressed()
{
	emit showUnitSelectionScene("collaboration", true);
}

void GameMenuScene::createButtonPressed()
{
	emit showUnitSelectionScene("create", true);
}

void GameMenuScene::infoButtonPressed()
{
	qCDebug(logNotImplemented) << "Information asked";
}

void GameMenuScene::rewardsButtonPressed()
{
	qCDebug(logNotImplemented) << "Rewards asked";
}

void GameMenuScene::configButtonPressed()
{
	qCDebug(logNotImplemented) << "Configuration asked";
}

void GameMenuScene::playerChanged(Player* newPlayer)
{
	// Enable the game mode buttons only when there is a actual player
	bool activePlayer = newPlayer != nullptr;
	if (trainingButton) trainingButton->setEnabled(activePlayer);

	// ToDo: these sections are not implemented yet
	activePlayer = false;
	if (missionsButton) missionsButton->setEnabled(activePlayer);
	if (collaborationButton) collaborationButton->setEnabled(activePlayer);
	if (createButton) createButton->setEnabled(activePlayer);
}
