#include "UnitPlayingScene.h"

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_playing", stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
{
	createStandardMenu(context + ' ' + levelUnit);
}

UnitPlayingScene::~UnitPlayingScene()
{
}

void UnitPlayingScene::backButtonPressed()
{
	emit showUnitSelectionScene(context, false);
}
