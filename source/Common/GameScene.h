#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Common.h"
#include "Scene.h"

class LabelButton;
class LinearLayout;
class Actor;
class SvgButton;

/// The standard menu will require this percent of vertical space on the scene
const double defaultStandardMenuProportion = 64.754 / 768;

class GameScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameScene)

  protected:
	/// Button to return to the previous screen
	SvgButton* backButton = nullptr;
	/// Sprite containing the name of the scene
	Actor* gameTitle = nullptr;
	/// Button to show info about this scene, e.g: the task description on unit playing scene
	SvgButton* infoButton = nullptr;
	/// A label showing the title of the scene, e.g: "Training" or "Challenges"
	LabelButton* sceneTitle = nullptr;
	/// A gauge showing the name of the player, his/her score, and level
//	PlayerStatus* playerStatus;
	/// Gear button to configure the game
	SvgButton* configButton = nullptr;
	/// Button for accessing the rewards
//	SvgButton* rewardsButton;
	/// When pressed, shows or hiddes the code editor
	SvgButton* codeEditorToggle = nullptr;

  public:
	/// Constructor
	explicit GameScene(const QString& sceneName, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~GameScene();

  protected: // Title bar: menu in game
	/// Creates a standard menu with common buttons: configure, back/exit, player...
	/// Adds it to the scene's layout. If a scene's layout does not exist, it creates
	/// a global vertical linear layout
	void createStandardMenu(const QString& title, bool enableCodeEditorToggle, double verticalProportion = defaultStandardMenuProportion);

  protected slots:
	/// Called when the Back button is pressed
	virtual void backButtonPressed() = 0;
	/// Called when the Player section is pressed
	void playerStatusPressed();
	/// Called when user press the Information button
	virtual void infoButtonPressed();
	/// Called when user press the Rewards button
	void rewardsButtonPressed();
	/// Called when user press the Config button
	void configButtonPressed();
	/// Called when user press the code editor toggle button
	virtual void codeSegmentTogglePressed();
};

#endif // GAMESCENE_H
