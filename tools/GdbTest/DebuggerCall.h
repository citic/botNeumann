#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include "ToolCall.h"

class DebuggerCall : public ToolCall
{
	Q_OBJECT

  public:
	explicit DebuggerCall(QObject *parent = nullptr);
	virtual ~DebuggerCall();
	bool start();
};

#endif // DEBUGGERCALL_H
