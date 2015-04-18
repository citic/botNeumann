#include "CodeEditor.h"
#include "CodeSegment.h"
#include "Compiler.h"
#include "PlayerSolution.h"
#include "Unit.h"
#include <QAction>
#include <QComboBox>
#include <QMainWindow>
#include <QSlider>
#include <QToolBar>

// Default width and height of the tools in toolbars
const int toolBarIconSize = 18;

CodeSegment::CodeSegment(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
	, innerMainWindow( new QMainWindow(this) )
	, codeEditor(nullptr)
	, playerSolution(nullptr)
	, compiler(nullptr)
{
	setObjectName("codeSegment");

	setupInnerWidget();
	setupCodeEditor(); // It must be called before the toolbars
	setupEditingToolbar();
	innerMainWindow->addToolBarBreak();
	setupRunToolbar();
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

void CodeSegment::setupEditingToolbar()
{
	// Create the toolbar
	QToolBar* editToolBar = innerMainWindow->addToolBar(tr("Edit"));
	editToolBar->setIconSize( QSize(toolBarIconSize, toolBarIconSize) );

	// Create new files in the solution
	newFileAction = new QAction(QIcon(":/unit_playing/buttons/new_file.svg"), tr("&New file"), this);
	newFileAction->setObjectName("newFile");
	newFileAction->setShortcut(QKeySequence("Ctrl+N"));
	newFileAction->setStatusTip(tr("Adds a new file to the solution"));
	connect(newFileAction, SIGNAL(triggered()), this, SLOT(newFileTriggered()));
	editToolBar->addAction(newFileAction);

	// Undo
	undoAction = new QAction(QIcon(":/unit_playing/buttons/undo.svg"), tr("&Undo"), this);
	undoAction->setObjectName("undo");
	undoAction->setEnabled(false);
	undoAction->setShortcut(QKeySequence("Ctrl+Z"));
	undoAction->setStatusTip(tr("Undoes the last action done in the editor"));
	connect(codeEditor, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));
	connect(undoAction, SIGNAL(triggered()), codeEditor, SLOT(undo()));
	editToolBar->addAction(undoAction);

	// Redo
	redoAction = new QAction(QIcon(":/unit_playing/buttons/redo.svg"), tr("&Redo"), this);
	redoAction->setObjectName("redo");
	redoAction->setEnabled(false);
	redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));
	redoAction->setStatusTip(tr("Redoes the last undone action in the editor"));
	connect(codeEditor, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));
	connect(redoAction, SIGNAL(triggered()), codeEditor, SLOT(redo()));
	editToolBar->addAction(redoAction);

	// Cut
	cutAction = new QAction(QIcon(":/unit_playing/buttons/cut.svg"), tr("C&ut"), this);
	cutAction->setObjectName("cut");
	cutAction->setEnabled(false);
	cutAction->setShortcut(QKeySequence("Ctrl+X"));
	cutAction->setStatusTip(tr("Moves the selection to the clipboard"));
	connect(codeEditor, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
	connect(cutAction, SIGNAL(triggered()), codeEditor, SLOT(cut()));
	editToolBar->addAction(cutAction);

	// Copy
	copyAction = new QAction(QIcon(":/unit_playing/buttons/copy.svg"), tr("&Copy"), this);
	copyAction->setObjectName("copy");
	copyAction->setEnabled(false);
	copyAction->setShortcut(QKeySequence("Ctrl+C"));
	copyAction->setStatusTip(tr("Copies the selection to the clipboard"));
	connect(codeEditor, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));
	connect(copyAction, SIGNAL(triggered()), codeEditor, SLOT(copy()));
	editToolBar->addAction(copyAction);

	// Paste
	pasteAction = new QAction(QIcon(":/unit_playing/buttons/paste.svg"), tr("&Paste"), this);
	pasteAction->setObjectName("paste");
	pasteAction->setEnabled(false);
	pasteAction->setShortcut(QKeySequence("Ctrl+V"));
	pasteAction->setStatusTip(tr("Pastes the clipboard contents over the selection"));
	connect(pasteAction, SIGNAL(triggered()), codeEditor, SLOT(paste()));
	editToolBar->addAction(pasteAction);

	// A combo box to show and choose the active file being edited
	fileSelector = new QComboBox();
	connect(fileSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(fileSelectorIndexChanged(QString)));
	editToolBar->addWidget(fileSelector);
}

