#include "BotNeumannDirector.h"
#include "CodeSegment.h"
#include "InfoDialog.h"
#include "MainWindow.h"
#include "Stage.h"

#include <QAction>
#include <QDockWidget>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

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
	restoreSettings();
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

	// A hidden action to reset configuration
	QAction* resetSettingsAction = new QAction(this);
	resetSettingsAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
	connect( resetSettingsAction, SIGNAL(triggered(bool)), this, SLOT(resetSettings()) );
	menuBar()->addAction(resetSettingsAction);
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

void MainWindow::resetSettings()
{
	const QString& text = tr("Do you want to reset the configuration. All players' progress will be deleted");
	if ( QMessageBox::question(this, tr("Settings reset?"), text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes )
	{
		QSettings settings;
		settings.clear();
		settings.sync();
		QMessageBox::warning(this, tr("Settings reset"), tr("Settings were reset. Please restart botNeumann++"));
	}
}
