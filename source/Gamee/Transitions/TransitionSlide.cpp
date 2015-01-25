#include "TransitionSlide.h"
#include "Scene.h"

TransitionSlide::TransitionSlide(Direction direction, Scene* previousScene, Scene* nextScene, bool deletePreviousScene, QObject *parent)
	: Transition(previousScene, nextScene, deletePreviousScene, parent)
	, direction(direction)
{
	const QRectF& rect = previousScene->boundingRect();
	switch ( direction )
	{
		case Direction::top: nextScene->setPos(0.0, rect.height()); break;
		case Direction::right: nextScene->setPos(- rect.width(), 0.0); break;
		case Direction::bottom: nextScene->setPos(0.0, - rect.height()); break;
		case Direction::left: nextScene->setPos(rect.width(), 0.0); break;
		default: Q_ASSERT(false);
	}
}

TransitionSlide::~TransitionSlide()
{
}

void TransitionSlide::animate(int frame)
{
	double completed = double(frame) / timeLine.endFrame();
	const QRectF& rect = previousScene->boundingRect();
	switch ( direction )
	{
		case Direction::top:
			previousScene->setPos(0.0, - completed * rect.height());
			nextScene->setPos(0.0, rect.height() * (1 - completed));
			break;

		case Direction::right:
			previousScene->setPos(completed * rect.width(), 0.0);
			nextScene->setPos(- rect.width() * (1 - completed), 0.0);
			break;

		case Direction::bottom:
			previousScene->setPos(0.0, completed * rect.height());
			nextScene->setPos(0.0, - rect.height() * (1 - completed));
			break;

		case Direction::left:
			previousScene->setPos(- completed * rect.width(), 0.0);
			nextScene->setPos(rect.width() * (1 - completed), 0.0);
			break;

		default:
			Q_ASSERT(false);
	}
}
