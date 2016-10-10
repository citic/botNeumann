#include "DebuggerCall.h"

DebuggerCall::DebuggerCall(const QFileInfo &executablePath, QObject *parent)
	: ToolCall(parent)
	, executablePath(executablePath)
{
}

DebuggerCall::~DebuggerCall()
{
}

QString DebuggerCall::getDebugger()
{
  #if defined(Q_OS_MACX)
	return getCompilerInstallationDirectory() + "ggdb";
  #else
	return getCompilerInstallationDirectory() + "gdb";
#endif
}

#include <QDebug>

void DebuggerCall::start()
{
	const QString& command = QString("'%1' -q -i mi '%2'").arg(getDebugger()).arg(getExecutablePath().absoluteFilePath());
	qDebug() << "DebuggerCall:" << command;
}

