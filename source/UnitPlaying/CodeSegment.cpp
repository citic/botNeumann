#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "CodeSegment.h"
#include "Player.h"
#include "Unit.h"
#include <QAction>
#include <QMainWindow>
#include <QSlider>
#include <QToolBar>

CodeSegment::CodeSegment(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
	, innerMainWindow( new QMainWindow(this) )
{
	setObjectName("codeEditor");

	setupInnerWidget();
	setupToolbar();
	setupCodeEditor();
}

CodeSegment::~CodeSegment()
{
}

void CodeSegment::setupInnerWidget()
{
	// Ask the main window to behave as normal widget, instead of a real main window
	innerMainWindow->setWindowFlags(Qt::Widget);

	// Set the inner main window as the inner widget of this dock widget
	setWidget(innerMainWindow);
}

void CodeSegment::setupToolbar()
{
	// Create the toolbar
	QToolBar* toolBar = innerMainWindow->addToolBar("code");

	// Create the Run or pause action
	runOrPauseAction = new QAction(QIcon(":/unit_playing/unit_playing/button_run.svg"), tr("&Run or pause"), this);
	runOrPauseAction->setShortcut(QKeySequence("Ctrl+R"));
	runOrPauseAction->setStatusTip(tr("Compiles the code and starts its visualization"));
	runOrPauseAction->setEnabled(false);
	connect(runOrPauseAction, SIGNAL(triggered()), this, SLOT(runOrPauseTriggered()));
	toolBar->addAction(runOrPauseAction);

	// Create the step into button
	stepIntoAction = new QAction(QIcon(":/unit_playing/unit_playing/button_step_into.svg"), tr("Step &into"), this);
	stepIntoAction->setShortcut(QKeySequence("Ctrl+I"));
	stepIntoAction->setStatusTip(tr("Executes next statement entering in functions"));
	stepIntoAction->setEnabled(false);
	connect(stepIntoAction, SIGNAL(triggered()), this, SLOT(stepIntoTriggered()));
	toolBar->addAction(stepIntoAction);

/* Svg missing for the moment
	// Create the step out button
	stepOutAction = new QAction(QIcon(":/unit_playing/unit_playing/button_step_out.svg"), tr("Step &out"), this);
	stepOutAction->setShortcut(QKeySequence("Ctrl+O"));
	stepOutAction->setStatusTip(tr("Executes next statement in current function"));
	stepOutAction->setEnabled(false);
	connect(stepOutAction, SIGNAL(triggered()), this, SLOT(stepOutTriggered()));
	toolBar->addAction(stepOutAction);
*/
	// Create the stop button
	stopAction = new QAction(QIcon(":/unit_playing/unit_playing/button_stop.svg"), tr("S&top"), this);
	stopAction->setShortcut(QKeySequence("Ctrl+T"));
	stopAction->setStatusTip(tr("Compiles the code and starts its visualization"));
	stopAction->setEnabled(false);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopTriggered()));
	toolBar->addAction(stopAction);

	// Create the control that allows user to set the speed of the visualization
	visualizationSpeedSlider = new QSlider(Qt::Horizontal, this);
	visualizationSpeedSlider->setToolTip(tr("Visualization speed"));
	visualizationSpeedSlider->setFocusPolicy(Qt::WheelFocus);
	visualizationSpeedSlider->setTickPosition(QSlider::TicksBelow);
	visualizationSpeedSlider->setTickInterval(20);
	visualizationSpeedSlider->setValue(50);
	visualizationSpeedSlider->setSingleStep(1);
	visualizationSpeedSlider->setPageStep(10);
	connect(visualizationSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(visualizationSpeedChanged(int)));
	toolBar->addWidget(visualizationSpeedSlider);
}

void CodeSegment::setupCodeEditor()
{
	// The code editor is a QTextEdit object
	codeEditor = new CodeEditor(this);

	// Place the code editor as the central widget of this dock widget
	innerMainWindow->setCentralWidget(codeEditor);
}

void CodeSegment::loadCodeForUnit(Unit* unit)
{
	// Close files from previous units
	sourceFiles.clear();

	// ToDo: load the list of files from the player's source
	// ToDo: implement a PlayerSolution class in charge of this task

	// ToDo: load from player's preferences the last edited source file for this unit
	// for this version, only main.cpp is assumed
	Player* player = BotNeumannApp::getInstance()->getCurrentPlayer();
	const QString& lastEditedFilepath = getPlayerUnitSourcePath(player, unit, "main.cpp");

	sourceFiles.append(lastEditedFilepath);

	// Ask the editor to show this source
	codeEditor->loadCodeForUnit(unit, lastEditedFilepath);

	// Now the run button can be triggered
	runOrPauseAction->setEnabled(true);
}

void CodeSegment::reset()
{
	codeEditor->reset();
}

QString CodeSegment::getPlayerUnitPath(Player* player, Unit* unit)
{
	return player->getLocalDataPath() + '/' + unit->getId();
}

QString CodeSegment::getPlayerUnitSourcePath(Player* player, Unit* unit, const QString& basename)
{
	return getPlayerUnitPath(player, unit) + '/' + basename;
}

#include <QDebug>
#include "Compiler.h"
#include "Diagnostic.h"

void CodeSegment::runOrPauseTriggered()
{
	// If there are not files, ignore the call
	if ( sourceFiles.size() <= 0 ) return;

	// Compile and run the player solution
	// ToDo: playerSolution.compileAndRun();

	Compiler compiler;
	bool canBeRun = compiler.compile(sourceFiles[0]);

	// Show diagnostics in terminal
	const QList<Diagnostic*>& diagnostics = compiler.getDiagnostics();
	for ( int i = 0; i < diagnostics.size(); ++i )
	{
		const Diagnostic* diagnostic = diagnostics[i];
		qCritical() << diagnostic->getSeverityText()
					<< diagnostic->getLine() << ':' << diagnostic->getColumn()
					<< "::" << diagnostic->getMessage();
	}

	if ( canBeRun ) qDebug() << "Code can be run/interpreted";
		// interpret code
}

void CodeSegment::stepIntoTriggered()
{
	qDebug() << "Step into triggered";
}

void CodeSegment::stepOutTriggered()
{
	qDebug() << "Step out triggered";
}

void CodeSegment::stopTriggered()
{
	qDebug() << "Stop triggered";
}

void CodeSegment::visualizationSpeedChanged(int speed)
{
	qDebug() << "Visualization speed" << speed;
}
