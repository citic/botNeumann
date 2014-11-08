#include "Stage.h"
#include "Scene.h"

Stage::Stage(QWidget *parent)
	: QGraphicsView(parent)
{
	// By default remove borders and avoid using scroll bars
	setFrameShape(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

Stage::Stage(QGraphicsScene* scene, QWidget* parent)
	: QGraphicsView(scene, parent)
{
}

void Stage::resizeEvent(QResizeEvent* event)
{
	// The main window changed its size. It has a new size. Update the current scene
	Q_UNUSED(event)
	if ( scene() ) dynamic_cast<Scene*>(scene())->resize(width(), height());

	// Make the center visible again
	centerOn(width() * 0.5, height() * 0.5);
}
