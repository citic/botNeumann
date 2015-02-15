#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <QObject>

class Player;

/** Manages the list of players on this device or network players */
class PlayerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PlayerManager)

  protected:
	/// Current active player, who is playing on this device, null if no players have been
	/// registered yet
	Player* currentPlayer;

  public:
	/// Constructor
	explicit PlayerManager(QObject *parent = nullptr);
	/// Destructor
	~PlayerManager();
	/// Loads the configuration of the last player who played in the previous session of this game
	/// @return true on success, false if there is not previous player stored in settings
	bool reloadLastPlayer();
	/// Stores the nickname of the current player in settings
	/// This method is automatically called by destructor
	void saveLastPlayer();
	/// Get access to the current active player on this device
	inline Player* getCurrentPlayer() const { return currentPlayer; }
	/// Creates a new player
	/// @param nickname Identifier of the player
	/// @param network If true @a nickname must be unique on the world and player will be added to
	/// the player network. If false, player will be created locally only
	/// @return A pointer to the created player, nullptr on error
	Player* createPlayer(const QString& nickname, bool network);
	/// Imports player data from the network, i.e: sign in a network player on this device
	Player* retrievePlayer(const QString& nickname);
	/// Removers a player
	/// @param network If false the player is removed from this device only. If true the player is
	/// removed from both, this device and the network
	/// @return true on success, false otherwise
	bool removePlayer(const QString& nickname, bool network);
	/// Syncronize data of the current player between this device and the network database
	bool syncCurrentPlayer();
};

#endif // PLAYERMANAGER_H
