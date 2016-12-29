#include "LogManager.h"
#include "BotNeumannApp.h"
#include "Player.h"

#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QMutex>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

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
			qInfo("Appending to log file '%s'", qUtf8Printable(filename));
		else
		{
			// It is a new log file, write the CSV header line and the first event
			logFile.write("Date;Time;User;Type;Category;Details\n");
			qInfo("Log file '%s' created", qUtf8Printable(filename));
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

	// Write the date and time when the event was logged
	const QString& datetime = QDateTime::currentDateTime().toString("yyMMdd;hhmmss");

	// Store the id of the current player who generated the event
	Player* player = BotNeumannApp::getInstance()->getCurrentPlayer();
	const QByteArray& playerId = player ? player->getId() : QByteArray();

	// Avoid two or more threads writing on the log file symultaneously
	static QMutex mutex;
	QMutexLocker lock(&mutex);

	// A convenience object to format the text that will be sent to the logFile
	QTextStream logStream(&logFile);
	logStream << datetime << ';' << playerId << ';' << typeStr << ';' << context.category << ';' << message << '\n';

	// Also copy the message to the standard error
	QTextStream stderrStream(stderr);
	stderrStream << typeStr << ": " << context.category << ": " << message << '\n';

	if ( type == QtFatalMsg )
		abort();
}

QString LogManager::getLogFilename()
{
	// If log filename is in settings, use it, otherwise generate one
	const QVariant& filename = QSettings().value("Log/Filename");
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
		QSettings().setValue("Log/Filename", filename);

	return filename;
}
