#include "Player.h"
#include "Common.h"
#include "LogManager.h"
#include "Util.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include <QStandardPaths>

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
	nickname = settings.value( sk(playerGroup + "/Nickname"), "").toString();
	return true;
}

bool Player::save()
{
	QSettings settings;
	const QString& playerGroup = "Players/" + id;
	if ( settings.contains(sk(playerGroup)) == false )
	{
		const QString& dateTime = QDateTime::currentDateTime().toString("yyMMdd-hhmmss");
		settings.setValue(sk(playerGroup + "/Created"), dateTime);
	}
	settings.setValue(sk(playerGroup + "/Nickname"), nickname);
	settings.sync();
	return true;
}

bool Player::hasCompletedUnit(const QString& unitFilename)
{
	QSettings settings;
	const QString& key = buildCompletedUnitKey(unitFilename);
	return settings.value(sk(key), false).toBool();
}

bool Player::setCompletedUnit(const QString& filename, bool completed)
{
	QSettings settings;
	const QString& key = buildCompletedUnitKey(filename);
	settings.setValue(sk(key), completed);
	settings.sync();
	return true;
}

QString Player::buildCompletedUnitKey(const QString& filename) const
{
	const QString& unitId = QFileInfo(filename).baseName();
	return "Players/" + id + '/' + unitId + "/Completed";
}

QString Player::getLocalDataPath() const
{
	return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + id;
}

QString Player::getShortcutPath() const
{
	return "/tmp/bn_" + nickname;
}

QDir Player::getLocalDataDirectory(bool create)
{
	// On Unix-like systems, try the shortcut symlink first
  #ifndef Q_OS_WIN
	QDir shortcut( getShortcutPath() );
	if ( shortcut.exists() )
		return shortcut;
  #endif

	// The shortcut does not exist, use the long path
	QDir dir( getLocalDataPath() );
	if ( create && ! dir.exists() )
		createLocalDataDirectory();

	// On Unix-like systems, create a short symbolic link to reduce bloated output
  #ifndef Q_OS_WIN
	QFile::link( dir.absolutePath(), getShortcutPath() );
	if ( shortcut.exists() )
		return shortcut;
  #endif

	return dir;
}

bool Player::createLocalDataDirectory()
{
	const QString& path = getLocalDataPath();
	if ( Util::createDirectory( path ) == false )
	{
		qCCritical(logPlayer) << "Unable to create user data directory:" << path;
		return false;
	}

	return true;
}

QString Player::generatePlainId() const
{
	QHostInfo hostInfo;
	const QString& hostname = hostInfo.localHostName();
	const QString& datetime = QDateTime::currentDateTime().toString("yyMMdd_hhmmss");
	return hostname + '-' + datetime + '-' + nickname;
}

QByteArray Player::generateCodedId(bool logUserCreation) const
{
	const QString& plainId = generatePlainId();
	const QByteArray& hash = QCryptographicHash::hash(qPrintable(plainId), QCryptographicHash::Md5).toHex();
	if ( logUserCreation )
		qCInfo(logPlayer, "Player %s created with id %s", qPrintable(plainId), qPrintable(hash));
	return hash;
}

bool Player::autogenerateId()
{
	if ( this->id.isEmpty() )
	{
		this->id = generateCodedId(true);
		return true;
	}
	return false;
}

void Player::remove()
{
	const QString& playerGroup = "Players/" + id;
	QSettings().remove( sk(playerGroup) );
}
