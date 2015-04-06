#include "Player.h"
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

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

bool Player::hasCompletedUnit(const QString& unitId)
{
	QSettings settings;
	const QString& key = "Players/" + nickname + '/' + unitId + "Completed";
	return settings.value(key, false).toBool();
}

QString Player::getLocalDataPath() const
{
	return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + nickname;
}

QDir Player::getLocalDataDirectory() const
{
	QDir dir( getLocalDataPath() );
	if ( ! dir.exists() ) dir.mkdir( dir.path() );
	return dir;
}
