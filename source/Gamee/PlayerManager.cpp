#include "PlayerManager.h"
#include "Common.h"
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
	settings.beginGroup(sk("Players"));

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
	const QVariant& playerId = settings.value(sk("Players/LastPlayer"));
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
	QSettings settings;
	const QByteArray& playerId = currentPlayer ? currentPlayer->getId() : QByteArray();
	settings.setValue(sk("Players/LastPlayer"), QVariant(playerId));
	if ( currentPlayer) currentPlayer->save();
}

Player* PlayerManager::setCurrentPlayer(Player* player)
{
	if ( currentPlayer && currentPlayer != player )
		currentPlayer->deleteLater();
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
	setCurrentPlayer(player);
	loadPlayers();
	return player;
}

bool PlayerManager::removePlayer(const QByteArray& playerId)
{
	if ( currentPlayer && currentPlayer->getId() == playerId )
	{
		currentPlayer->remove();
		setCurrentPlayer(nullptr);
	}
	else
	{
		Player* player = findPlayerById(playerId);
		if ( player )
			player->remove();
	}
	loadPlayers();
	return true;
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
