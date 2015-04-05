#ifndef CODEEDITORDOCKWIDGET_H
#define CODEEDITORDOCKWIDGET_H

#include <QDockWidget>

class QAction;
class QMainWindow;
class QSlider;
class QTextEdit;
class SyntaxHighlighter;

class CodeEditorDockWidget : public QDockWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(CodeEditorDockWidget)

  protected:
	/// In order to have a toolbar, the widget of this code editor must be a main window
	QMainWindow* innerMainWindow;
	/// Object where the actual source code is shown and edited
	QTextEdit* codeEditor;
	/// Store formatting rules for C++
	SyntaxHighlighter* highlighter;
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
	inline QTextEdit* getCodeEditor() const { return codeEditor; }
	/// Set the give code in the editor
	void setCode(const QString& code);

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
