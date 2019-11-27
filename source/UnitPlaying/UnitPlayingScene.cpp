#include "CodeSegment.h"
#include "CompiledProgram.h"
#include "Compiler.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "ExecutionThread.h"
#include "HeapSegment.h"
#include "LogManager.h"
#include "MainWindow.h"
#include "MessagesArea.h"
#include "PlayerSolution.h"
#include "Stage.h"
#include "TestCaseManager.h"
#include "UnitPlayingScene.h"
#include "Visualizator.h"

#include <QSettings>
#include <QTime>
#include <QTimer>

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene(mapSceneName(sceneUnitPlaying), stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
{
	setBackground("up_background");

  #ifdef BN_LAYOUT_TIMING
	QTime creationTime;
	creationTime.start();
  #endif

	// We extract some vertical space of the game menu to place the test cases control keeping an
	// golden ratio (aurea) where the standard menu has the longer height and the test cases the shorter
	const double standardMenuReduction = 0.61803398874988; // 1 / aurea
	const double standardMenuProportion = defaultStandardMenuProportion * standardMenuReduction;

	// Create the standar menu with back button, title, code editor toggle, config, and so on
	createStandardMenu(context + ' ' + levelUnit, true, standardMenuProportion );

	// Create the tube representing the test cases
	testCaseManager = new TestCaseManager(this);
	this->layout->addItem(testCaseManager, (1.0 - standardMenuReduction) * defaultStandardMenuProportion);

	// Load the botNeumann unit from the .botnu file
	if ( ! unit.load(filename) )
		qCCritical(logApplication) << "Unit not loaded:" << filename;

	// Create objects for the vertical segments
	heapSegment = new HeapSegment(unit, this);
	cpuCores = new CpuCores(unit, this);
	dataSegment = new DataSegment(unit, this);

	// After the vertical segments are created, they can calculate their height, therefore we can
	// distribute the screen space between each segment according to the number of rows they require
	addVerticalSegments();

	// Create the docking segments
	createCodeSegment();
	createMessagesArea();

  #ifdef BN_LAYOUT_TIMING
	qDebug() << "Unit playing constructor time: " << creationTime.elapsed() / 1000.0 << "s";
	QTimer::singleShot(0, this, [creationTime]() { qCInfo(logApplication, "Unit playing scene creation time: %lfs", creationTime.elapsed() / 1000.0); } );
  #endif

  #if GAMIFICATION
	// This is the last scene loaded
	QSettings settings;
	settings.setValue( sk("Application/LastScene"), sceneName );
	settings.setValue( sk("Application/LastContext"), context );
	settings.setValue( sk("Application/LastLevelUnit"), levelUnit );
	settings.setValue( sk("Application/LastUnitFilename"), filename );
  #endif
}

UnitPlayingScene::~UnitPlayingScene()
{
	// Nothing to do: Layout system deletes heapSegment, cpuCores, and dataSegment
}

void UnitPlayingScene::startLeavingStage()
{
	// The Unit Playing scene is leaving the stage

	// Code segment and messages area were hidden when the back button was pressed,
	// just release their memory
	Q_ASSERT(codeSegment);
	Q_ASSERT(messagesArea);
	codeSegment->deleteLater();
	messagesArea->deleteLater();
}

void UnitPlayingScene::finishedEnteringStage()
{
	// The scene is ready in the stage after the transition

	// Show the code segment and the messages area
	QSettings settings;
	codeSegment->setVisible( settings.value(sk("CodeSegment/Visible"), true).toBool() );
	messagesArea->setVisible( settings.value(sk("MessagesArea/Visible"), true).toBool() );

	// If there is an old player solution, replace it
	delete playerSolution;
	playerSolution = new PlayerSolution(this);

	// When the player's source code has eventually been compiled:
	// The messages area lists the warnings and errors (diagnostics)
	connect( playerSolution, SIGNAL(playerSolutionBuilt(CompiledProgram*)), messagesArea, SLOT(playerSolutionBuilt(CompiledProgram*)) );
	// The code segment may require to highlight erroneous lines
	connect( playerSolution, SIGNAL(playerSolutionBuilt(CompiledProgram*)), codeSegment, SLOT(playerSolutionBuilt(CompiledProgram*)) );
	// We do not wait until the test cases are generated to start the visualization
	connect( playerSolution, SIGNAL(playerSolutionBuilt(CompiledProgram*)), this, SLOT(playerSolutionBuilt(CompiledProgram*)) );

	// When ALSO all test cases were generated, run player solution against them
	testCaseManager->setPlayerSolution(playerSolution);
	connect( playerSolution, SIGNAL(allBuilt()), testCaseManager, SLOT(testPlayerSolution()) );
	connect( playerSolution, SIGNAL(allBuilt()), this, SLOT(loadStandardOutput()) );

	// When user wants to change the active test case, we have to stop current visualization and
	// start a new one for the selected test case
	connect( testCaseManager, SIGNAL(activeTestCaseChanged(int)), this, SLOT(activeTestCaseChanged(int)) );

	// Load the source file list that compounds the player's solution
	playerSolution->loadSolutionForUnit( &unit );

	// Loads or restore the code for this unit
	codeSegment->loadPlayerCodeForUnit( playerSolution, &unit );
}

QList<GuiBreakpoint*> UnitPlayingScene::retrieveBreakpoints()
{
	return codeSegment->retrieveBreakpoints();
}

void UnitPlayingScene::infoButtonPressed()
{
	// ToDo: get active language
	Q_ASSERT(messagesArea);
	messagesArea->setVisible( ! messagesArea->isVisible() );
}

void UnitPlayingScene::backButtonPressed()
{
	Q_ASSERT(codeSegment);
	Q_ASSERT(messagesArea);

	// Keep visibility preferences of the user for future use
	QSettings settings;
	settings.setValue(sk("CodeSegment/Visible"), codeSegment->isVisible());
	settings.setValue(sk("MessagesArea/Visible"), messagesArea->isVisible());

	codeSegment->setVisible(false);
	messagesArea->setVisible(false);

	// We use a timer to wait until the docks (code segment and messages area) have been completely
	// hidden to allow the QGraphicsView widget to resize before starting the transition to the unit
	// selection scene
	QTimer::singleShot( 0, this, SLOT(callUnitSelectionScene()) );
}

void UnitPlayingScene::callUnitSelectionScene()
{
	emit showUnitSelectionScene(context, false);
}

void UnitPlayingScene::codeSegmentTogglePressed()
{
	Q_ASSERT(codeSegment);
	codeSegment->setVisible( ! codeSegment->isVisible() );
}

void UnitPlayingScene::addVerticalSegments()
{
	Q_ASSERT(heapSegment && cpuCores && dataSegment);

	double heapSegmentRows = heapSegment->getHeightInRows();
	double cpuCoresRows = cpuCores->getHeightInRows();
	double dataSegmentRows = dataSegment->getHeightInRows();
	double totalRows = heapSegmentRows + cpuCoresRows + dataSegmentRows;
	double availableProportion = 1.0 - defaultStandardMenuProportion;

	double heapSegmentProportion = heapSegmentRows / totalRows * availableProportion;
	double cpuCoresProportion = cpuCoresRows / totalRows * availableProportion;
	double dataSegmentProportion = dataSegmentRows / totalRows * availableProportion;

	Q_ASSERT(layout);
	this->layout->addLayout(heapSegment, heapSegmentProportion);
	this->layout->addLayout(cpuCores, cpuCoresProportion);
	// Leave a small separation to generate a sensation of space (the floor can be seen)
	this->layout->addLayout(dataSegment, dataSegmentProportion);
}

void UnitPlayingScene::createCodeSegment()
{
	// The Code Editor also behaves as the code segment, it is already created by the main window
	// Get a pointer to it, and store for future use
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);

	// Source code editor is only visible in playing scenes, disabled by default
	Q_ASSERT(codeSegment == nullptr);
	codeSegment = new CodeSegment(mainWindow);
	codeSegment->setVisible(false);
	mainWindow->addDockWidget(Qt::LeftDockWidgetArea, codeSegment);

	// When the user presses the button Run, his/her solution is compiled and linked. If there were
	// no errors, the simulation should start
	connect( this, SIGNAL(stateChanged(UnitPlayingState)), codeSegment, SLOT(onStateChanged(UnitPlayingState)) );
	connect( codeSegment, SIGNAL(userRunOrPaused()), this, SLOT(userRunOrPaused()) );
	connect( codeSegment, SIGNAL(userStopped()), this, SLOT(userStopped()) );

	// When GDB reports a change on some execution thread, update its highlighted line
	Q_ASSERT(cpuCores);
	connect( cpuCores, SIGNAL(executionThreadUpdated(ExecutionThread*,int&)), codeSegment, SLOT(executionThreadUpdated(ExecutionThread*,int&)) );
}

