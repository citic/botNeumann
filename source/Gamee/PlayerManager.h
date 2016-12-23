#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <QList>
#include <QObject>

class Player;

/** Manages the list of players on this device or network players */
class PlayerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PlayerManager)

  protected:
	/// The list of players stored on this device
	QList<Player*> players;
	/// Current active player, who is playing on this device, null if no players have been
	/// registered yet.
	Player* currentPlayer;

  public:
	/// Constructor
	explicit PlayerManager(QObject *parent = nullptr);
	/// Destructor
	virtual ~PlayerManager();
	/// Load the list of all players on this device
	/// @return The count of players loaded
	int loadPlayers();
	/// @return The number of players stored in this device
	/// @remarks Call @a loadPlayers() before calling this method
	inline int getPlayerCount() const { return players.count(); }
	/// Get access to the i-th player in read-only mode
	/// @remarks No index out of bounds verification is done
	inline const Player* getPlayerAt(int index) const { return players[index]; }
	/// Get access to the i-th player in write mode
	/// @remarks No index out of bounds verification is done
	inline Player* getPlayerAt(int index) { return players[index]; }
	/// Removes all player objects from the @a players list
	void clearPlayers();

  public:
	/// Loads the configuration of the last player who played in the previous session of this game
	/// @return true on success, false if there is not previous player stored in settings
	bool reloadLastPlayer();
	/// Stores the nickname of the current player in settings
	/// This method is automatically called by destructor
	void saveLastPlayer();
	/// Get access to the current active player on this device
	inline Player* getCurrentPlayer() const { return currentPlayer; }
	/// Set the current active player
	/// @return A reference to the new player object, nullptr on error
	Player* setCurrentPlayer(Player* player);
	/// Set the current active player by its unique id
	/// @return A reference to the new player object, nullptr on error
	Player* setCurrentPlayer(const QByteArray& playerId);
	/// Creates a new player
	/// @param nickname Identifier of the player
	/// @return A pointer to the created player, nullptr on error
	Player* createPlayer(const QString& nickname);
	/// Imports player data from the network, i.e: sign in a network player on this device
	Player* retrievePlayer(const QString& nickname);
	/// Removers a player
	/// @param network If false the player is removed from this device only. If true the player is
	/// removed from both, this device and the network
	/// @return true on success, false otherwise
	bool removePlayer(const QString& nickname, bool network);
	/// Syncronize data of the current player between this device and the network database
	bool syncCurrentPlayer();

  signals:
	/// Emitted when the player is changed
	void playerChanged(Player* currentPlayer);
};

#endif // PLAYERMANAGER_H
