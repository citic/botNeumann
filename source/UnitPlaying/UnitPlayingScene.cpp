#include "UnitPlayingScene.h"
#include <QDebug>

UnitPlayingScene::UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_playing", stage, parent)
	, context(context)
	, levelUnit(levelUnit)
	, filename(filename)
{
	createStandardMenu(context + ' ' + levelUnit);
	if ( ! unit.load(filename) )
		qDebug() << "Error: unit not loaded" << filename;
	else
		unit.print();
}

UnitPlayingScene::~UnitPlayingScene()
{
}

void UnitPlayingScene::backButtonPressed()
{
	emit showUnitSelectionScene(context, false);
}
