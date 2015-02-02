#include "UnitSelectionScene.h"

UnitSelectionScene::UnitSelectionScene(const QString& context, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_selection", stage, parent)
	, context(context)
{
	createStandardMenu(context);
}

UnitSelectionScene::~UnitSelectionScene()
{
}

void UnitSelectionScene::backButtonPressed()
{
	emit newSceneAsked(SceneId::gameMenu);
}
