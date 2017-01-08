#ifndef ACTOR_H
#define ACTOR_H

#include "ScenicElement.h"
#include <QGraphicsSvgItem>

/**
   @brief Any graphic object that moves or reacts on the scene
 */
class Actor : public ScenicElement<QGraphicsSvgItem>
{
	Q_OBJECT
	Q_DISABLE_COPY(Actor)
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

  public:
	/// Constructor
	explicit Actor(QGraphicsItem* parentItem);
	/// Creates an actor with the given svg filename
	explicit Actor(const QString& fileName, QGraphicsItem* parentItem);
	/// Animates this actor appearing or disappearing from the scene
	void appear(qreal fromOpacity = 0.0, qreal toOpacity = 1.0);
};

#endif // ACTOR_H
