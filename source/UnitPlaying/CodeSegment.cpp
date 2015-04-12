#include "CodeEditor.h"
#include "CodeSegment.h"
#include "PlayerSolution.h"
#include "Unit.h"
#include <QAction>
#include <QMainWindow>
#include <QSlider>
#include <QToolBar>

CodeSegment::CodeSegment(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
	, innerMainWindow( new QMainWindow(this) )
	, playerSolution(nullptr)
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

	// Create the step out button
	stepOutAction = new QAction(QIcon(":/unit_playing/unit_playing/button_step_out.svg"), tr("Step &out"), this);
	stepOutAction->setShortcut(QKeySequence("Ctrl+O"));
	stepOutAction->setStatusTip(tr("Executes next statement in current function"));
	stepOutAction->setEnabled(false);
	connect(stepOutAction, SIGNAL(triggered()), this, SLOT(stepOutTriggered()));
	toolBar->addAction(stepOutAction);

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
	// If there is an old player solution, replace it
	delete playerSolution;
	playerSolution = new PlayerSolution(this);

	// Load the source file list that compounds the player's solution
	playerSolution->loadSolutionForUnit(unit);

	// Ask the editor to show this source
	codeEditor->loadFile(unit, playerSolution->getLastEditedFilepath());

	// Now the run button can be triggered
	runOrPauseAction->setEnabled(true);
}

void CodeSegment::reset()
{
	codeEditor->reset();
}

#include <QDebug>

void CodeSegment::runOrPauseTriggered()
{
	// Compile and run the player solution
	Q_ASSERT(playerSolution);
	bool canBeRun = playerSolution->compile();

	// ToDo: run logic
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
