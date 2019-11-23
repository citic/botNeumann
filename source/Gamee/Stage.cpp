#include "Stage.h"
#include "Scene.h"

Stage::Stage(QWidget *parent)
	: QGraphicsView(parent)
{
	// By default remove borders and avoid using scroll bars
	setFrameShape(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setObjectName("stageRoom");
	setScene(&graphicsScene);

  #ifdef BN_NOGAMIFICATION
	setAcceptDrops(true);
  #endif
}

void Stage::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event)
	// Update the graphics scene size
	graphicsScene.setSceneRect(0, 0, width(), height());
	// The main window and this graphics view have changed its size.
	// It has a new size. Tell anybody who is interested
	emit resized(width(), height());
//	if ( scene() ) dynamic_cast<Scene*>(scene())->resize(width(), height());
	// Make the center visible again
//	centerOn(width() * 0.5, height() * 0.5);
}
