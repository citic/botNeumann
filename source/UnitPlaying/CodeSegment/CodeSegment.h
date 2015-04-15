#ifndef CODESEGMENT_H
#define CODESEGMENT_H

#include <QDockWidget>

class CodeEditor;
class Compiler;
class QAction;
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

  protected:
	/// In order to have a toolbar, the widget of this code editor must be a main window
	QMainWindow* innerMainWindow;
	/// Object where the actual source code is shown and edited
	CodeEditor* codeEditor;
	/// Executes and animates the code or pauses it if already running
	QAction* runOrPauseAction;
	/// If visualisation is paused, executes the next code statement entering in functions if they
	/// are defined by user
	QAction* stepIntoAction;
	/// If visualisation is paused, executes the next code statement in the current function
	QAction* stepOutAction;
	/// Stops the visualisation of the code, if it is running
	QAction* stopAction;
	/// Allows user to set the speed of the visualization
	QSlider* visualizationSpeedSlider;
	/// Manages the list of source files that compound the player's solution to an unit
	PlayerSolution* playerSolution;
	/// Object in charge of compiling the player solution
	Compiler* compiler;

  public:
	/// Constructor
	explicit CodeSegment(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	/// Destructor
	virtual ~CodeSegment();
	/// Get access to the code editor
	inline CodeEditor* getCodeEditor() const { return codeEditor; }
	/// Get access to the compiler and its output
	inline Compiler* getCompiler() const { return compiler; }
	/// Restores the last code made by player for the given unit, or the default unit's code if
	/// player nas not played this unit
	void loadCodeForUnit(Unit* unit);

  signals:
	/// Emitted when the run button is pressed, and therefore it is necessary to clear old output
	void buildStarted();
	/// Emitted when the compilation and linking process have finished
	/// @param compiler Provides a pointer to the compiler that made the compilation and linking
	/// process. Therefore, the interested objects can get the list of diagnostics generated.
	void buildFinished(Compiler* compiler);
	/// Emitted when the visualization has started
	void visualizationStarted();
	/// Emitted when the visualization has finished
	void visualizationFinished();

  protected:
	/// Set up the inner main window
	void setupInnerWidget();
	/// Create and set up the tool bar with undo, copy, run, debug and other actions
	void setupToolbar();
	/// Create and configure the text editor object
	void setupCodeEditor();
	/// Starts the compilation process. It is done in background. When the compilation is finished
	/// the @a compilationFinished() signal is emitted.
	void startBuild();
	/// ToDo:
	void startVisualization();
	/// Called when the Pause button is pressed in order to pause the visualization
	void pauseVisualization();

  protected slots:
	/// Called when the player solution has finished to compile and link
	void compilerFinished();
	// ToDo: Move these slots to an Interpreter class
	/// Called when the run or pause button is pressed
	void runOrPauseTriggered();
	/// Called when the step into button is pressed
	void stepIntoTriggered();
	/// Called when the step out button is pressed
	void stepOutTriggered();
	/// Called when the stop button is pressed
	void stopTriggered();
	/// Called when the visualization speed is changed by user
	void visualizationSpeedChanged(int speed);
};

#endif // CODESEGMENT_H
