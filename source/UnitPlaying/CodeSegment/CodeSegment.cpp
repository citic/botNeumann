#include "CodeEditor.h"
#include "CodeSegment.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include "ExecutionThread.h"
#include "LogManager.h"
#include "PlayerSolution.h"
#include "Unit.h"
#include "VisualizationSpeed.h"

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
	runOrPauseAction = new QAction(this);
	setupRunAction("Run", false);
	connect(runOrPauseAction, SIGNAL(triggered()), this, SIGNAL(userRunOrPaused()));
	toolBar->addAction(runOrPauseAction);

	// Create step over action
	stepOverAction = new QAction(QIcon(":/unit_playing/buttons/step_over.svg"), tr("Step over (&Next)"), this);
	stepOverAction->setShortcut(QKeySequence("Ctrl+U"));
	stepOverAction->setToolTip(tr("Step over: run next statement in current function (Ctrl+U)"));
	stepOverAction->setEnabled(false);
	connect(stepOverAction, SIGNAL(triggered()), this, SIGNAL(userSteppedOver()));
	toolBar->addAction(stepOverAction);

	// Create the step into button
	stepIntoAction = new QAction(QIcon(":/unit_playing/buttons/step_into.svg"), tr("Step &into"), this);
	stepIntoAction->setShortcut(QKeySequence("Ctrl+I"));
	stepIntoAction->setToolTip(tr("Step into: run next statement entering in functions (Ctrl+I)"));
	stepIntoAction->setEnabled(false);
	connect(stepIntoAction, SIGNAL(triggered()), this, SIGNAL(userSteppedInto()));
	toolBar->addAction(stepIntoAction);

	// Create the step out button
	stepOutAction = new QAction(QIcon(":/unit_playing/buttons/step_out.svg"), tr("Step &out"), this);
	stepOutAction->setShortcut(QKeySequence("Ctrl+O"));
	stepOutAction->setToolTip(tr("Step out: exit from current function (Ctrl+O)"));
	stepOutAction->setEnabled(false);
	connect(stepOutAction, SIGNAL(triggered()), this, SIGNAL(userSteppedOut()));
	toolBar->addAction(stepOutAction);

	// Create the stop button
	stopAction = new QAction(QIcon(":/unit_playing/buttons/stop.svg"), tr("S&top"), this);
	stopAction->setShortcut(QKeySequence("Ctrl+T"));
	stopAction->setToolTip(tr("Stop the visualization (Ctrl+T)"));
	stopAction->setEnabled(false);
	connect(stopAction, SIGNAL(triggered()), this, SIGNAL(userStopped()));
	toolBar->addAction(stopAction);

	// Create the control that allows user to set the speed of the visualization
	visualizationSpeedSlider = new QSlider(Qt::Horizontal, this);
	visualizationSpeedSlider->setToolTip(tr("Visualization speed: left is slow, right is faster"));
	visualizationSpeedSlider->setMaximum(0);
	visualizationSpeedSlider->setMaximum(200);
	visualizationSpeedSlider->setFocusPolicy(Qt::WheelFocus);
	visualizationSpeedSlider->setTickPosition(QSlider::TicksBelow);
	visualizationSpeedSlider->setTickInterval(20);
	visualizationSpeedSlider->setValue(100);
	visualizationSpeedSlider->setSingleStep(5);
	visualizationSpeedSlider->setPageStep(10);
	connect(visualizationSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(visualizationSpeedChanged(int)));
	toolBar->addWidget(visualizationSpeedSlider);
}

void CodeSegment::setupCodeEditor()
{
	// ToDo: Have several code editors, one for each source file in player's solution
	// The code editor is a QTextEdit object
	codeEditor = new CodeEditor(this);

	// Propagate events to the visualization controller and the debugger
	connect( codeEditor, SIGNAL(breakpointAction(GuiBreakpoint*)), this, SIGNAL(breakpointAction(GuiBreakpoint*)) );

	// Place the code editor as the central widget of this dock widget
	innerMainWindow->setCentralWidget(codeEditor);

	// Do not force users to click the editor in order to start typing on it
	codeEditor->setFocus();
}

void CodeSegment::setupRunAction(const QString& name, bool enabled)
{
	runOrPauseAction->setObjectName(name);
	runOrPauseAction->setIcon( QIcon(":/unit_playing/buttons/run.svg") );
	runOrPauseAction->setToolTip(name == "Run" ? tr("Run: compiles the code and starts the visualization (Ctrl+R)") : tr("Resumes the visualization (Ctrl+R)"));
	runOrPauseAction->setShortcut(QKeySequence("Ctrl+R"));

	runOrPauseAction->setEnabled(enabled);
}

