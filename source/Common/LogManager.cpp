#include "LogManager.h"

LogManager::LogManager(QObject* parent)
	: QObject(parent)
{
	qInstallMessageHandler(LogManager::messageHandler);
}

LogManager::~LogManager()
{
	qInstallMessageHandler(nullptr);
}

void LogManager::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	QByteArray localMsg = message.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
	  fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
	  break;
	case QtInfoMsg:
	  fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
	  break;
	case QtWarningMsg:
	  fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
	  break;
	case QtCriticalMsg:
	  fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
	  break;
	case QtFatalMsg:
	  fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
	  abort();
	}
}
