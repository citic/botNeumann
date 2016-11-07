#include "DebuggerBreakpoint.h"

DebuggerBreakpoint::DebuggerBreakpoint(const QString& filename, int lineNumber)
	 : filename(filename)
	 , lineNumber(lineNumber)
{
}

DebuggerBreakpoint::~DebuggerBreakpoint()
{
}
