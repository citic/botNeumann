#include "MainWindow.h"
#include <QDockWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Set basic properties for the main window
	setObjectName("MainWindow"); // used to save preferences
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
	stageRoom->setObjectName("stageRoom");
	setCentralWidget(stageRoom);

	// Remove borders and avoid using scroll bars
	stageRoom->setFrameShape(QFrame::NoFrame);
	stageRoom->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	stageRoom->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// ToDo: replace with actual scene
	stageRoom->setScene(& scene);
	QGraphicsRectItem* rect = scene.addRect(0, 0, 1024, 768);
	rect->setPen(Qt::NoPen);
	rect->setBrush(Qt::NoBrush);
	/*QGraphicsSvgItem* background =*/ new QGraphicsSvgItem(":/game_menu/game_menu/background.svg", rect);
	//scene.addItem(background);
}

// ToDo: remove
#include <QTextEdit>

void MainWindow::setupCodeEditor()
{
	// Source code editor is only visible in playing scenes, disabled by default
	codeEditor = new QDockWidget(this);
	codeEditor->setObjectName("codeEditor");
	codeEditor->setWindowTitle(tr("Program"));
	codeEditor->setVisible(false);
	addDockWidget(Qt::RightDockWidgetArea, codeEditor);

	// ToDo: only for testing: an empty widget
	codeEditor->setWidget(new QTextEdit());
}
