#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Common.h"
#include "Scene.h"

class LinearLayout;
class Player;
class SvgButton;

/**
   @brief The first scene where player chooses the game modality to play
 */
class GameMenuScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameMenuScene)

  protected:
	/// Game mode buttons
	SvgButton* trainingButton;
	SvgButton* missionsButton;
	SvgButton* collaborationButton;
	SvgButton* createButton;

  public:
	/// Constructor
	explicit GameMenuScene(Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~GameMenuScene();

  protected:
	/// Create buttons and layouts
	void setupButtons(LinearLayout* rightLayout);
	/// Create config buttons
	void setupConfigButtons(LinearLayout* parentLayout);

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);

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
	/// Called when there is a player change. For example when a new player is created
	void playerChanged(Player* newPlayer);
};

#endif // GAMEMENUSCENE_H
