#ifndef BOTNEUMANNDIRECTOR_H
#define BOTNEUMANNDIRECTOR_H

#include "Director.h"
#include "Common.h"

class Scene;

class BotNeumannDirector : public Director
{
	Q_OBJECT

  protected:
	/// Current scene being presented
	Scene* currentScene;
	/// The previous scene, used to create transitions between one scene and the next one
	Scene* previousScene;

  public:
	/// Constructor
	explicit BotNeumannDirector(Stage* stage = nullptr, QObject* parent = nullptr);
	/// Destructor
	virtual ~BotNeumannDirector();
	/// Begin the game. Run the first scene
	virtual void begin() override;

  public slots:
	/// Replaces the current scene for a new scene of the given id
	/// A transition is automatically applied, according to the direction of the change
	void replaceScene(SceneId newSceneId);

  protected:
	/// Creates a new Scene object for the given id
	/// @return The scene object or nullptr if id is unknown or not implemented yet
	Scene* createScene(SceneId sceneId);
	/// Set the new active scene, its conections
	void setCurrentScene(Scene* newScene);
};

#endif // BOTNEUMANNDIRECTOR_H
