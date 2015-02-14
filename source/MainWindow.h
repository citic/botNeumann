#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BotNeumannDirector;
class CodeEditorDockWidget;
class Scene;
class Stage;
class QDockWidget;

/**
 * @brief The main window where all the scenes are shown
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow)

  protected:
	/// The place where scenes are performed. It is a graphics view control, able to render svg
	/// actors (images)
	Stage* stage;
	/// Source code editor is shown as a docking widget
	/// ToDo: implement as a new class inherited from QDockWidget
	CodeEditorDockWidget* codeEditor;
	/// Knows what to do when we need change from one scene to another
	BotNeumannDirector* director;

  public:
	/// Constructor
	explicit MainWindow(QWidget* parent = nullptr);
	/// Destructor
	virtual ~MainWindow();
	/// Get access to the CodeEditor, required by UnitPlayingScene
	inline CodeEditorDockWidget* getCodeEditorDockWidget() const { return codeEditor; }

  protected:
	/// Create the control where scenes are shown and shows the first scene
	void setupStage();
	/// Create the source code editor control
	void setupCodeEditor();
	/// Save the window dimensions to be used later
	void saveSettings();
	/// Restore (apply) the saved window dimensions
	void restoreSettings();
};

#endif // MAINWINDOW_H
