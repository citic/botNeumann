#include "BotNeumannApp.h"
#include "Player.h"
#include "PlayerStatus.h"

PlayerStatus::PlayerStatus(QGraphicsItem* parentItem)
	: LabelButton("", parentItem)
{
	playerChanged( static_cast<BotNeumannApp*>(qApp)->getCurrentPlayer() );
}

PlayerStatus::~PlayerStatus()
{
}

void PlayerStatus::playerChanged(Player* newPlayer)
{
	const QString& nickname = newPlayer ? newPlayer->getNickname() : tr("NoPlayer");
	setText(nickname);
}
