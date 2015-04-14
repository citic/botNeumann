#include "BotNeumannDirector.h"
#include "CodeSegment.h"
#include "InfoDialog.h"
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
	, codeSegment(nullptr)
	, director(nullptr)
	, infoDialog(nullptr)
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
	setupCodeSegment();
	restoreSettings();
	// Sometimes settings restore the codeEditor, but it must be invisible at the beginning
	codeSegment->setVisible(false);
}

MainWindow::~MainWindow()
{
	// Store dimensions for next time
	saveSettings();
}

void MainWindow::showInfoDialog(const QString& title, const QString& htmlInfo)
{
	if ( ! infoDialog )
		infoDialog = new InfoDialog(title, htmlInfo, this);
	infoDialog->show();
}

void MainWindow::hideInfoDialog()
{
	if ( infoDialog )
		infoDialog->hide();
}

void MainWindow::toggleInfoDialog(const QString& title, const QString& htmlInfo)
{
	if ( ! infoDialog )
		return showInfoDialog(title, htmlInfo);
	infoDialog->setInfo(title, htmlInfo);
	infoDialog->isVisible() ? infoDialog->hide() : infoDialog->show();
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

void MainWindow::setupCodeSegment()
{
	// Source code editor is only visible in playing scenes, disabled by default
	Q_ASSERT(codeSegment == nullptr);
	codeSegment = new CodeSegment(this);
	addDockWidget(Qt::RightDockWidgetArea, codeSegment);
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