void CodeSegment::setupPauseAction(bool enabled)
{
	runOrPauseAction->setObjectName("Pause");
	runOrPauseAction->setIcon(QIcon(":/unit_playing/buttons/pause.svg"));
	runOrPauseAction->setToolTip(tr("Pauses the visualization (Ctrl+P)"));
	runOrPauseAction->setShortcut(QKeySequence("Ctrl+P"));

	runOrPauseAction->setEnabled(enabled);
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
	codeEditor->setPlayerSolutionForUnit(unit, playerSolution);
	codeEditor->loadInitialFile( lastEditedFilePath.absoluteFilePath() );
	fileSelector->setCurrentText( lastEditedFilePath.fileName() );

	// Now the run button can be triggered
	runOrPauseAction->setEnabled(true);
}

void CodeSegment::startBuild()
{
	// This method must be only called if there is a player solution with files
	Q_ASSERT(playerSolution);
	Q_ASSERT(playerSolution->hasFiles());

	// If there is unsaved changes, save them
	codeEditor->saveChanges();

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
}

void CodeSegment::newFileTriggered()
{
	qCDebug(logNotImplemented) << "New file triggered";
}

void CodeSegment::fileSelectorIndexChanged(const QString& text)
{
	// Get the full path to the filename and load it in the code editor
	codeEditor->loadFile( playerSolution->getPlayerUnitSourcePath(text) );
}

void CodeSegment::visualizationSpeedChanged(int speed)
{
	qCInfo(logPlayer) << "Visualization speed set to " << speed;
	VisualizationSpeed::getInstance().updateSpeed(speed);
}

void CodeSegment::diagnosticSelected(int index)
{
	Q_ASSERT(compiler);
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < compiler->getAllDiagnostics().size());

	// Get the diagnostic that was selected
	Diagnostic* diagnostic = compiler->getAllDiagnostics()[index];

	// If the diagnostic was produced by a file that is not part of the player solution, for
	// example, an external library, do not open the file in the Code Editor
	QFileInfo filePath(diagnostic->getFilename());
	if ( playerSolution->findFileIndex(filePath) == -1 )
		return;

	// The file is part of the solution. Select it in the file selector
	fileSelector->setCurrentText( filePath.fileName() );

	// Place the cursor on the line and column where the diagnostic was detected
	codeEditor->placeCursor( diagnostic->getLine(), diagnostic->getColumn() );
}

QList<GuiBreakpoint*> CodeSegment::retrieveBreakpoints()
{
	// ToDo: for each opened source file (i.e. have several code editors open)
	return codeEditor->retrieveBreakpoints();
}

void CodeSegment::onStateChanged(UnitPlayingState currentState)
{
	// Visualizatio control buttons enable or disable depending on the current state
	bool run = currentState == UnitPlayingState::editing;
	bool resume = currentState == UnitPlayingState::paused;
	bool pause = currentState == UnitPlayingState::animating;
	bool stepOver = currentState == UnitPlayingState::paused;
	bool stepInto = currentState == UnitPlayingState::paused;
	bool stepOut = currentState == UnitPlayingState::paused;
	bool stop = currentState == UnitPlayingState::animating || currentState == UnitPlayingState::paused;

	// Enable actions according to the current state
	stepOverAction->setEnabled(stepOver);
	stepIntoAction->setEnabled(stepInto);
	stepOutAction->setEnabled(stepOut);
	stopAction->setEnabled(stop);

	// Run or pause share the same action
	if ( run )
		setupRunAction("Run", true);
	else if ( resume )
		setupRunAction("Resume", true);
	else if ( pause )
		setupPauseAction(true);
	else
		runOrPauseAction->setEnabled(false);
}

void CodeSegment::clearAnimation()
{
	// Remove all the highlights in code editors
	Q_ASSERT(codeEditor);
	codeEditor->clearHighlights();
}

void CodeSegment::executionThreadUpdated(const ExecutionThread* executionThread)
{
	// Get the file index of the execution thread
	Q_ASSERT(playerSolution);

	// If we have to clear the previous highlighted line
	if ( executionThread->getPreviousLineNumber() > 0 )
	{
		// Clear the previous highlighted line only if this is the shown file
		int fileIndex = playerSolution->findFileIndex( executionThread->getPreviousFilename() );
		if ( fileIndex >= 0 && fileIndex == fileSelector->currentIndex() )
			codeEditor->clearHighlight( executionThread->getPreviousLineNumber(), executionThread->getHighlightColor(), false );
	}

	// If we have to highlight the line being executed
	if ( executionThread->getLineNumber() > 0 )
	{
		// Clear the previous highlighted line only if this is the shown file
		int fileIndex = playerSolution->findFileIndex( executionThread->getFilename() );
		if ( fileIndex >= 0 && fileIndex == fileSelector->currentIndex() )
			codeEditor->addHighlight( executionThread->getLineNumber(), executionThread->getHighlightColor(), true );
	}
}
