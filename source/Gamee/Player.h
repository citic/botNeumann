#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class QDir;

class Player : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Player)

  protected:
	/// Identifier of the player
	QString nickname;

  public:
	/// Constructor
	explicit Player(const QString& nickname = "", QObject* parent = nullptr);
	/// Destructor
	~Player();
	/// Get access to the nickname
	inline const QString& getNickname() const { return nickname; }
	/// Save an entry for this player on settings
	bool save();
	/// Returns true if this player completed the unit with the given id (or base filename)
	bool hasCompletedUnit(const QString& unitId);
	/// Returns the path to the directory in the local machine to store data for this player, for
	/// example, his/her products according he or he is going progressing through the game
	QString getLocalDataPath() const;
	/// Get the data directory for this user. If the data directory does not exist, it is created
	/// The directory existence is tested each time this function is called. This may be not
	/// efficient, but avoids crashes if user deletes the player directories in local machine
	QDir getLocalDataDirectory() const;
};

#endif // PLAYER_H
