#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Common.h"
#include "Scene.h"

class LinearLayout;

/**
   @brief The first scene where player chooses the game modality to play
 */
class GameMenuScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameMenuScene)

  public:
	/// Constructor
	explicit GameMenuScene(Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~GameMenuScene();
	/// Get the identification number of this scene
	virtual inline SceneId getSceneId() const override { return sceneGameMenu; }

  protected:
	/// Create buttons and layouts
	void setupButtons(LinearLayout* rightLayout);
	/// Create config buttons
	void setupConfigButtons(LinearLayout* parentLayout);

  protected slots:
	/// Called when Training button is pressed
	void trainingButtonPressed();
	/// Called when Missions button is pressed
	void missionsButtonPressed();
	/// Called when Collaboration button is pressed
	void collaborationButtonPressed();
	/// Called when Create button is pressed
	void createButtonPressed();
	/// Called when user press the Information button
	void infoButtonPressed();
	/// Called when user press the Rewards button
	void rewardsButtonPressed();
	/// Called when user press the Config button
	void configButtonPressed();
};

#endif // GAMEMENUSCENE_H
