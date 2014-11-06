#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

class QDockWidget;
class QGraphicsView;

/**
 * @brief The main window where all the scenes are shown
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

  protected:
	/// The place where scenes are performed. It is a graphics view control, able to render svg
	/// actors (images)
	QGraphicsView* stageRoom;
	/// The scene currently shown in the central widget
	/// It changes according to the context. For example: game menu, unit selection,
	/// ToDo: Convert to a hierarchy of scenes, it is a test for now
	QGraphicsScene scene;
	/// Source code editor is shown as a docking widget
	/// ToDo: implement as a new class inherited from QDockWidget
	QDockWidget* codeEditor;

  public:
	/// Constructor
	explicit MainWindow(QWidget *parent = 0);
	/// Destructor
	virtual ~MainWindow();

  protected:
	void setupStage();
	void setupCodeEditor();
};

#endif // MAINWINDOW_H
