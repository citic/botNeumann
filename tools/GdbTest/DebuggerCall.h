#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include "ToolCall.h"

class QSocketNotifier;

class DebuggerCall : public ToolCall
{
	Q_OBJECT

  protected:
	int pseudoterminalDeviceFileDescriptor = 0;
	QSocketNotifier* pseudoterminalActivityMonitor = nullptr;

  public:
	explicit DebuggerCall(QObject *parent = nullptr);
	virtual ~DebuggerCall();
	bool start();

  protected:
	/// A pseudoterminal is used to execute gdb
	bool createPseudoterminal();
	void startMonitoringPseudoterminal();

  private slots:
	/// Called each time GDB produces output
	void onGdbOutput(int fileDescriptor);
};

#endif // DEBUGGERCALL_H
