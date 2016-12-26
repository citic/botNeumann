#include "PlayerManager.h"
#include "Player.h"
#include <QSettings>

PlayerManager::PlayerManager(QObject *parent)
	: QObject(parent)
	, currentPlayer(nullptr)
{
}

PlayerManager::~PlayerManager()
{
	clearPlayers();
	delete currentPlayer;
}

int PlayerManager::loadPlayers()
{
	// Avoid concatenate with old previous
	clearPlayers();

	// Players are stored in settings on this device
	QSettings settings;
	settings.beginGroup("Players");

	// There is a subfolder for each player
	foreach ( const QVariant& id, settings.childGroups() )
	{
		Player* player = new Player( id.toByteArray() );
		player->load();
		players.append(player);
	}
	return players.count();
}

Player* PlayerManager::findPlayerById(const QByteArray& id)
{
	for ( int index = 0; index < players.count(); ++index )
		if ( players[index]->getId() == id )
			return players[index];

	return nullptr;
}

void PlayerManager::clearPlayers()
{
	for ( int index = 0; index < players.count(); ++index )
		delete players[index];
	players.clear();
}

bool PlayerManager::reloadLastPlayer()
{
	// Get the last player id from the settings
	QSettings settings;
	const QVariant& playerId = settings.value("Players/LastPlayer");
	if ( playerId.isNull() ) return false;
	Q_ASSERT(currentPlayer == nullptr);

	// Create a Player object and load it from settings using the ID
	currentPlayer = new Player(playerId.toByteArray());
	currentPlayer->load();

	// Done
	emit playerChanged(currentPlayer);
	return true;
}

void PlayerManager::saveLastPlayer()
{
	if ( currentPlayer == nullptr ) return;
	QSettings settings;
	settings.setValue("Players/LastPlayer", QVariant(currentPlayer->getId()));
	currentPlayer->save();
}

Player* PlayerManager::setCurrentPlayer(Player* player)
{
	Q_ASSERT(player);
	if ( currentPlayer && currentPlayer != player ) currentPlayer->deleteLater();
	currentPlayer = player;
	saveLastPlayer();
	emit playerChanged(currentPlayer);
	return currentPlayer;
}

Player* PlayerManager::setCurrentPlayer(const QByteArray& playerId)
{
	Q_ASSERT(playerId.isEmpty() == false);
	Player* player = new Player(playerId);
	Q_ASSERT(player);
	player->load();
	return setCurrentPlayer(player);
}

Player* PlayerManager::createPlayer(const QString& nickname)
{
	Player* player = new Player("", nickname);
	player->autogenerateId();
	qDebug("Player %s created with id %s", qPrintable(nickname), qPrintable(player->getId()));
	return setCurrentPlayer(player);
}

bool PlayerManager::renamePlayer(const QByteArray& playerId, const QString& newNickname)
{
	if ( currentPlayer && currentPlayer->getId() == playerId )
	{
		currentPlayer->setNickname(newNickname);
		currentPlayer->save();
		emit playerChanged(currentPlayer);
	}
	else
	{
		Player* player = findPlayerById(playerId);
		if ( player )
		{
			player->setNickname(newNickname);
			player->save();
		}
	}
	return true;
}
