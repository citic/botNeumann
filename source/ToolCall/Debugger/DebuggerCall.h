#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include "ToolCall.h"

/**
	@brief Establishes a session with the debugger in order to create a visualization. This base
	class is an abstraction layer for all supported debuggers (Gdb, only for the moment)
*/
class DebuggerCall : public ToolCall
{
	Q_OBJECT
	Q_DISABLE_COPY(DebuggerCall)

  public:
	/// Constructor
	explicit DebuggerCall(QObject* parent = nullptr);
	/// Destructor
	virtual ~DebuggerCall() override;
	/// Emits finished() when it has finished. The result (success, error, or warnings) can be
	/// consulted after the finished() signal has been emitted
	virtual bool start() = 0;
};

#endif // DEBUGGERCALL_H
