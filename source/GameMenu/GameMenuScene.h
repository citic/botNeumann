#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Scene.h"

class QGraphicsLinearLayout;
class QGraphicsWidget;
class QPushButton;

/**
   @brief The first scene where player chooses the game modality to play
 */
class GameMenuScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameMenuScene)

  protected:
	/// When pressed moves to the Training mode of the game
	QPushButton* trainingButton;
	/// When pressed moves to the Missions mode: difficult challenges, such as ACM contests
	QPushButton* missionsButton;
	/// When pressed moves to the Collaboration mode of the game
	QPushButton* collaborationButton;
	/// When pressed moves to the Create new challenges mode
	QPushButton* createButton;
	/// GraphicsScene requires a GraphicsWidget to place the buttons on it
	QGraphicsWidget* graphicsWidget;
	/// The global layout contains the other layouts
	QGraphicsLinearLayout* globalLayout;

  public:
	/// Constructor
	explicit GameMenuScene(QObject* parent = nullptr, qreal width = 1024.0, qreal height = 768.0);
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
	/// Call this method when the stage room has changed its dimensions
	virtual void resize(qreal width, qreal height) override;
};

#endif // GAMEMENUSCENE_H
