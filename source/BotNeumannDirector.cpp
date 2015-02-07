#include "BotNeumannDirector.h"
#include "GameMenuScene.h"
#include "Stage.h"
#include "TransitionSlide.h"
#include "UnitPlayingScene.h"
#include "UnitSelectionScene.h"

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
	showGameMenuScene();
}

bool BotNeumannDirector::showGameMenuScene()
{
	// If a transition is currently running, we can't replace the scene yet
	if ( isTransitionRunning() ) return false;
	GameMenuScene* newScene = new GameMenuScene(stage);
	connect(newScene, SIGNAL(showUnitSelectionScene(QString,bool)), this, SLOT(showUnitSelectionScene(QString,bool)));
	return replaceScene( newScene, false );
}

bool BotNeumannDirector::showUnitSelectionScene(const QString& context, bool forward)
{
	// If a transition is currently running, we can't replace the scene yet
	if ( isTransitionRunning() ) return false;
	UnitSelectionScene* newScene = new UnitSelectionScene(context, stage);
	connect(newScene, SIGNAL(showGameMenuScene()), this, SLOT(showGameMenuScene()));
	connect(newScene, SIGNAL(showUnitPlayingScene(QString,QString,QString)), this, SLOT(showUnitPlayingScene(QString,QString,QString)));
	return replaceScene( newScene, forward );
}

bool BotNeumannDirector::showUnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename)
{
	// If a transition is currently running, we can't replace the scene yet
	if ( isTransitionRunning() ) return false;
	UnitPlayingScene* newScene = new UnitPlayingScene(context, levelUnit, filename, stage);
	connect(newScene, SIGNAL(showUnitSelectionScene(QString,bool)), this, SLOT(showUnitSelectionScene(QString,bool)));
	return replaceScene( newScene, true );
}

bool BotNeumannDirector::replaceScene(Scene* newScene, bool forward)
{
	// Current scene pass to be the previous scene
	previousScene = currentScene;
	setCurrentScene(newScene);

	// Run a transition to make the scene replacement more obvious
	if ( previousScene == nullptr ) return true;
	auto direction = forward ? TransitionSlide::Direction::left : TransitionSlide::Direction::right;
	Transition* transition = new TransitionSlide(direction, previousScene, currentScene, true, this);
	transition->run(true);

	// The transition will remove the previous scene object
	previousScene = nullptr;
	return true;
}

void BotNeumannDirector::setCurrentScene(Scene* newScene)
{
	// Overwrite the current scene pointer
	this->currentScene = newScene;

	// Each time the stage change its size, the current scene should adapt to the new dimensions
	connect(stage, SIGNAL(resized(qreal,qreal)), currentScene, SLOT(resize(qreal,qreal)));

	// If there is another scene, the new one will not receive the resize event, at least the
	// user resizes the window. So, let the new scene to adjust to the actual size of the window
	if ( previousScene ) currentScene->resize(stage->width(), stage->height());
}