void UnitPlayingScene::createMessagesArea()
{
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);

	// Create a dock widget for the messages
	messagesArea = new MessagesArea(mainWindow);
	messagesArea->setVisible(false);
	messagesArea->setUnitDescription( unit.getDescription("es"), false );
	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, messagesArea);

	// When user selects a diagnostic in the tools output, point its place in the code
	connect(messagesArea, SIGNAL(diagnosticSelected(int)), codeSegment, SLOT(diagnosticSelected(int)));
}

void UnitPlayingScene::changeState(UnitPlayingState newState)
{
	if ( this->state != newState )
	{
		this->state = newState;
		emit stateChanged(state);
	}
}

void UnitPlayingScene::updateStandardInputOutput(const GdbItemTree& tree, VisualizationContext context, int& maxDuration)
{
	QList<ExecutionThread*> inputQueue;
	QList<ExecutionThread*> outputQueue;
	cpuCores->getThreadsWaitingForIO(inputQueue, outputQueue);
	int bytes = dataSegment->updateStandardInputOutput(tree, context, inputQueue, outputQueue, maxDuration);
	if ( bytes > 0 )
		messagesArea->updateStandardInputOutput(bytes, context, maxDuration);
}

void UnitPlayingScene::userRunOrPaused()
{
	// The Run/Pause produces several actions depending on the current state
	if ( state == UnitPlayingState::editing )
	{
		// App is in state editing code, we have to build the player solution and start animation
		changeState(UnitPlayingState::building);
		playerSolution->logSolution("Run");
		buildAll();
	}
	else if ( state == UnitPlayingState::animating )
	{
		// Animation is in progress, pause it
		if ( visualizator->pause() )
			changeState(UnitPlayingState::paused);
	}
	else if ( state == UnitPlayingState::paused )
	{
		// Animation is paused, resume it
		if ( visualizator->resume() )
			changeState(UnitPlayingState::animating);
	}
	else
	{
		Q_ASSERT(false); // Run/Pause button not allowed under other state
	}
}

