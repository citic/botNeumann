#ifndef STAGE_H
#define STAGE_H

#include <QGraphicsView>

/**
   @brief The Stage where scenery is placed and actors play in the game
 */
class Stage : public QGraphicsView
{
	Q_OBJECT

  public:
	/// Constructor
	explicit Stage(QWidget* parent = nullptr);
	/// Creates the stage and plays the given scene
	explicit Stage(QGraphicsScene* scene, QWidget* parent = nullptr);
	/// Called after each time the window is resized
	virtual void resizeEvent(QResizeEvent* event) override;
};

#endif // STAGE_H
