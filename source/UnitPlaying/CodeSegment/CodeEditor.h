#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QSet>

class LineNumberArea;
class QTimer;
class PlayerSolution;
class SyntaxHighlighter;
class Unit;

/**
	Edits one source file at time

	@todo Replace CodeSegment::codeEditor by a QStackedWidget, in order to support several
	source files and breakpoints across the entire player's solution
*/
class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

  protected:
	/// Store formatting rules for C++
	SyntaxHighlighter* highlighter;
	/// Reference to existing unit being played
	Unit* unit;
	/// Reference to existing user's solution to the unit being played
	PlayerSolution* playerSolution;
	/// The full path to the source file being edited
	QString filepath;
	/// Fires a short time after the last made change in the document, in order to do some tasks
	/// such as auto save() and auto compile()
	QTimer* autoSaveTimer;
	/// Object that paints line numbers in the left margin of the code editor
	LineNumberArea* lineNumberArea;

  public:
	/// Constructor
	explicit CodeEditor(QWidget* parent = nullptr);
	/// Destructor
	~CodeEditor();
	/// Set the unit being solved by player and his/her solution
	/// @remark This method must be called when the UnitPlayingScene is launched before asking
	/// to edit any player-solution's source files
	void setPlayerSolutionForUnit(Unit* unit, PlayerSolution* playerSolution);
	/// Restores the last code made by player for the given unit, or the default unit's code if
	/// player nas not played this unit
	/// @param filepath Full path to the active source file to be edited by the player. In order
	/// to solve the current unit's problem, the player may generate several files. Just one file
	/// can be edited at time. The fullpath of the active source file is given in this parameter.
	/// The file may exist or not. If the file exists, it will be loaded. Otherwise an initial
	/// code provided by the unit will be loaded and this filename will be stored internally. When
	/// player changes the code, the autosave function will active and it will use this filename
	/// to store the changes
	/// @return true on success, false otherwise. ToDo: decide how to report errors?
	bool loadInitialFile(const QString& filepath);
	/// Loads a file given for its full path
	bool loadFile(const QString& filepath);
	/// Calculate the width in pixels required by the line number area. The width depends on the
	/// number of lines of the current document
	int getLineNumberAreaWidth();
	/// When the line number area object must be painted, this method is called
	/// @remarks LineNumberArea class calls this method
	void lineNumberAreaPaintEvent(QPaintEvent* event);
	/// When the users clicks a line number this method is called to toggle a breakpoint
	/// @remarks LineNumberArea class calls this method
	void toggleBreakpointEvent(QMouseEvent* event);
	/// Toggles the breakpoint at the given line number
	/// Returns the current state of the breakpoint at lineNumber
	/// @param lineNumber a number starting at 1 indicating the line number where user made click
	void toggleBreakpoint(QTextBlock& block);
	/// Get access to the list of user-defined breakpoints
	QList<QString> retrieveBreakpoints() const;

  public slots:
	/// Saves if there are changes to the @a filepath document in secondary memory
	/// @return true on success or when there is not need to save, false on error
	bool saveChanges();
	/// Place cursor in the given line (block) and column
	void placeCursor(int line, int column );

  protected:
	/// Load an initial code provided by the unit. This method is called when the player has never
	/// played the current unit before
	bool loadUnitInitialCode();
	/// Load the file contents of the file which full path is stored in @a filepath member data
	bool loadFileContents();
	/// Overrided in order to adjust size of the line number area when the code editor is resize
	virtual void resizeEvent(QResizeEvent* event) override;
	/// If the given block has a breakpoint, paints a mark in the line numbers area
	/// If the breakpoint is valid, the mark is red, else, gray
	void paintBreakpoint(QTextBlock& block, QPainter& painter, int top, int width, int fontHeight);

  protected slots:
	/// Called each time the document is changed
	void documentChanged();
	/// Saves always the content of editor to the @a filepath document in secondary memory
	/// @return true on success, false on error
	bool save();
	/// Make the left margin of the text editor bigger, in order to have space to the line number
	/// area widget
	void updateLineNumberAreaWidth();
	/// @brief Invoked when the editor's viewport has been scrolled.
	/// In the editor we resize and draw the line numbers on the LineNumberArea. We need to do this
	/// when the number of lines in the editor changes, and when the editor's viewport() is scrolled
	/// @param rect The part of the editing area that is do be updated (redrawn).
	/// @param dy holds the number of pixels the view has been scrolled vertically.
	void updateLineNumberArea(const QRect& rect, int dy);
	/// Whenever the cursor's position changes, we highlight the current line
	void highlightCurrentLine();
};

#endif // CODEEDITOR_H
