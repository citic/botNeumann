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
	// A valid stage is required in order to begin directing
	Q_ASSERT(stage);

	// The game menu is the first scene, but it could different in a future, for example, when
	// resuming the game
	Q_ASSERT(currentScene == nullptr);
	currentScene = new GameMenuScene(stage);

	// Each time the stage change its size, the current scene should adapt to the new dimensions
	connect(stage, SIGNAL(resized(qreal,qreal)), currentScene, SLOT(resize(qreal,qreal)));

	// React when a button asking for replacing the scene for a new one
	connect(currentScene, SIGNAL(newSceneAsked(SceneId)), this, SLOT(replaceScene(SceneId)));
}

#include <QDebug>
void BotNeumannDirector::replaceScene(SceneId newSceneId)
{
	qDebug() << "New scene asked: " << (int)newSceneId;
}