void CodeSegment::setupRunToolbar()
{
	// Create the toolbar
	QToolBar* toolBar = innerMainWindow->addToolBar(tr("Run"));
	toolBar->setIconSize( QSize(toolBarIconSize, toolBarIconSize) );

	// Create the Run or pause action
	runOrPauseAction = new QAction(QIcon(":/unit_playing/buttons/run.svg"), tr("&Run or pause"), this);
	runOrPauseAction->setObjectName("Run");
	runOrPauseAction->setShortcut(QKeySequence("Ctrl+R"));
	runOrPauseAction->setStatusTip(tr("Compiles the code and starts its visualization"));
	runOrPauseAction->setEnabled(false);
	connect(runOrPauseAction, SIGNAL(triggered()), this, SLOT(runOrPauseTriggered()));
	toolBar->addAction(runOrPauseAction);

	// Create the step into button
	stepIntoAction = new QAction(QIcon(":/unit_playing/buttons/step_into.svg"), tr("Step &into"), this);
	stepIntoAction->setShortcut(QKeySequence("Ctrl+I"));
	stepIntoAction->setStatusTip(tr("Executes next statement entering in functions"));
	stepIntoAction->setEnabled(false);
	connect(stepIntoAction, SIGNAL(triggered()), this, SLOT(stepIntoTriggered()));
	toolBar->addAction(stepIntoAction);

	// Create the step out button
	stepOutAction = new QAction(QIcon(":/unit_playing/buttons/step_out.svg"), tr("Step &out"), this);
	stepOutAction->setShortcut(QKeySequence("Ctrl+O"));
	stepOutAction->setStatusTip(tr("Executes next statement in current function"));
	stepOutAction->setEnabled(false);
	connect(stepOutAction, SIGNAL(triggered()), this, SLOT(stepOutTriggered()));
	toolBar->addAction(stepOutAction);

	// Create the stop button
	stopAction = new QAction(QIcon(":/unit_playing/buttons/stop.svg"), tr("S&top"), this);
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

	// Fill the file selector with the list of files that comprise this player solution
	fileSelector->clear();
	fileSelector->addItems( playerSolution->getSourceNames() );

	// Ask the editor to show this source
	const QFileInfo& lastEditedFilePath = playerSolution->getLastEditedFilePath();
	codeEditor->loadFile(unit, lastEditedFilePath.absoluteFilePath() );
	fileSelector->setCurrentText( lastEditedFilePath.fileName() );

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
	compiler->compile(playerSolution->getSourcePaths(), playerSolution->getExecutablePath());
}

void CodeSegment::compilerFinished()
{
	// Get the compiler and its results
	Q_ASSERT(compiler);

	// Alert other object the compilation process finished, for example, show the generated
	// diagnostics in the tools' output on messages area
	emit buildFinished(compiler);

	// If there are not errors, the player's solution can be run. Start the visualization
	if ( compiler->getErrorCount() == 0 )
	{
		startVisualization();
		runOrPauseAction->setObjectName("Pause");
		runOrPauseAction->setIcon(QIcon(":/unit_playing/buttons/pause.svg"));
		runOrPauseAction->setStatusTip(tr("Pauses the visualization"));
		runOrPauseAction->setEnabled(false);
	}
	else
		runOrPauseAction->setEnabled(true);
}

void CodeSegment::startVisualization()
{
	// ToDo: Call the debugger here

	// If it is successful, enable the stop button
	stopAction->setEnabled(true);
}

void CodeSegment::pauseVisualization()
{
	// Pause the visualization code here
}

void CodeSegment::newFileTriggered()
{
	qDebug() << "New file triggered";
}

void CodeSegment::fileSelectorIndexChanged(const QString& text)
{
	qDebug() << "New source file selected" << text;
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
