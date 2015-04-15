#include "CodeSegment.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "HeapSegment.h"
#include "MainWindow.h"
#include "MessagesDockWidget.h"
#include "Stage.h"
#include "UnitPlayingScene.h"
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
	, messagesDockWidget(nullptr)
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
	createMessagesDockWidget();
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
	messagesDockWidget->setVisible(false);
	messagesDockWidget->deleteLater();
}

void UnitPlayingScene::finishedEnteringStage()
{
	// The scene is ready in the stage after the transition

	// Show the code segment and the messages area
	codeSegment->setVisible(true);
	messagesDockWidget->setVisible(true);

	// Loads or restore the code for this unit
	codeSegment->loadCodeForUnit( &unit );
}

void UnitPlayingScene::infoButtonPressed()
{
	// ToDo: get active language
	Q_ASSERT(messagesDockWidget);
	messagesDockWidget->setVisible( ! messagesDockWidget->isVisible() );
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
}

void UnitPlayingScene::createMessagesDockWidget()
{
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);

	// Create a dock widget for the messages
	messagesDockWidget = new MessagesDockWidget(mainWindow);
	messagesDockWidget->setVisible(false);
	messagesDockWidget->setUnitDescription( unit.getDescription("es"), false );
	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, messagesDockWidget);

	// When the code segment has built a solution and has diagnostics show them in the messages area
	Q_ASSERT(codeSegment);
	connect(codeSegment, SIGNAL(buildFinished(Compiler*)), messagesDockWidget, SLOT(buildFinished(Compiler*)));
}
