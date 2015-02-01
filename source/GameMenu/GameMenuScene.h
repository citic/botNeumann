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

  protected:
	/// When pressed moves to the Training mode of the game
	SvgButton* trainingButton;
	/// When pressed moves to the Missions mode: difficult challenges, such as ACM contests
	SvgButton* missionsButton;
	/// When pressed moves to the Collaboration mode of the game
	SvgButton* collaborationButton;
	/// When pressed moves to the Create new challenges mode
	SvgButton* createButton;

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

  protected slots:
	/// Called when Training button is pressed
	void trainingPressed();
	/// Called when Missions button is pressed
	void missionsPressed();
	/// Called when Collaboration button is pressed
	void collaborationPressed();
	/// Called when Create button is pressed
	void createPressed();
};

#endif // GAMEMENUSCENE_H
