#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Scene.h"

class LinearLayout;
class SvgButton;

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
	virtual inline SceneId getSceneId() const override { return SceneId::gameMenu; }

  protected:
	/// Create buttons and layouts
	void setupButtons(LinearLayout* rightLayout);
	/// Create config buttons
	void setupConfigButtons(LinearLayout* parentLayout);

  protected slots:
	/// Called when Training button is pressed
	void trainingPressed();
	/// Called when Missions button is pressed
	void missionsPressed();
	/// Called when Collaboration button is pressed
	void collaborationPressed();
	/// Called when Create button is pressed
	void createPressed();
	/// Called when user press the Information button
	void infoPressed();
	/// Called when user press the Rewards button
	void rewardsPressed();
	/// Called when user press the Config button
	void configPressed();
};

#endif // GAMEMENUSCENE_H
