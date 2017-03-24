#include "BotNeumannDirector.h"
#include "CodeSegment.h"
#include "InfoDialog.h"
#include "MainWindow.h"
#include "Stage.h"

#include <QAction>
#include <QDesktopServices>
#include <QDockWidget>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, stage(nullptr)
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
	setupHiddenActions();
	restoreSettings();
}

MainWindow::~MainWindow()
{
	// Store dimensions for next time
	saveSettings();
	delete director;
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

void MainWindow::setupHiddenActions()
{
	// A hidden action to reset configuration
	QAction* resetSettingsAction = new QAction(this);
	resetSettingsAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
	connect( resetSettingsAction, SIGNAL(triggered(bool)), this, SLOT(resetSettings()) );
	menuBar()->addAction(resetSettingsAction);

	// A hidden action to reveal the directory where logs and user data is stored
	QAction* openLogDirAction = new QAction(this);
	openLogDirAction->setShortcut(QKeySequence("Ctrl+Shift+L"));
	connect( openLogDirAction, SIGNAL(triggered(bool)), this, SLOT(revealLogDirectory()) );
	menuBar()->addAction(openLogDirAction);
}

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.setValue(sk("MainWindow/Size"), size());
	settings.setValue(sk("MainWindow/State"), saveState());
}

void MainWindow::restoreSettings()
{
	QSettings settings;
	resize( settings.value(sk("MainWindow/Size"), sizeHint()).toSize() );
	restoreState( settings.value(sk("MainWindow/State")).toByteArray() );
}

void MainWindow::resetSettings()
{
	const QString& text = tr("Do you want to reset the configuration. All players' progress will be deleted");
	if ( QMessageBox::question(this, tr("Settings reset?"), text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes )
	{
		QSettings().setValue(sk("Application/SettingsReset"), true);
		QMessageBox::warning(this, tr("Settings reset"), tr("Please restart botNeumann++ for the changes to take effect"));
	}
}

void MainWindow::revealLogDirectory()
{
	const QString& path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDesktopServices::openUrl(QUrl("file://" + path, QUrl::TolerantMode));
}
