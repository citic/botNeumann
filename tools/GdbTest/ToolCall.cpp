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

// @see QProcess::ProcessError enum
static const char* const ProcessErrorStr[] =
{
	"FailedToStart", //### file not found, resource error
	"Crashed",
	"Timedout",
	"ReadError",
	"WriteError",
	"UnknownError"
};

void ToolCall::processErrorOccurred(QProcess::ProcessError error)
{
	printf("%s: error %i: %s\n", qPrintable(toolName), (int)error, ProcessErrorStr[(int)error]);
	emit errorOccurred();
}
