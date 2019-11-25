#ifndef BOTNEUMANNDIRECTOR_H
#define BOTNEUMANNDIRECTOR_H

#include "Director.h"
#include "Common.h"

class QFileInfo;
class Scene;

class BotNeumannDirector : public Director
{
	Q_OBJECT
	Q_DISABLE_COPY(BotNeumannDirector)

  protected:
	/// Current scene being presented
	Scene* currentScene;
	/// The previous scene, used to create transitions between one scene and the next one
	Scene* previousScene;

  public:
	/// Constructor
	explicit BotNeumannDirector(Stage* stage = nullptr, QObject* parent = nullptr);
	/// Destructor
	virtual ~BotNeumannDirector() override;
	/// Begin the game. Run the first scene
	virtual void begin() override;
	/// Returns true if a transition is running
	inline bool isTransitionRunning() const { return previousScene != nullptr; }

  public slots:
	/// Shows the game menu scene and replaces the previous scene by a transition, if any
	/// @return true on success, false if a transition is taking place
	bool showGameMenuScene();
	/// Shows the scene where player selects the unit to play
	/// @param context One of the following strings: "training", "challenges", "collaboration"
	/// @param forward Decides the direction of the transition. Send true if moving from game menu
	/// and send false if moving from playing scene
	/// @return true on success, false if a transition is taking place
	bool showUnitSelectionScene(const QString& context, bool forward);
	/// Shows the scene where player can solve problems by building code and visualizing it running
	/// @param context One of the following strings: "training", "challenges", "collaboration"
	/// @param levelUnit The number of the level and unit separated by a dash, e.g: "2-14"
	/// @param filename The full resource path of the .botnu unit with details of the unit
	/// @return true on success, false if a transition is taking place
	bool showUnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename);
	/// Shows the scene where player can solve problems, but using a default empty level
	/// @return true
	bool showFolderDropUnitPlayingScene();
	/// Shows the scene where player can solve problems, but using a default empty level
	/// @return true
	bool showFolderDropUnitPlayingScene(const QFileInfo& dir);
	/// Shows the scene where player can solve problems using the given parameters
	/// @return true
	bool showFolderDropUnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename);

  protected:
	/// Replaces the current scene for the new given scene
	/// @param forward Decides the direction of the transition. Send true if moving from game menu
	/// and send false if moving from playing scene
	/// @return true on success, false if the scene cannot be changed
	bool replaceScene(Scene* newScene, bool forward);
	/// Set the new active scene, its conections
	void setCurrentScene(Scene* newScene);
	/// Reloads the last scene saved in settings
	bool showLastScene();

  protected slots:
	/// Called when a scene transition is done
	void sceneTransitionDone() { previousScene = nullptr; }
};

#endif // BOTNEUMANNDIRECTOR_H
