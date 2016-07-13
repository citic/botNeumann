#include "DebuggerCall.h"

static const QString gdbPath = QStringLiteral(
#ifdef Q_OS_MAC
	"/opt/local/bin/ggdb");
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
	const QString& command = QStringLiteral("%1 -q --interpreter=mi2").arg(gdbPath);
	printf("%s: starting: %s\n", qPrintable(toolName), qPrintable(command));
	process.start(command);
	process.waitForStarted();
	return process.state() != QProcess::NotRunning;
}
