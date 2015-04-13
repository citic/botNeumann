#include "ToolCall.h"
#include <QDateTime>
#include <QProcess>

ToolCall::ToolCall(QObject* parent)
	: QObject(parent)
	, state( ToolCallState::unknown )
	, process(nullptr)
{
}

ToolCall::~ToolCall()
{
}

bool ToolCall::isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2)
{
	return file1.lastModified() > file2.lastModified();
}

QString ToolCall::getCxxCompiler()
{
  #if defined(Q_OS_MACX)
	return "clang++";
  #else
	return "g++";
  #endif
}

QStringList ToolCall::getDefaultCompilerArguments()
{
	QStringList arguments;
	arguments << "-g" << "-Wall" << "-Wextra" << "-std=c++11";
	if ( getCxxCompiler() == "clang++" )
		arguments << "-fdiagnostics-parseable-fixits";
	return arguments;
}

QStringList ToolCall::getDefaultLinkerArguments()
{
	QStringList arguments;
  #if ! defined(Q_OS_WIN)
	arguments << "-lm";
  #endif
	return arguments;
}
