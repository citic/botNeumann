#include "GameMenuScene.h"
#include "MainWindow.h"
#include <QDockWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QSettings>
#include "Stage.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Set basic properties for the main window
	setObjectName("MainWindow"); // used to save preferences
	setWindowTitle("bot Neumann++");
	resize(1024, 768); // affects only desktop applications
	setMinimumSize(480, 320);
  #ifdef Q_OS_LINUX
	setWindowIcon(QIcon(":/game_logo.svg"));
  #endif

	// Create each window section
	setupStage();
	setupCodeEditor();
	restoreSettings();
}

MainWindow::~MainWindow()
{
	// Store dimensions for next time
	saveSettings();
}

void MainWindow::setupStage()
{
	// Create the stage room and make it the central widget
	stage = new Stage(this);
	stage->setObjectName("stageRoom");
	setCentralWidget(stage);

	// The game menu is the default scene
	auto scene = new GameMenuScene(stage, stage->width(), stage->height());
	stage->setScene(scene);
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

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.setValue("MainWindow/Size", size());
	settings.setValue("MainWindow/State", saveState());
}

void MainWindow::restoreSettings()
{
	QSettings settings;
	resize( settings.value("MainWindow/Size", sizeHint()).toSize() );
	restoreState( settings.value("MainWindow/State").toByteArray() );
}

