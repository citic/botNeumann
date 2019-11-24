#ifndef GUIBREAKPOINT_H
#define GUIBREAKPOINT_H

#include <QTextBlockUserData>

class DebuggerBreakpoint;

/**
	Tells if a code editor line/block has or not a breakpoint.

	GuiBreakpoints are set by user in code editors. Class Breakpoint manages breakpoints set by
	debugger. See @a Breakpoint class for more details.
**/
class GuiBreakpoint : public QTextBlockUserData
{
	Q_DISABLE_COPY(GuiBreakpoint)

  public:
	/// GuiBreakpoint actions are issued when events related to breakpoints are issued, for
	/// example, creating a breakpoint or removing it.
	enum class Action
	{
		unknown,
		created,
		removed,
		// disabled,
		// updated,
	};

  protected:
	/// The source file where this breakpoint is defined
	QString filename;
	/// The line number within @a filename where this breakpoint is set
	/// This field is updated each time the list of breakpoints is retrieved to be sent to debugger
	int lineNumberInEditor;
	/// The number that this breakpoint had when the player's solution was compiled/visualizated
	/// for last time. Therfore its number in object code. If this breakpoint was set
	/// after a change in the source file that is not synchronized with the object code being
	/// visualizated, this number will be distinct to its current line number in the code editor.
	/// An invalid breakpoint is painted gray instead of red in the line number
	/// area, to alert user that the breakpoint is not going to have effect in visualization.
	int lineNumberInObjectCode;
	/// This breakpoint object can be used to communicate changes in breakpoints
	Action action;

  public:
	/// Constructor
	explicit GuiBreakpoint(const QString& filename, int lineNumberInEditor);
	/// Destructor
	virtual ~GuiBreakpoint() override;
	/// Gets the filename where this breakpoint is defined
	const QString& getFilename() const { return filename; }
	/// Gets the line where this file is defined
	inline int getLineNumber() const { return lineNumberInEditor; }
	/// Updates the line number
	/// @param both Send true to update the line number in object code as well
	void updateLineNumber(int lineNumber, bool both);
	/// Get access to the last line number that debugger knows for this breakpoint
	inline int getLineNumberInObjectCode() const { return lineNumberInObjectCode; }
	/// Called when user starts the visualization. All breakpoints are updated with their current
	/// line numbers
	inline void setLineNumberInObjectCode(int lineNumber) { this->lineNumberInObjectCode = lineNumber; }
	/// Returns true if the line in editor is synchornized with the last object code generated
	/// This method updates the internal current line in editor with the given one
	bool isSyncWithObjectCode(int currentLineNumber);
	/// Builds a text in format "filename:lineNumber" where a breakpoint is set
	/// The line number on object code is used instead of the number in editor
	/// "Original location" is the name used by GDB for the pair source:line
	QString buildOriginalLocation() const;
	/// Tells if this object transports an action related to this breakpoint
	inline Action getAction() const { return action; }
	/// Makes this object to transport an action related to this breakpoint
	inline void setAction(Action action) { this->action = action; }
	/// Returns true if this GUI breakpoint is the same than the given debugger breakpoint. Two
	/// breakpoints are considered the same if they are set at the same file and line
	bool matches(const DebuggerBreakpoint& debuggerBreakpont) const;
};

#endif // GUIBREAKPOINT_H
