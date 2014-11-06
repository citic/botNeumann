#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
	Q_OBJECT

  private:
	/// Interface design made with QtDesigner
	Ui::MainWindow *ui;
	/// The scene currently shown in the central widget
	/// ToDo: Convert to a hierarchy of scenes, it is a test for now
	QGraphicsScene scene;

  public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();
};

#endif // MAINWINDOW_H