bool UnitPlayingScene::buildAll()
{
	// Remove potential old diagnostics in messages area
	messagesArea->clear();

	// We start an asychronous process of building the player solution in several steps
	// In each completed step, playerSolution will emit a signal and some other objects
	// awaits for these signals
	Q_ASSERT(playerSolution);
	return playerSolution->buildAll();
}

void UnitPlayingScene::playerSolutionBuilt(CompiledProgram* compiledProgram)
{
	// If there are errors, do not start the visualization
	if ( compiledProgram->getCompiler()->getErrorCount() > 0 )
		return changeState(UnitPlayingState::editing);

	changeState(UnitPlayingState::starting);

	// By default, the first test case (the example) is always animated. If user wants another,
	// s/he can press it later
	startVisualization();
}

void UnitPlayingScene::startVisualization(int testCaseNumber)
{
	// If called with 0, it is first time this visualization is called
	Q_ASSERT(testCaseNumber >= 0);
	bool firstTime = testCaseNumber == 0;
	if ( firstTime )
		testCaseNumber = 1;

	// The player solution generated an executable and we are ready to visualize it, or a new
	// test case was actived and the previuous visualization is finished
	visualizator->deleteLater();
	visualizator = new Visualizator(playerSolution, testCaseNumber, this);
	Q_ASSERT(visualizator);

	// When user creates or removes breakpoints and visualization is running, update them
	connect( codeSegment, SIGNAL(breakpointAction(GuiBreakpoint*)), visualizator, SLOT(breakpointAction(GuiBreakpoint*)) );

	// When animation finished, reflect the result in the test case manager
	Q_ASSERT(testCaseManager);
	connect( visualizator, &Visualizator::playerSolutionFinished, testCaseManager, &TestCaseManager::playerSolutionFinished );

	// When user asks to step forward
	connect( codeSegment, SIGNAL(userSteppedForward()), visualizator, SLOT(stepForward()) );

	// Start the animation, if it started change the state to animating, otherwise return to editing
	if ( ! visualizator->start() )
		return changeState(UnitPlayingState::editing);

	// Load the test case expected input in the StandardInputOutputInspector
	dataSegment->loadTestCase(testCaseNumber, playerSolution, true, firstTime == false);
	messagesArea->loadTestCase(testCaseNumber, playerSolution, true, firstTime == false);
	changeState(UnitPlayingState::animating);
}

void UnitPlayingScene::loadStandardOutput()
{
    // Only if built was successful
    Q_ASSERT(playerSolution);
    Q_ASSERT(playerSolution->getPlayerSolutionProgram());

    if ( playerSolution->getPlayerSolutionProgram()->getErrorCount() != 0 )
        return;

	// After the player solution has generated all output for all test cases, load the output
	// in the visualization
	Q_ASSERT(visualizator);
	int testCaseNumber = visualizator->getTestCaseNumber();
	dataSegment->loadTestCase(testCaseNumber, playerSolution, false, true);
	messagesArea->loadTestCase(testCaseNumber, playerSolution, false, true);
}

void UnitPlayingScene::userStopped(bool removeTestCases)
{
	// Stop the animation
	Q_ASSERT(visualizator);
	if ( visualizator->stop() )
	{
		// Tell the segments to remove animation artifacts
		if (removeTestCases) testCaseManager->clearAnimation();
		codeSegment->clearAnimation();
		heapSegment->clearAnimation();
		cpuCores->clearAnimation();
		dataSegment->clearAnimation();

		// Return to editing state
		changeState(UnitPlayingState::editing);
	}
}

void UnitPlayingScene::activeTestCaseChanged(int newTestCaseNumber)
{
	// Stop current visualization
	userStopped(false);

	// Start a new visualization for the new active test case
	startVisualization(newTestCaseNumber);
}
