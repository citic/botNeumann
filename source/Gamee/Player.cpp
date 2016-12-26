#include "Player.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

Player::Player(const QByteArray& id, const QString& nickname, QObject *parent)
	: QObject(parent)
	, id(id)
	, nickname(nickname)
{
}

Player::~Player()
{
}

bool Player::rename(const QString& newNickname, bool save)
{
	this->nickname = newNickname;
	if ( save ) this->save();
	return true;
}

bool Player::load()
{
	QSettings settings;
	const QString& playerGroup = "Players/" + id;
	nickname = settings.value(playerGroup + "/Nickname", "").toString();
	return true;
}

bool Player::save()
{
	QSettings settings;
	const QString& playerGroup = "Players/" + id;
	if ( settings.contains(playerGroup) == false )
	{
		const QString& dateTime = QDateTime::currentDateTime().toString("yyMMdd-hhmmss");
		settings.setValue(playerGroup + "/Created", dateTime);
	}
	settings.setValue(playerGroup + "/Nickname", nickname);
	settings.sync();
	return true;
}

bool Player::hasCompletedUnit(const QString& unitId)
{
	QSettings settings;
	const QString& key = "Players/" + id + '/' + unitId + "/Completed";
	return settings.value(key, false).toBool();
}

QString Player::getLocalDataPath() const
{
	return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + id;
}

QDir Player::getLocalDataDirectory() const
{
	QDir dir( getLocalDataPath() );
	if ( ! dir.exists() )
		if ( ! dir.mkpath( "." ) )
			qWarning() << "Unable to create user data directory: " << dir.path();

	return dir;
}

QString Player::generatePlainId() const
{
	QHostInfo hostInfo;
	const QString& hostname = hostInfo.localHostName();
	const QString& datetime = QDateTime::currentDateTime().toString("yyMMdd_hhmmss");
	qDebug("Generating id from %s", qPrintable(hostname + '-' + datetime + '-' + nickname));
	return hostname + '-' + datetime + '-' + nickname;
}

QByteArray Player::generateCodedId() const
{
	return QCryptographicHash::hash(qPrintable(generatePlainId()), QCryptographicHash::Md5).toHex();
}

bool Player::autogenerateId()
{
	if ( this->id.isEmpty() )
	{
		this->id = generateCodedId();
		return true;
	}
	return false;
}

void Player::remove()
{
	const QString& playerGroup = "Players/" + id;
	QSettings().remove(playerGroup);
}
