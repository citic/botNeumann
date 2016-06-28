#include "ToolCall.h"

ToolCall::ToolCall(const QString& toolName, QObject *parent)
	: QObject(parent)
	, toolName(toolName)
	, process(this)
{
	connect( &process, SIGNAL(finished(int)), this, SLOT(processFinished()) );
	connect( &process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(processErrorOccurred(QProcess::ProcessError)) );
}

ToolCall::~ToolCall()
{
	printf("%s: object deleted\n", qPrintable(toolName));
}

qint64 ToolCall::processId() const
{
	return process.processId();
}

void ToolCall::processFinished()
{
	printf("%s: execution finished\n", qPrintable(toolName));
	emit toolFinished();
}

void ToolCall::processErrorOccurred(QProcess::ProcessError error)
{
	printf("%s: error %i\n", qPrintable(toolName), (int)error);
	emit errorOccurred();
}
