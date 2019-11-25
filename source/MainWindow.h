#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BotNeumannDirector;
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
	/// Knows what to do when we need change from one scene to another
	BotNeumannDirector* director;
	/// The same info dialog is used in several scenes of the game
	InfoDialog* infoDialog;

  public:
	/// Constructor
	explicit MainWindow(QWidget* parent = nullptr);
	/// Destructor
	virtual ~MainWindow() override;
	/// Shows the info dialog with the given information
	void showInfoDialog(const QString& title, const QString& htmlInfo);
	/// Hides the info dialog
	void hideInfoDialog();
	/// Shows or hides the info dialog
	void toggleInfoDialog(const QString& title, const QString& htmlInfo);

  protected:
	/// Create the control where scenes are shown and shows the first scene
	void setupStage();
	/// Create hidden menu options that are activated by special commands
	void setupHiddenActions();
	/// Save the window dimensions to be used later
	void saveSettings();
	/// Restore (apply) the saved window dimensions
	void restoreSettings();
	/// Called when user drags files from a file explorer
	/// Used to accept folders containing programming exercises
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	/// Called when user drops files from a file explorer
	virtual void dropEvent(QDropEvent *event) override;

  protected slots:
	/// Triggered when user presses Cmd+Shift+R to reset the settings
	void resetSettings();
	/// Reveals the directory where logs and user data is stored
	void revealLogDirectory();
	/// Called when user drags and drops an unit from a folder
	void unitDropped(const QString& path);
};

#endif // MAINWINDOW_H
