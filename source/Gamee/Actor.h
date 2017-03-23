#ifndef ACTOR_H
#define ACTOR_H

#include "ScenicElement.h"

#include <QGraphicsSvgItem>
#include <QStringList>
#include <QTimeLine>

/**
   @brief Any graphic object that moves or reacts on the scene
 */
class Actor : public ScenicElement
{
	Q_OBJECT
	Q_DISABLE_COPY(Actor)

  protected: // Properties to be animated
	/// Controls transitions (eg: changing the face of this actor)
	QTimeLine timeLine;
	/// Stores the faces that this Actor can change while it is doing a transition
	QStringList faces;

  public:
	/// @see ScenicElement::ScenicElement()
	explicit Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Animates this actor appearing or disappearing from the scene
	/// @return the actual duration of the animation in milliseconds after ajusting to the user
	/// preference in visualization speed slider
	int appear(int duration = 1000, qreal fromOpacity = 0.0, qreal toOpacity = 1.0, int initialDelay = 0);
	/// Convenience method to disappear an actor
	inline int disappear(int duration = 1000, int initialDelay = 0) { return appear(duration, 1.0, 0.0, initialDelay); }
	/// Set the faces that this actor may show during a transition
	/// @param faces An array of element ids, eg: { "robot1_back", "robot1_side", "robot1_front" }
	inline void setFaces(const QStringList& faces) { this->faces = faces; }
	/// Animate a face change of the element using a list of element ids. The animation will have
	/// several transitions as element ids are in the array. Each transition the element id of
	/// the ScenicElement will be changed
	/// @param faces An array of element ids, eg: { "robot1_back", "robot1_side", "robot1_front" }
	/// @param forward, true to transition faces in incremental order, false in backward order
	/// @return the actual duration of the animation in milliseconds after ajusting to the user
	/// preference in visualization speed slider
	int transitionFaces(const QStringList& faces, QTimeLine::Direction direction = QTimeLine::Forward, int duration = 750);
	/// Animate a face change using the @a faces member list that was previously set with the
	/// @a setFaces() method
	int transitionFaces(QTimeLine::Direction direction = QTimeLine::Forward, int duration = 750);

  protected slots:
	/// Called each time the face of the actor must change (element id property)
	void changeFace(int frame);
};

#endif // ACTOR_H
