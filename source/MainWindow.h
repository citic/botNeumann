#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BotNeumannDirector;
class CodeSegment;
class InfoDialog;
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
	CodeSegment* codeSegment;
	/// Knows what to do when we need change from one scene to another
	BotNeumannDirector* director;
	/// The same info dialog is used in several scenes of the game
	InfoDialog* infoDialog;

  public:
	/// Constructor
	explicit MainWindow(QWidget* parent = nullptr);
	/// Destructor
	virtual ~MainWindow();
	/// Get access to the CodeEditor, required by UnitPlayingScene
	inline CodeSegment* getCodeSegment() const { return codeSegment; }
	/// Shows the info dialog with the given information
	void showInfoDialog(const QString& title, const QString& htmlInfo);
	/// Hides the info dialog
	void hideInfoDialog();
	/// Shows or hides the info dialog
	void toggleInfoDialog(const QString& title, const QString& htmlInfo);

  protected:
	/// Create the control where scenes are shown and shows the first scene
	void setupStage();
	/// Create the source code editor control
	void setupCodeSegment();
	/// Save the window dimensions to be used later
	void saveSettings();
	/// Restore (apply) the saved window dimensions
	void restoreSettings();
};

#endif // MAINWINDOW_H
