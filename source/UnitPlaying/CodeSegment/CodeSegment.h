#ifndef CODESEGMENT_H
#define CODESEGMENT_H

#include "Common.h"

#include <QDockWidget>

class CodeEditor;
class Compiler;
class CompiledProgram;
class ExecutionThread;
class GuiBreakpoint;
class QAction;
class QComboBox;
class QMainWindow;
class QSlider;
class Player;
class PlayerSolution;
class Unit;

/**
	@brief Represents the data segment for the current unit.

	In order to solve the unit's problem, the current player may create several source files
	(e.g: main.cpp, MyClass.h, MyClass.cpp). These files are stored in a subfolder for the player

	The data segment (this class) lists all the source files that compound the current's player
	solution, and let him/her to switch between them. The CodeEditor object is able to show and
	edit only one of these files at time. Therefore, this class deals with some source files at
	time, whereas CodeEditor must be notified when the current file has changed.
 */
class CodeSegment : public QDockWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(CodeSegment)

  protected: // General objects
	/// In order to have a toolbar, the widget of this code editor must be a main window
	QMainWindow* innerMainWindow;
	/// Object where the actual source code is shown and edited
	CodeEditor* codeEditor = nullptr;
	/// Manages the list of source files that compound the player's solution to an unit
	PlayerSolution* playerSolution = nullptr;

  protected: // Edit toolbar
	/// Create new files in the solution: header file, source file and C++ classes
	QAction* newFileAction;
	/// Undoes the last action done in the editor
	QAction* undoAction;
	/// Redoes the last undone action in the editor
	QAction* redoAction;
	/// Cuts the current selection in the text editor and places it into the clipboard
	QAction* cutAction;
	/// Copies the current selection in the text editor and places a copy into the clipboard
	QAction* copyAction;
	/// Paste a copy of the clipboard contents over the current selection in the text editor
	QAction* pasteAction;
	/// Let player change the file to be edited, if the solution is compound of several files
	QComboBox* fileSelector;

  protected: // Run toolbar
	/// Executes and animates the code or pauses it if already running
	QAction* runOrPauseAction;
	/// If visualisation is paused, executes the next code statement in the current function
	QAction* stepOverAction;
	/// If visualisation is paused, executes the next code statement entering in functions if they
	/// are defined by user
	QAction* stepIntoAction;
	/// If visualisation is paused, executes the exits from the current function
	QAction* stepOutAction;
	/// Stops the visualisation of the code, if it is running
	QAction* stopAction;
	/// Allows user to set the speed of the visualization
	QSlider* visualizationSpeedSlider;

  public:
	/// Constructor
	explicit CodeSegment(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	/// Destructor
	virtual ~CodeSegment();
	/// Get access to the code editor
	inline CodeEditor* getCodeEditor() const { return codeEditor; }
	/// Restores the last code made by player for the given unit, or the default unit's code if
	/// player nas not played this unit
	void loadCodeForUnit(Unit* unit);
	/// Starts the compilation process. It is done in background. When the compilation is finished
	/// the @a compilationFinished() signal is emitted.
	void startBuild();

  public slots:
	/// Called when user selects one of the diagnostics in the tools output
	/// @param index The index of the selected diagnostic in the allDiagnostics list
	void diagnosticSelected(int index);
	/// Get access to the breakpoints list set by the user
	QList<GuiBreakpoint*> retrieveBreakpoints();
	/// Called when the visualization changes it state (stopped, starting, running, finished) to
	/// enable or disable visualization control buttons
	void onStateChanged(UnitPlayingState currentState);
	/// Called when visualization has finished in order to clear highlited lines
	void clearAnimation();
	/// Called when an execution thread was updated from GDB in order to update the highlighted line
	void executionThreadUpdated(const ExecutionThread* executionThread);

  signals:
	/// Emitted when user presses the Run/Pause button
	void userRunOrPaused();
	/// Emitted when the compilation and linking process have finished
	/// @param compiler Provides a pointer to the compiler that made the compilation and linking
	/// process. Therefore, the interested objects can get the list of diagnostics generated.
	void buildFinished(Compiler* compiler);
	/// Called when the step into button is pressed
	void userSteppedInto();
	/// Called when the step over button is pressed
	void userSteppedOver();
	/// Called when the step out button is pressed
	void userSteppedOut();
	/// Emitted when the stop button is pressed
	void userStopped();
	/// Emited when user presses over a breakpoint symbol in any code editor window in order to
	/// remove the breakpoint. Visualization controller requires this signal in order to clear
	/// the breakpont in debugger when visualization is running.
	void breakpointAction(GuiBreakpoint* guiBreakpoint);

  protected:
	/// Set up the inner main window
	void setupInnerWidget();
	/// Create standard editing tool bar with undo, copy, run, debug and other actions
	void setupEditingToolbar();
	/// Create a toolbar with buttons for running, stop, step into/out
	void setupRunToolbar();
	/// Create and configure the text editor object
	void setupCodeEditor();
	/// The Run and Pause share the same action for space constraints. This method converts the
	/// action in Run action
	/// @param name Send "Run" or "Resume"
	void setupRunAction(const QString& name, bool enabled);
	/// Converts the Run/Pause action into a Pause action
	void setupPauseAction(bool enabled);

  protected slots:
	/// Called when a new file should be added to the solution: a new header file, or
	/// source file, or C++ class
	void newFileTriggered();
	/// Called when player selects another source file in the file selector combo box
	void fileSelectorIndexChanged(const QString& text);
	/// Called when the player solution has finished to compile and link
	void compilerFinished(CompiledProgram* playerSolutionProgram);
	/// Called when the visualization speed is changed by user
	void visualizationSpeedChanged(int speed);
};

#endif // CODESEGMENT_H
