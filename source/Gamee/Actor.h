#ifndef ACTOR_H
#define ACTOR_H

#include "ScenicElement.h"

/**
   @brief Any graphic object that moves or reacts on the scene
 */
class Actor : public ScenicElement
{
	Q_OBJECT
	Q_DISABLE_COPY(Actor)

  public:
	/// Constructor
	explicit Actor(QGraphicsItem* parentItem = 0);
	/// Creates an actor with the given svg filename
	explicit Actor(const QString& fileName, QGraphicsItem* parentItem = nullptr);
};

#endif // ACTOR_H
