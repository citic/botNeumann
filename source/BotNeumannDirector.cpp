#include "BotNeumannDirector.h"
#include "GameMenuScene.h"
#include "Stage.h"
#include "TransitionSlide.h"

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
	setCurrentScene( new GameMenuScene(stage) );
}

void BotNeumannDirector::replaceScene(SceneId newSceneId)
{
	// If there are a previous scene, a transition is currently running, wait
	if ( previousScene != nullptr ) return;

	// Create an object for the new asked scene, if not valid, ignore the event
	Scene* newScene = createScene(newSceneId);
	if ( newScene == nullptr ) return;

	// Current scene pass to be the previous scene
	previousScene = currentScene;
	setCurrentScene(newScene);

	// Run a transition to make the scene replacement more obvious
	if ( previousScene == nullptr ) return;
	TransitionSlide::Direction direction = newSceneId > previousScene->getSceneId()
		? TransitionSlide::Direction::left
		: TransitionSlide::Direction::right;
	Transition* transition = new TransitionSlide(direction, previousScene, currentScene, true, this);
	transition->run(true);

	// The transition will remove the previous scene object
	previousScene = nullptr;
}

Scene* BotNeumannDirector::createScene(SceneId sceneId)
{
	switch ( sceneId )
	{
		case SceneId::gameMenu: return new GameMenuScene(stage);
		case SceneId::training: return new GameMenuScene(stage); // TrainingScene(stage);
		//case SceneId::missions: return new MissionsScene(stage);
		//case SceneId::collaboration: return new CollaborationScene(stage);
		//case SceneId::create: return new CreateScene(stage);
		//case SceneId::unitSelection: return new UnitSelectionScene(stage);
		//case SceneId::unitPlaying: return new UnitPlayingScene(stage);
		default: return nullptr;
	}
}

void BotNeumannDirector::setCurrentScene(Scene* newScene)
{
	// Overwrite the current scene pointer
	currentScene = newScene;

	// Each time the stage change its size, the current scene should adapt to the new dimensions
	connect(stage, SIGNAL(resized(qreal,qreal)), currentScene, SLOT(resize(qreal,qreal)));

	// If there is another scene, the new one will not receive the resize event, at least the
	// user resizes the window. So, let the new scene to adjust to the actual size of the window
	if ( previousScene ) currentScene->resize(stage->width(), stage->height());

	// React when a button asking for replacing the scene for a new one
	connect(currentScene, SIGNAL(newSceneAsked(SceneId)), this, SLOT(replaceScene(SceneId)));
}
