#ifndef PROP_H
#define PROP_H

#include "Actor.h"

/** Inanimated objects in the interface. This class may be deleted because it only
	gives another name to Actor. */
class Prop : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(Prop)

  public:
	/// @see Actor::Actor()
	explicit Prop(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~Prop();
};

#endif // PROP_H
