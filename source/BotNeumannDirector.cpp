#include "BotNeumannDirector.h"
#include "GameMenuScene.h"
#include "Stage.h"

BotNeumannDirector::BotNeumannDirector(Stage* stage, QObject* parent)
	: Director(stage, parent)
	, currentScene(nullptr)
	, previousScene(nullptr)
{
}

BotNeumannDirector::~BotNeumannDirector()
{
}

void BotNeumannDirector::begin()
{
	// The game menu is the first scene, but it could different in a future, for example, when
	// resuming the game
	Q_ASSERT(currentScene == nullptr);
	currentScene = new GameMenuScene(stage);
	connect(stage, SIGNAL(resized(qreal,qreal)), currentScene, SLOT(resize(qreal,qreal)));

//	connect(scene, SIGNAL(), this, SLOT());
}
