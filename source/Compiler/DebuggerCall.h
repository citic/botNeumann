#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include "ToolCall.h"

/**
	@brief Establishes a session with the debugger in order to create a visualization
*/
class DebuggerCall : public ToolCall
{
	Q_OBJECT
	Q_DISABLE_COPY(DebuggerCall)

  protected:
	/// Absolute path to the player's solution executable that will be run
	QFileInfo executablePath;

  public:
	/// Constructor
	explicit DebuggerCall(const QFileInfo& executablePath, QObject* parent = nullptr);
	/// Destructor
	~DebuggerCall();
	/// Get access to the executable that is debugged
	inline const QFileInfo& getExecutablePath() const { return executablePath; }
};

#endif // DEBUGGERCALL_H
