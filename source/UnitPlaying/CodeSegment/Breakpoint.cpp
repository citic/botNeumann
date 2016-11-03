#include "Breakpoint.h"

Breakpoint::Breakpoint(const QString& filename, int lineNumber)
	 : filename(filename)
	 , lineNumber(lineNumber)
{
}

Breakpoint::~Breakpoint()
{
}
