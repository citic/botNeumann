#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BaseScene;
class QDockWidget;
class QGraphicsView;

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
	QGraphicsView* stageRoom;
	/// The scene currently shown in the central widget
	/// It changes according to the context. For example: game menu, unit selection,
	BaseScene* scene;
	/// Source code editor is shown as a docking widget
	/// ToDo: implement as a new class inherited from QDockWidget
	QDockWidget* codeEditor;

  public:
	/// Constructor
	explicit MainWindow(QWidget *parent = 0);
	/// Destructor
	virtual ~MainWindow();

  protected:
	/// Create the control where scenes are shown and shows the first scene
	void setupStage();
	/// Create the source code editor control
	void setupCodeEditor();
	/// Called after each time the window is resized
	virtual void resizeEvent(QResizeEvent* event) override;
};

#endif // MAINWINDOW_H
