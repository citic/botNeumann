#ifndef BOTNEUMANNDIRECTOR_H
#define BOTNEUMANNDIRECTOR_H

#include "Director.h"

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
	BotNeumannDirector(Stage* stage = nullptr, QObject* parent = nullptr);
	/// Destructor
	virtual ~BotNeumannDirector();
	/// Begin the game. Run the first scene
	virtual void begin() override;
};

#endif // BOTNEUMANNDIRECTOR_H
