#include "LogManager.h"
#include "BotNeumannApp.h"
#include "Common.h"
#include "Player.h"

#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QMutex>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

Q_LOGGING_CATEGORY(logApplication, "app")
Q_LOGGING_CATEGORY(logBuild, "player.build")
Q_LOGGING_CATEGORY(logDebugger, "app.debugger")
Q_LOGGING_CATEGORY(logEditor, "app.editor")
Q_LOGGING_CATEGORY(logNotImplemented, "app.notimplemented")
Q_LOGGING_CATEGORY(logPlayer, "player")
Q_LOGGING_CATEGORY(logVisualizator, "app.visualizator")

QFile LogManager::logFile;

// Text versios of QtMsgType
const char* const msgTypeStr[] = { "Debug", "Warning", "Critical", "Fatal", "Info" };

LogManager::LogManager(QObject* parent)
	: QObject(parent)
{
	// Open the log file for appending events
	const QString& filename = getLogFilename();
	logFile.setFileName(filename);
	bool exists = QFile::exists(filename);
	if ( logFile.open(QFile::Append | QFile::Text) )
	{
		// The file was opened successfully. Install the message handler that will use it
		qInstallMessageHandler(LogManager::messageHandler);

		// Log file is ready to receive messages. If it is a new file save a header
		if ( exists )
			qCDebug(logApplication, "Appending to log file '%s'", qUtf8Printable(filename));
		else
		{
			// It is a new log file, write the CSV header line and the first event
			logFile.write("Date;Time;SessionTime;User;Type;Category;Details\n");
			qCInfo(logApplication(), "Log file '%s' created", qUtf8Printable(filename));
		}

	}
	else
		qWarning("Error: could not open log file '%s'", qUtf8Printable(filename));
}

LogManager::~LogManager()
{
	qInstallMessageHandler(nullptr);
	logFile.close();
}


void LogManager::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	// Convert the type to a string
	Q_ASSERT(type < sizeof(msgTypeStr) / sizeof(msgTypeStr[0]));
	const char* typeStr = msgTypeStr[type];

	// Write the date and time when the event was logged and the seconds from session start
	const QDateTime& now = QDateTime::currentDateTime();
	const QString& datetime = now.toString("yyMMdd;hhmmss");

	// Used to calculate the ellapsed time of each event from the start of the session
	static QDateTime sessionStart = QDateTime::currentDateTime();
	qint64 elapsed = sessionStart.secsTo(now);

	// Store the id of the current player who generated the event
	Player* player = BotNeumannApp::getInstance()->getCurrentPlayer();
	const QByteArray& playerId = player ? player->getId() : QByteArray();

	// Avoid two or more threads writing on the log file symultaneously
	static QMutex mutex;
	QMutexLocker lock(&mutex);

	// A convenience object to format the text that will be sent to the logFile
	QTextStream logStream(&logFile);
	logStream << datetime << ';' << elapsed << ';' << playerId << ';' << typeStr << ';' << context.category << ';' << message << '\n';
	logStream.flush();

	// Also copy the message to the standard error, but do not report the 'default' category
	QTextStream stderrStream(stderr);
	stderrStream << typeStr;
	if ( qstrcmp(context.category, "default") != 0 )
		stderrStream << ": " << context.category;
	stderrStream << ": " << message << '\n';
	stderrStream.flush();

	// Fatal messages cause application to stop
	if ( type == QtFatalMsg )
		abort();
}

QString LogManager::getLogFilename()
{
	// If log filename is in settings, use it, otherwise generate one
	const QVariant& filename = QSettings().value(sk("Log/Filename"));
	return filename.isNull() ? buildLogFilename(true) : filename.toString();
}

QString LogManager::buildLogFilename(bool saveInSettings)
{
	// Build a log filename using the hostname and the creation date to avoid collisions with other
	// logs when they are submitted to the shared network server
	const QString& dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	const QString& hostname = QHostInfo().localHostName();
	const QString& datetime = QDateTime::currentDateTime().toString("yyMMdd_hhmmss");
	const QString& filename = QString("%1/log-%2-%3.csv").arg(dirPath).arg(hostname).arg(datetime);

	QDir dir( dirPath );
	if ( ! dir.exists() )
		if ( ! dir.mkpath( "." ) )
			qWarning() << "Unable to create application data directory: " << dir.path();

	// Save the file name on the settings for future use
	if ( saveInSettings )
		QSettings().setValue( sk("Log/Filename"), filename );

	return filename;
}
