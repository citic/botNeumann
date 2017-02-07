#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class QDir;

class Player : public QObject
{
	Q_OBJECT
	// Default C++ copy methods are fine

  protected:
	/// Unique identifier of this player on the world
	QByteArray id;
	/// User given name of the player. It can be renamed later while keeping the id
	QString nickname;

  public:
	/// Constructor
	/// @param id Unique identifier of this player on the world, if blank a new one will be created
	explicit Player(const QByteArray& id = "", const QString& nickname = "", QObject* parent = nullptr);
	/// Destructor
	virtual ~Player();
	/// Get access to the unique id of this player on the world
	inline const QByteArray& getId() const { return id; }
	/// Get access to the nickname
	inline const QString& getNickname() const { return nickname; }
	/// Changes the nickname to this player. Do not save the changes
	inline void setNickname(const QString& nickname) { this->nickname = nickname; }
	/// Rename this player
	bool rename(const QString& newNickname, bool save = true);
	/// Load player data from settings
	bool load();
	/// Save an entry for this player on settings
	bool save();
	/// Returns true if this player completed the unit with the given id (or base filename)
	bool hasCompletedUnit(const QString& unitId);
	/// Get the data directory for this user. If the data directory does not exist, it is created
	/// The directory existence is tested each time this function is called. This may be not
	/// efficient, but avoids crashes if user deletes the player directories in local machine
	QDir getLocalDataDirectory(bool create);
	/// Creates the directory for player's data
	bool createLocalDataDirectory();
	/// Creates and retuns an plain text unique id for this player using the machine name, the
	/// current date and the nickname of this player
	QString generatePlainId() const;
	/// Creates a MD5 encoded unique id for this player
	/// @param logUserCreation Logs the user creation event
	/// @see createPlainId()
	QByteArray generateCodedId(bool logUserCreation = false) const;
	/// Autogenerates an unique id
	/// @return true if a new code was set, false if code was untouched
	bool autogenerateId();
	/// Removes this player from the settings
	void remove();

  protected:
	/// Returns the path to the directory in the local machine to store data for this player, for
	/// example, his/her products according he or he is going progressing through the game
	/// This method is protected because it does not create the folder if it does not exist
	QString getLocalDataPath() const;
	/// On Unix systems, a /tmp/bn_player symbolic link may be used to reduce bloated output due to
	/// long paths. This method just returns the suggested shortcut path, using the nickname of
	/// the player for readibility
	QString getShortcutPath() const;
};

#endif // PLAYER_H
