#include "Player.h"
#include "PlayerManager.h"
#include <QSettings>

PlayerManager::PlayerManager(QObject *parent)
	: QObject(parent)
	, currentPlayer(nullptr)
{
}

PlayerManager::~PlayerManager()
{
}

bool PlayerManager::reloadLastPlayer()
{
	Q_ASSERT(currentPlayer == nullptr);
	QSettings settings;
	const QVariant& nickname = settings.value("Players/LastPlayer");
	if ( nickname.isNull() ) return false;
	currentPlayer = new Player(nickname.toString());
	return true;
}

void PlayerManager::saveLastPlayer()
{
	if ( currentPlayer == nullptr ) return;
	QSettings settings;
	settings.setValue("Players/LastPlayer", currentPlayer->getNickname());
}
