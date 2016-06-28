#include "DebuggerCall.h"
#include <stdio.h>

static const QString gdbPath = QStringLiteral(
#ifdef Q_OS_MAC
	"ggdb");
#else
	"gdb");
#endif

DebuggerCall::DebuggerCall(QObject *parent)
	: QObject(parent)
	, process(this)
{
}

DebuggerCall::~DebuggerCall()
{
	// Exit gdb cleanly
	fprintf(stderr, "DebuggerCall: exiting gdb...\n");
	process.write("-gdb-exit\n");
	process.terminate();
	process.waitForFinished();
	fprintf(stderr, "DebuggerCall object deleted\n");
}

bool DebuggerCall::start()
{
	const QString& command = QStringLiteral("%1 --interpreter=mi2").arg(gdbPath);
	process.start(command);
	process.waitForStarted();
	return process.state() != QProcess::NotRunning;
}
