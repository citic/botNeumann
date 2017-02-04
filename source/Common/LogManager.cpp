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

Q_LOGGING_CATEGORY(logApplication,      "A---")
Q_LOGGING_CATEGORY(logEditor,           "AEdt")
Q_LOGGING_CATEGORY(logDebugger,         "ADbg")
Q_LOGGING_CATEGORY(logDebuggerRequest,  "ADbC")
Q_LOGGING_CATEGORY(logDebuggerResponse, "ADbR")
Q_LOGGING_CATEGORY(logNotImplemented,   "ANIm")
Q_LOGGING_CATEGORY(logVisualizator,     "AVis")
Q_LOGGING_CATEGORY(logPlayer,           "P---")
Q_LOGGING_CATEGORY(logBuild,            "PBld")
Q_LOGGING_CATEGORY(logTemporary,        "!!!!")

QFile LogManager::logFile;

// Separator used in the log file
static const char sep = '\t';

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
		// Log file is ready to receive messages. If it is a new file save a header
		if ( ! exists )
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
	// Hack: do not log =library-load gdb messages
	if ( shouldIgnoreMessage(message) )
		return;

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

	QString category(context.category);

	if ( logCategoryToFile(type, category) )
	{
		// A convenience object to format the text that will be sent to the logFile
		QTextStream logStream(&logFile);
		logStream << datetime << sep << elapsed << sep << playerId << sep << typeStr << sep << context.category << sep << message << '\n';
		logStream.flush();
	}

	// Also copy the message to the standard error, but do not report the 'default' category
	if ( logCategoryToStdErr(type, category) )
	{
		QTextStream stderrStream(stderr);
		stderrStream << '[' << typeStr[0] << context.category << "] [" << message << "]\n";
		stderrStream.flush();
	}

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

bool LogManager::logCategoryToFile(QtMsgType type, const QString& category)
{
	if ( type == QtDebugMsg )
		return false;
	if ( category.startsWith("ADb") )
		return false;

	return true;
}

bool LogManager::logCategoryToStdErr(QtMsgType type, const QString& category)
{
	if ( type == QtDebugMsg && category == "!!!!" )
		return false;

	return true;
}

bool LogManager::shouldIgnoreMessage(const QString& message)
{
	if ( message.startsWith("=library-loaded") )
		return true;
	if ( message.contains("OSO archive file") )
		return true;

  #ifdef Q_OS_MAC
	if ( message.contains("/private/var") )
		return true;
  #endif

	return false;
}
