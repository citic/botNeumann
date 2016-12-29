#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QString>

class GdbTreeNode;

/** Manages breakpoints set in debugger

	There are two types of breakpoints in botNeumann. (1) GuiBreakpoints are set by user in line
	numbers area of code editors. (2) Breakpoints in debugger call. Usually they are synchronized
	because when visualization starts, GuiBreakpoints are sent to debugger. After visualization
	has started, user may remove or created new GuiBreakpoints. These are sent againg to the
	running debugger session and they are considered "valid" because source code is synchronized.
	But user may introduce line changes, such as insert lines or move code in editor. The source
	code in editor gets out-synchronized with source code in debugger. Any new breakpoint
	modification gets tricky, because line numbers is the method to "identify" breakpoints.

	Breakpoints are used in botNeumann to control the visualization. When users ask to run their
	solutions, there will happen two scenearios:

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

	A GDB breakpoint has the following fields (values are examples):
	- number="1"
	- type="breakpoint"
	- disp="keep"
	- enabled="y"
	- addr="0x00001000e00"
	- func="main"
	- file="rand.c"
	- fullname="/tmp/app/rand.c"
	- line="7"
	- thread-groups=["i1"]
	- times="0"
	- original-location="main"

	Extra:
	- at="main+4"
*/
class DebuggerBreakpoint
{
	Q_DISABLE_COPY(DebuggerBreakpoint)

  protected:
	/// Breakpoint number. Debuggers identify breakpoints with numbers when program is running
	int number;
	/// Debuggers report the memory address of the instruction to be halted
	size_t address;
	/// Debuggers report the function where breakpoints are set
	QString functionName;
	/// The file where the player wants to stop the execution
	QString filename;
	/// The full path of the source file where the breakpoint is
	QString filepath;
	/// The number of line where the breakpoint was set by user
	int lineNumber;
	/// @todo: thread-groups?

  public:
	/// Builds a user defined breakpoint
	explicit DebuggerBreakpoint(const GdbTreeNode& breakpointNode);
	/// Destructor
	~DebuggerBreakpoint();
	/// Updates the fields of this object from the parsed node from debugger
	void updateFromNode(const GdbTreeNode& breakpointNode);
	/// Return the number that debugger uses to identify this breakpoint
	inline int getNumber() const { return number; }
	/// Return the filename where this breakpoint is set
	inline const QString& getFilename() const { return filename; }
	/// Return the line number where this breakpoint is set
	inline int getLineNumber() const { return lineNumber; }
};

#endif // BREAKPOINT_H