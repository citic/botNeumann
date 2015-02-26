#include "CodeEditorDockWidget.h"
#include "MainWindow.h"
#include "Stage.h"
#include "UnitPlayingScene.h"
#include <QDebug>

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_playing", stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
{
	createStandardMenu(context + ' ' + levelUnit, true);
	if ( ! unit.load(filename) )
		qDebug() << "Error: unit not loaded" << filename;
	// else unit.print();

	// Get a pointer to the code editor
	MainWindow* mainWindow = dynamic_cast<MainWindow*>( stage->parent() );
	Q_ASSERT(mainWindow);
	codeEditorDockWidget = mainWindow->getCodeEditorDockWidget();
	Q_ASSERT(codeEditorDockWidget);
}

UnitPlayingScene::~UnitPlayingScene()
{
}

void UnitPlayingScene::startLeavingStage()
{
	codeEditorDockWidget->setVisible(false);
}

void UnitPlayingScene::finishedEnteringStage()
{
	codeEditorDockWidget->setVisible(true);

	// ToDo: Implement player information
	QString code;// = player->codeForUnit(unit.getId());
	if ( code.isEmpty() ) code = unit.getInitialCode();
	codeEditorDockWidget->setCode(code);
}

void UnitPlayingScene::backButtonPressed()
{
	emit showUnitSelectionScene(context, false);
}
