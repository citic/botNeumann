#ifndef CODEEDITORDOCKWIDGET_H
#define CODEEDITORDOCKWIDGET_H

#include <QDockWidget>

class CodeEditor;
class QAction;
class QMainWindow;
class QSlider;
class Player;
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
class CodeEditorDockWidget : public QDockWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(CodeEditorDockWidget)

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

  public:
	/// Constructor
	explicit CodeEditorDockWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	/// Destructor
	virtual ~CodeEditorDockWidget();
	/// Get access to the code editor
	inline CodeEditor* getCodeEditor() const { return codeEditor; }
	/// Restores the last code made by player for the given unit, or the default unit's code if
	/// player nas not played this unit
	void loadCodeForUnit(Unit* unit);
	/// Called when the player stopped playing the current unit, clear the code editor for being
	/// reused again for a new unit
	void reset();
	/// Get the player's directory to store source files for this unit. E.g: in Mac OS X:
	/// /Users/<user>/Library/Application Support/botNeumann/<player>/<unit>
	static QString getPlayerUnitPath(Player* player, Unit* unit);
	/// Get the full path of a source file in the player's directory for this unit. E.g: in MacOSX:
	/// /Users/<user>/Library/Application Support/botNeumann/<player>/<unit>/main.cpp
	/// @param basename The base name of the attempted C/C++ file, for example, main.cpp. This file
	/// is part of the solution created by the player
	static QString getPlayerUnitSourcePath(Player* player, Unit* unit, const QString& basename);

  protected:
	/// Set up the inner main window
	void setupInnerWidget();
	/// Create and set up the tool bar with undo, copy, run, debug and other actions
	void setupToolbar();
	/// Create and configure the text editor object
	void setupCodeEditor();

  protected slots:
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

#endif // CODEEDITORDOCKWIDGET_H
