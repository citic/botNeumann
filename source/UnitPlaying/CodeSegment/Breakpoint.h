#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QTextBlockUserData>

#if 0
/** Manages all breakpoints set in the application.

	Debuggers work with breakpoints. When users ask to run their solutions, there will happen two
	scenearios:

	-	If users have not set any explicit breakpoints, botNeumann stops at main() function.
		Therefore an implicit/hidden breakpoint is set. The visualization starts advancing one
		instruction at time at the speed set by the speed control.

	-	If users have defined explicit breakpoints, several instances of this structure are created
		and passed to the debugger (e.g: gbd). botNeumann also adds the implicit breakpoint at
		main() function. When visualization starts, it do not stops at main. It continues at
		maximum speed allowed by the computer (issuing gdb next instructions), in the same fashion
		the speed gauge is at top. Visualization will stop at the first user-defined breakpoint
		reached.

	User defined breakpoints are part of the player solution space. If user solution has several
	files, breakpoints may belong to different source files. The PlayerSolution class is able to
	store breakpoints in the user configuration files, in order to recover them when user continues
	a previous session.
*/
class Breakpoint
{
	Q_DISABLE_COPY(Breakpoint)

  protected:
	/// The file where the player wants to stop the execution
	QString filename;
	/// The number of line where the breakpoint was set by user
	int lineNumber;

	// Properties updated by the debugger:
	/// Breakpoint number. Debuggers identify breakpoints with numbers when program is running
	int number;
	/// The full path of the source file where the breakpoint is
	QString filepath;
	/// Debuggers report the function where breakpoints are set
	QString functionName;
	/// Debuggers report the memory address of the instruction to be halted
	size_t address;

  public:
	/// Builds a user defined breakpoint
	explicit Breakpoint(const QString& filename, int lineNumber);
	/// Deletes this breakpoint
	virtual ~Breakpoint();
};

#endif

/**
	Simply tells if a code editor line/block has or not a breakpoint
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

#endif // BREAKPOINT_H
