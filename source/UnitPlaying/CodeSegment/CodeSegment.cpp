#include "CodeEditor.h"
#include "CodeSegment.h"
#include "Compiler.h"
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
	, compiler(nullptr)
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
	runOrPauseAction->setObjectName("Run");
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

#include <QDebug>

void CodeSegment::runOrPauseTriggered()
{
	runOrPauseAction->objectName() == "Run" ? startBuild() : pauseVisualization();
}

void CodeSegment::startBuild()
{
	// This method must be only called if there is a player solution with files
	Q_ASSERT(playerSolution);
	Q_ASSERT( playerSolution->hasFiles() );

	// Avoid to call two or more times to the compiler
	runOrPauseAction->setEnabled(false);

	// If there is unsaved changes, save them
	codeEditor->saveChanges();

	// Alert other objects that a new compilation process has begun
	emit buildStarted();

	// ToDo: If there is an active compiling process, stop it
	//if ( compiler && compiler->isRunning() ) compiler->stop();
	if ( compiler ) compiler->deleteLater();

	// Create an object in charge of compiling the solution files
	compiler = new Compiler(this);

	// Compile and run the player solution. It requires some time and we do not wait until it
	// finishes. When the compilation and linking process has finished, our compilerFinished()
	// is called and we continue processing the results there
	connect(compiler, SIGNAL(finished()), this, SLOT(compilerFinished()));

	// Start the compiling process with the files in the solution and the expected executable file
	compiler->compile(playerSolution->getSourceFiles(), playerSolution->getExecutablePath());
}

void CodeSegment::startVisualization()
{
	// ToDo: Call the debugger here

	// If it is successful, enable the stop button
	stopAction->setEnabled(true);
}

void CodeSegment::compilerFinished()
{
	// Get the compiler and its results
	Q_ASSERT(compiler);

	// Alert other object the compilation process finished
	emit buildFinished(compiler);

	// Show diagnostics in messages output

/*
	const QList<Diagnostic*>& diagnostics = compiler->getDiagnostics();
	for ( int i = 0; i < diagnostics.size(); ++i )
	{
		const Diagnostic* diagnostic = diagnostics[i];
		qCritical() << diagnostic->getSeverityText()
					<< diagnostic->getLine() << ':' << diagnostic->getColumn()
					<< "::" << diagnostic->getMessage();
	}
	emit compilationFinished();
*/
	qDebug("Compilation finished: %i error(s), %i warning(s)", compiler->getErrorCount(), compiler->getWarningCount());

	// If there are not errors, the player's solution can be run. Start the visualization
	if ( compiler->getErrorCount() == 0 )
	{
		startVisualization();
		runOrPauseAction->setObjectName("Pause");
		runOrPauseAction->setIcon(QIcon(":/unit_playing/unit_playing/button_pause.svg"));
		runOrPauseAction->setStatusTip(tr("Pauses the visualization"));
		runOrPauseAction->setEnabled(false);
	}
	else
		runOrPauseAction->setEnabled(true);
}

void CodeSegment::pauseVisualization()
{
	// Pause the visualization code here
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
