#ifndef STAGE_H
#define STAGE_H

#include <QGraphicsView>

#include "GraphicsScene.h"

/**
	@brief The Stage where scenery is placed and actors play in the game

	Do not use the inherited setScene() method from QGraphicsView. Inherit a class from Director.
	Director instance will know how to organize the scenes of your game.
 */
class Stage : public QGraphicsView
{
	Q_OBJECT
	Q_DISABLE_COPY(Stage)

  protected:
	/// The Graphics scene where all the Gamee Scenes are shown
	GraphicsScene graphicsScene;

  public:
	/// Constructor
	explicit Stage(QWidget* parent = nullptr);
	/// Called after each time the window is resized
	virtual void resizeEvent(QResizeEvent* event) override;
	/// Provide access to the graphics scene
	inline QGraphicsScene& getGraphicsScene() { return graphicsScene; }

  signals:
	/// Emitted each time this graphics view has been resized
	void resized(qreal width, qreal height);
};

#endif // STAGE_H
