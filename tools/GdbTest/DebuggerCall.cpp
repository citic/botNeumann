#include "DebuggerCall.h"

static const QString gdbPath = QStringLiteral(
#ifdef Q_OS_MAC
	"ggdb");
#else
	"gdb");
#endif

DebuggerCall::DebuggerCall(QObject *parent)
	: ToolCall("DebuggerCall", parent)
{
}

DebuggerCall::~DebuggerCall()
{
	// Exit gdb cleanly
	printf("%s: exiting gdb...\n", qPrintable(toolName));
	process.write("-gdb-exit\n");
	process.terminate();
	process.waitForFinished();
}

bool DebuggerCall::start()
{
	printf("%s: starting gdb...\n", qPrintable(toolName));
	const QString& command = QStringLiteral("%1 --interpreter=mi2").arg(gdbPath);
	process.start(command);
	process.waitForStarted();
	return process.state() != QProcess::NotRunning;
}
