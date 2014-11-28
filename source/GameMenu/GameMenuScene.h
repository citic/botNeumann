#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Scene.h"

class QPushButton;

/**
   @brief The first scene where player chooses the game modality to play
 */
class GameMenuScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameMenuScene)

  protected:
	QPushButton* trainingButton;
	QPushButton* missionsButton;
	QPushButton* collaborationButton;
	QPushButton* createButton;


  public:
	/// Constructor
	explicit GameMenuScene(QObject* parent = nullptr, qreal width = 10.0, qreal height = 10.0);
	/// Destructor
	virtual ~GameMenuScene();

  protected:
	/// Create buttons and layouts
	void setupButtons();

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
