#ifndef PROP_H
#define PROP_H

#include "ScenicElement.h"

/** Inanimated objects in the interface. This class may be deleted because it only
	gives another name to ScenicElement. */
class Prop : public ScenicElement
{
	Q_OBJECT
	Q_DISABLE_COPY(Prop)

  public:
	/// @see ScenicElement::ScenicElement()
	explicit Prop(SceneId sceneId, const QString& svgElementId, QGraphicsItem* parentItem);
	/// @see ScenicElement::ScenicElement()
	explicit Prop(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~Prop();
};

#endif // PROP_H
