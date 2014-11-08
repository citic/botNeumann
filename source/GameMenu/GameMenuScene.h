#ifndef GAMEMENUSCENE_H
#define GAMEMENUSCENE_H

#include "Scene.h"

/**
   @brief The first scene where player chooses the game modality to play
 */
class GameMenuScene : public Scene
{
	Q_OBJECT
	Q_DISABLE_COPY(GameMenuScene)

  public:
	/// Constructor
	explicit GameMenuScene(QObject *parent = nullptr, qreal width = 10.0, qreal height = 10.0);
	/// Destructor
	virtual ~GameMenuScene();

  signals:

  public slots:
};

#endif // GAMEMENUSCENE_H
