#include "CodeSegment.h"
#include "CpuCores.h"
#include "DataSegment.h"
#include "HeapSegment.h"
#include "MainWindow.h"
#include "Stage.h"
#include "UnitPlayingScene.h"
#include <QDebug>

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_playing", stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
	, heapSegment(nullptr)
	, cpuCores(nullptr)
	, dataSegment(nullptr)
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
	createCodeSegment();
	createHeapSegment(heapSegmentProportion);
	createCpuCores(cpuCoresProportion);
	createDataSegment(dataSegmentProportion);
}

UnitPlayingScene::~UnitPlayingScene()
{
	// Layout system deletes heapSegment, cpuCores, and dataSegment
}

void UnitPlayingScene::startLeavingStage()
{
	// Hide the code editor when the scene is leaving the stage
	codeSegment->reset();
	codeSegment->setVisible(false);
}

void UnitPlayingScene::finishedEnteringStage()
{
	// The scene is ready in the stage after the transition, show the code editor (code segment)
	codeSegment->setVisible(true);

	// Loads or restore the code for this unit
	codeSegment->loadCodeForUnit( &unit );
}

void UnitPlayingScene::infoButtonPressed()
{
	// Create a dialog showing information about this level
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);
	// ToDo: get active language
	mainWindow->toggleInfoDialog( unit.getId(), unit.getDescription("es") );
}

void UnitPlayingScene::backButtonPressed()
{
	emit showUnitSelectionScene(context, false);
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

void UnitPlayingScene::createCodeSegment()
{
	// The Code Editor also behaves as the code segment, it is already created by the main window
	// Get a pointer to it, and store for future use
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);
	codeSegment = mainWindow->getCodeSegment();
	Q_ASSERT(codeSegment);
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
