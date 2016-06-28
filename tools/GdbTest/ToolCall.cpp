#include "ToolCall.h"

ToolCall::ToolCall(const QString& toolName, QObject *parent)
	: QObject(parent)
	, toolName(toolName)
	, process(this)
{
}

ToolCall::~ToolCall()
{
	printf("%s: object deleted\n", qPrintable(toolName));
}

void ToolCall::processUserProgramFinished()
{
	printf("%s: execution finished\n", qPrintable(toolName));
	emit userProgramFinished();
}

void ToolCall::processErrorOcurred(QProcess::ProcessError error)
{
	printf("%s: error %i\n", qPrintable(toolName), (int)error);
}
