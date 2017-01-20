#ifndef ACTOR_H
#define ACTOR_H

#include "ScenicElement.h"
#include <QGraphicsSvgItem>

/**
   @brief Any graphic object that moves or reacts on the scene
 */
class Actor : public ScenicElement
{
	Q_OBJECT
	Q_DISABLE_COPY(Actor)
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

  public:
	/// @see ScenicElement::ScenicElement()
	explicit Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Animates this actor appearing or disappearing from the scene
	/// @return the actual duration of the animation in milliseconds after ajusting to the user
	/// preference in visualization speed slider
	int appear(int duration = 1000, qreal fromOpacity = 0.0, qreal toOpacity = 1.0);
	/// Convenience method to disappear an actor
	inline int disappear(int duration = 1000) { return appear(duration, 1.0, 0.0); }
};

#endif // ACTOR_H
