#include "DebuggerCall.h"

DebuggerCall::DebuggerCall(QObject *parent)
	: ToolCall(parent)
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
