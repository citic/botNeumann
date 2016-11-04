#include "CodeSegment.h"
#include "Compiler.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "HeapSegment.h"
#include "MainWindow.h"
#include "MessagesArea.h"
#include "Stage.h"
#include "UnitPlayingScene.h"
#include "Visualizator.h"
#include <QDebug>

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_playing", stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
	, codeSegment(nullptr)
	, heapSegment(nullptr)
	, cpuCores(nullptr)
	, dataSegment(nullptr)
	, messagesArea(nullptr)
	, visualizator(nullptr)
{
	// Create the standar menu with back button, title, code editor toggle, config, and so on
	createStandardMenu(context + ' ' + levelUnit, true);

	// Load the botNeumann unit from the .botnu file
	if ( ! unit.load(filename) )
		qDebug() << "Error: unit not loaded" << filename;

	// Distribute the screen space between each segment according to the number of rows they require
	double heapSegmentProportion = 0.0, cpuCoresProportion = 0.0, dataSegmentProportion;
	distributeScreenSpace(heapSegmentProportion, cpuCoresProportion, dataSegmentProportion);
	// Set up each component of the process/unit
	createHeapSegment(heapSegmentProportion);
	createCpuCores(cpuCoresProportion);
	createDataSegment(dataSegmentProportion);
	createCodeSegment();
	createMessagesArea();
}

UnitPlayingScene::~UnitPlayingScene()
{
	// Layout system deletes heapSegment, cpuCores, and dataSegment
}

void UnitPlayingScene::startLeavingStage()
{
	// The Unit Playing scene is leaving the stage

	// Hide and remove the the code editor
	codeSegment->setVisible(false);
	codeSegment->deleteLater();

	// Hide and remove the messages area
	messagesArea->setVisible(false);
	messagesArea->deleteLater();
}

void UnitPlayingScene::finishedEnteringStage()
{
	// The scene is ready in the stage after the transition

	// Show the code segment and the messages area
	codeSegment->setVisible(true);
	messagesArea->setVisible(true);

	// Loads or restore the code for this unit
	codeSegment->loadCodeForUnit( &unit );
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
	emit showUnitSelectionScene(context, false);
}

void UnitPlayingScene::codeSegmentTogglePressed()
{
	Q_ASSERT(codeSegment);
	codeSegment->setVisible( ! codeSegment->isVisible() );
}

void UnitPlayingScene::distributeScreenSpace(double& heapSegmentProportion, double& cpuCoresProportion, double& dataSegmentProportion)
{
	double heapSegmentRows = unit.getHeapSegmentRows() + 0.5; // 0.5 because the HS interface
	double cpuCoresRows = unit.getStackSegmentVisibleRows() + 1.1; // 1 robot + space
	double dataSegmentRows = unit.getDataSegmentRows() + 0.5; // 0.5 = 0.3 in/out tubes + 0.2 table
	double totalRows = heapSegmentRows + cpuCoresRows + dataSegmentRows;
	double availableProportion = 1.0 - standardMenuProportion;

	heapSegmentProportion = heapSegmentRows / totalRows * availableProportion;
	cpuCoresProportion = cpuCoresRows / totalRows * availableProportion;
	dataSegmentProportion = dataSegmentRows / totalRows * availableProportion;
}

void UnitPlayingScene::createHeapSegment(double heapSegmentProportion)
{
	Q_ASSERT(layout);
	Q_ASSERT(heapSegment == nullptr);
	heapSegment = new HeapSegment(unit, this);
	this->layout->addLayout(heapSegment, heapSegmentProportion);
}

void UnitPlayingScene::createCpuCores(double cpuCoresProportion)
{
	// Cpu cores are represented as available workstations
	Q_ASSERT(layout);
	Q_ASSERT(cpuCores == nullptr);
	cpuCores = new CpuCores(unit, this);
	this->layout->addLayout(cpuCores, cpuCoresProportion * 0.98);

	// Leave a small separation to generate a sensation of space (the floor can be seen)
	this->layout->addStretch(cpuCoresProportion * 0.02);
}

void UnitPlayingScene::createDataSegment(double dataSegmentProportion)
{
	// Cpu cores are represented as available workstations
	Q_ASSERT(layout);
	Q_ASSERT(dataSegment == nullptr);
	dataSegment = new DataSegment(unit, this);
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
	mainWindow->addDockWidget(Qt::RightDockWidgetArea, codeSegment);

	// When the user presses the button Run, his/her solution is compiled and linked. If there were
	// no errors, the simulation should start
	connect( codeSegment, SIGNAL(buildFinished(Compiler*)), this, SLOT(buildFinished(Compiler*)) );
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

	// When the code segment has built a solution and has diagnostics show them in the messages area
	Q_ASSERT(codeSegment);
	connect(codeSegment, SIGNAL(buildFinished(Compiler*)), messagesArea, SLOT(buildFinished(Compiler*)));

	// When user selects a diagnostic in the tools output, point its place in the code
	connect(messagesArea, SIGNAL(diagnosticSelected(int)), codeSegment, SLOT(diagnosticSelected(int)));
}

void UnitPlayingScene::buildFinished(Compiler *compiler)
{
	// If there are errors, do not start the visualization
	if ( compiler->getErrorCount() > 0 )
		return;

	// The player solution generated an executable and we are ready to visualize it
	delete visualizator;
	visualizator = new Visualizator(compiler->getExecutablePath(), this);

	// ToDo: Connect signals emitted by visualizator with each part of the window

	// Start the animation
	visualizator->start();
}
