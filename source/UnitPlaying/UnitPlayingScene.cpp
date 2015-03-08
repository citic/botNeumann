#include "CodeEditorDockWidget.h"
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

	// Set up each component of the process/unit
	createCodeSegment();
	createHeapSegment();
	createCpuCores();
	createDataSegment();
}

UnitPlayingScene::~UnitPlayingScene()
{
	// Layout system deletes heapSegment, cpuCores, and dataSegment
}

void UnitPlayingScene::startLeavingStage()
{
	// Hide the code editor when the scene is leaving the stage
	codeSegment->setVisible(false);
}

void UnitPlayingScene::finishedEnteringStage()
{
	// The scene is ready in the stage after the transition, show the code editor (code segment)
	codeSegment->setVisible(true);

	// ToDo: Implement player information
	QString code;// = player->codeForUnit(unit.getId());
	if ( code.isEmpty() ) code = unit.getInitialCode();
	codeSegment->setCode(code);
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

void UnitPlayingScene::createCodeSegment()
{
	// The Code Editor also behaves as the code segment, it is already created by the main window
	// Get a pointer to it, and store for future use
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);
	codeSegment = mainWindow->getCodeEditorDockWidget();
	Q_ASSERT(codeSegment);
}

void UnitPlayingScene::createHeapSegment()
{
	Q_ASSERT(layout);
	Q_ASSERT(heapSegment == nullptr);
	heapSegment = new HeapSegment(unit.requiresHeapSegment(), unit.getRamSize(), this);
	this->layout->addLayout(heapSegment, 0.38);
}

void UnitPlayingScene::createCpuCores()
{
}

void UnitPlayingScene::createDataSegment()
{
}
