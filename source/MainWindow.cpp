#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDockWidget>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Set basic properties for the main window
	setWindowTitle("bot Neumann++");
	resize(1024, 768); // affects only desktop applications
	setMinimumSize(480, 320);

	// Create each window section
	setupStage();
	setupCodeEditor();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupStage()
{
	// Create the stage room and make it the central widget
	stageRoom = new QGraphicsView(this);
	setCentralWidget(stageRoom);

	// Remove borders and avoid using scroll bars
	stageRoom->setFrameShape(QFrame::NoFrame);
	stageRoom->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	stageRoom->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// ToDo: replace with actual scene
	stageRoom->setScene(& scene);
}

// ToDo: remove
#include <QTextEdit>

void MainWindow::setupCodeEditor()
{
	// Source code editor is only visible in playing scenes, disabled by default
	codeEditor = new QDockWidget(this);
	codeEditor->setWindowTitle(tr("Program"));
	//dockCodeEditor->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, codeEditor);

	// ToDo: only for testing: an empty widget
	codeEditor->setWidget(new QTextEdit());
}
