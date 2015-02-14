#include "BotNeumannDirector.h"
#include "CodeEditorDockWidget.h"
#include "MainWindow.h"
#include "Stage.h"
#include <QDockWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, stage(nullptr)
	, codeEditor(nullptr)
	, director(nullptr)
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
	// Sometimes settings restore the codeEditor, but it must be invisible at the beginning
	codeEditor->setVisible(false);
}

MainWindow::~MainWindow()
{
	// Store dimensions for next time
	saveSettings();
}

void MainWindow::setupStage()
{
	// Create the stage room and make it the central widget.
	// It will be destroyed by QObject ownership mechanism
	Q_ASSERT(stage == nullptr);
	stage = new Stage(this);
	setCentralWidget(stage);

	// Create the game director
	Q_ASSERT(director == nullptr);
	director = new BotNeumannDirector(stage);
	director->begin();
}

void MainWindow::setupCodeEditor()
{
	// Source code editor is only visible in playing scenes, disabled by default
	Q_ASSERT(codeEditor == nullptr);
	codeEditor = new CodeEditorDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, codeEditor);
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
