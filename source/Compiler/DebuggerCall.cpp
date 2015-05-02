#include "DebuggerCall.h"

DebuggerCall::DebuggerCall(const QFileInfo &executablePath, QObject *parent)
	: ToolCall(parent)
	, executablePath(executablePath)
{
}

DebuggerCall::~DebuggerCall()
{
}
