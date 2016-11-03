#ifndef GUIBREAKPOINT_H
#define GUIBREAKPOINT_H

#include <QTextBlockUserData>

/**
	Tells if a code editor line/block has or not a breakpoint.

	GuiBreakpoints are set by user in code editors. Class Breakpoint manages breakpoints set by
	debugger. See @a Breakpoint class for more details.
**/
class GuiBreakpoint : public QTextBlockUserData
{
	Q_DISABLE_COPY(GuiBreakpoint)

  protected:
	/// True if this breakpoint is updated with object code. False if this breakpoint was set
	/// after a change in the source file that is not synchronized with the object code being
	/// visualizated. An invalid breakpoint is painted gray instead of red in the line number
	/// area, to alert user that the breakpoint is not going to have effect in visualization.
	bool valid;

  public:
	/// Constructor
	explicit GuiBreakpoint(bool valid) : valid(valid) { }
	/// Destructor
	virtual ~GuiBreakpoint() { }
	/// Tells if this breakpoint is valid
	/// @see valid
	inline bool isValid() const { return valid; }
	/// Makes this breakpoint valid or invalid
	inline void makeValid(bool valid) { this->valid = valid; }
};

#endif // GUIBREAKPOINT_H
