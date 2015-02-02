#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.h"

class LinearLayout;
class ScenicElement;
class SvgButton;

class GameScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameScene)

  protected:
	/// Button to return to the previous screen
	SvgButton* backButton;
	/// Sprite containing the name of the scene
	ScenicElement* gameTitle;
	/// Button to show info about this scene, e.g: the task description on unit playing scene
	SvgButton* infoButton;
	/// A label showing the title of the scene, e.g: "Training" or "Challenges"
	SvgButton* sceneTitle;
	/// A gauge showing the name of the player, his/her score, and level
//	PlayerStatus* playerStatus;
	/// Gear button to configure the game
	SvgButton* configButton;
	/// Button for accessing the rewards
//	SvgButton* rewardsButton;

  public:
	/// Constructor
	explicit GameScene(const QString& sceneName, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~GameScene();

  protected: // Title bar: menu in game
	/// Creates a standard menu with common buttons: configure, back/exit, player...
	/// Adds it to the scene's layout. If a scene's layout does not exist, it creates
	/// a global vertical linear layout
	void createStandardMenu(const QString& title);

  protected slots:
	/// Called when the Back button is pressed
	virtual void backButtonPressed() = 0;
	/// Called when the Player section is pressed
	void playerStatusPressed();
	/// Called when user press the Information button
	void infoButtonPressed();
	/// Called when user press the Rewards button
	void rewardsButtonPressed();
	/// Called when user press the Config button
	void configButtonPressed();
};

#endif // GAMESCENE_H
