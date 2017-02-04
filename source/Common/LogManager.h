#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QFile>
#include <QLoggingCategory>
#include <QObject>

/// Logs general events related to the botNeumann application
Q_DECLARE_LOGGING_CATEGORY(logApplication)
/// Logs events related to building player solution such as compiling errors
Q_DECLARE_LOGGING_CATEGORY(logBuild)
/// Logs events related to the debugger extraction objects
Q_DECLARE_LOGGING_CATEGORY(logDebugger)
Q_DECLARE_LOGGING_CATEGORY(logDebuggerRequest)
Q_DECLARE_LOGGING_CATEGORY(logDebuggerResponse)
/// Logs events related to the editing process
Q_DECLARE_LOGGING_CATEGORY(logEditor)
/// Logs events related to not-implemented functionality
Q_DECLARE_LOGGING_CATEGORY(logNotImplemented)
/// Logs events related to the user progress through the game
Q_DECLARE_LOGGING_CATEGORY(logPlayer)
/// Temporary debug messages to call attention of programmer
/// Debug temporary are usually disbled, and Critical are shown with code [!!!!!]
Q_DECLARE_LOGGING_CATEGORY(logTemporary)
/// Logs events related to the controller in charge of the visualization process
Q_DECLARE_LOGGING_CATEGORY(logVisualizator)

// A kind of delegate
class MessagesArea;

/** Manages a log file to store events generated by players that will be used to data analysis

	The log manager also has functionality to upload logs to a central network server
**/
class LogManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(LogManager)

  protected:
	/// The log file keeps open while the botNeumann instance is running
	static QFile logFile;
	/// Propagate some messages to the log manager (a kind of delegate)
	static MessagesArea* messagesArea;

  public:
	/// Constructor
	explicit LogManager(QObject *parent = nullptr);
	/// Destructor
	virtual ~LogManager();
	/// Called by the q functions: Info, Debug, Warning, Critical, Fatal
	static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);
	/// Get the log file name for this device
	static QString getLogFilename();
	/// Builds a new file name for this device
	/// This function is only called once on a device
	static QString buildLogFilename(bool saveInSettings);
	/// Called by the MessagesArea to set it as a delegate
	static void setMessagesArea(MessagesArea* messagesArea);

  protected:
	/// Return true if this a very repetitive message that should be not logged
	static bool shouldIgnoreMessage(const QString& message);
	/// Return true if events of the given category should be recorded in log file
	static bool shouldLogToFile(QtMsgType type, const QString& category);
	/// Return true if events of the given category should be printed to standard error
	static bool shouldLogToStdErr(QtMsgType type, const QString& category);
	/// Return true if events of the given category should be shown in GUI (messages area)
	static bool shouldLogToGui(QtMsgType type, const QString& category);
};

#endif // LOGMANAGER_H
