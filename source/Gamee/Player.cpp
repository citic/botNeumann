#include "Player.h"
#include <QDateTime>
#include <QSettings>

Player::Player(const QString& nickname, QObject *parent)
	: QObject(parent)
	, nickname(nickname)
{
}

Player::~Player()
{
}

bool Player::save()
{
	QSettings settings;
	const QString& playerGroup = "Players/" + nickname;
	if ( settings.contains(playerGroup) == false )
	{
		const QString& dateTime = QDateTime::currentDateTime().toString("yyMMdd-hhmmss");
		settings.setValue(playerGroup + "/Created", dateTime);
	}
	return true;
}

bool Player::hasCompletedUnit(const QString& filename)
{
	QSettings settings;
	const QString& key = "Players/" + nickname + '/' + filename + "Completed";
	return settings.value(key, false).toBool();
}
