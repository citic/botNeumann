#include "BotNeumannApp.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PlayerStatus.h"

PlayerStatus::PlayerStatus(QGraphicsItem* parentItem)
	: LabelButton("", parentItem)
{
	BotNeumannApp* app = static_cast<BotNeumannApp*>(qApp);
	connect(app->getPlayerManager(), SIGNAL(playerChanged(Player*)), this, SLOT(playerChanged(Player*)));
	playerChanged( app->getCurrentPlayer() );
}

PlayerStatus::~PlayerStatus()
{
}

void PlayerStatus::playerChanged(Player* newPlayer)
{
	const QString& nickname = newPlayer ? newPlayer->getNickname() : tr("NoPlayer");
	setText(nickname);
}
