#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

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
};

#endif // PLAYER_H
