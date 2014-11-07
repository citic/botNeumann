#include "GameMenuScene.h"
#include "MainWindow.h"
#include <QDockWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, scene(nullptr)
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

	// The game menu is the default scene
	Q_ASSERT(scene == nullptr);
	scene = new GameMenuScene(this, width(), height());
	stageRoom->setScene(scene);
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

void MainWindow::resizeEvent(QResizeEvent* event)
{
	// The main window changed its size. It has a new size. Update the current scene
	Q_UNUSED(event)
	if ( scene ) scene->resize(width(), height());

	// Make the center visible again
	stageRoom->centerOn(width() * 0.5, height() * 0.5);
}

