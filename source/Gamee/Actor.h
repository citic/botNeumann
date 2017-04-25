#ifndef ACTOR_H
#define ACTOR_H

#include "AlignedItem.h"
#include "Common.h"
#include "LayoutItem.h"

#include <QGraphicsSvgItem>
#include <QStringList>
#include <QTimeLine>

class Layout;

/// Some disabled elements are shown transparent, this is the default opacity
const qreal defaultDisabledOpacity = 0.2;

/**
	@brief Any graphic object that moves or reacts on the scene
	All botNeumann graphics come from SVG files.

	Actors can have other Actors as children. Layouts manage the positioning and
	resizing of children elements.
 */
class Actor : public QGraphicsSvgItem, public LayoutItem, public AlignedItem
{
	Q_OBJECT
	Q_DISABLE_COPY(Actor)

  protected: // Properties to be animated
	/// Manages the position and size of children items of this item. When an item is added to this
	/// item, they become children and they are managed and destroyed by QGraphicsItem class.
	/// Changes applied to this QGraphicsItem, such as position and placing affect the children
	/// But the initial positioning of children and their size, is managed by this layout
	Layout* layout = nullptr;
	/// Controls transitions (eg: changing the face of this actor)
	QTimeLine timeLine;
	/// Stores the faces that this Actor can change while it is doing a transition
	QStringList faces;

  public:
	/// Creates a scenic element with the given SVG ID
	/// @param parentItem The new element will nested as child of this parentItem. Only scenes
	/// can send this parameter null
	/// @param prefixedSvgElementId the id of the object within the SVG to be rendered. If empty,
	/// id can be set later using the setElementId() method. The scene is infered from the first
	/// letters of @a prefixedSvgElementId
	/// @see mapSceneIdFromPrefix() to get the list of supported prefixes
	explicit Actor(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Overloaded constructor that loads a given svg file instead of using a shared renderer
	/// @param loadFile Dummy value to distinguish this constructor from the former
	explicit Actor(const QString& svgFilename, QGraphicsItem* parentItem, bool loadFile);
	/// Destructor
	virtual ~Actor();
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isActor() const override { return true; }
	/// Called on QGraphicItem in order to remove them from the scene
	virtual void removeFromScene() override;
	/// Sets the Z-index provided by layouts to the QGraphicsItem system
	virtual void setZ(qreal zValue) override { setZValue(zValue); }
	/// Set the shared renderer for the given scene
	void setRenderer(SceneId sceneId);
	/// @param prefixedText Any text that begins with some of the following prefixes:
	/// - "ge_" General common scene
	/// - "gm_" Game menu scene
	/// - "us_" Unit selection scene
	/// - "up_" Unit playing scene
	/// @return The mapping to the enumeration, sceneGeneral if not scene is mapped
	static SceneId mapSceneIdFromPrefix(const QString& prefixedText);
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;

  public:
	/// Animates an actor appearing or disappearing from the scene
	/// @return the actual duration of the animation in milliseconds after ajusting to the user
	/// preference in visualization speed slider
	static int animateAppear(QObject* object, int duration = 1000, int initialDelay = 0, qreal fromOpacity = 0.0, qreal toOpacity = 1.0);
	/// Animates this object appearing or disappearing from the scene
	/// @see LayoutItem::animateAppear()
	virtual int animateAppear(int duration = 1000, int initialDelay = 0, qreal fromOpacity = 0.0, qreal toOpacity = 1.0) override;
	/// Convenience method to disappear an actor
	inline int animateDisappear(int duration = 1000, int initialDelay = 0) { return animateAppear(duration, initialDelay, 1.0, 0.0); }
	/// Set the faces that this actor may show during a transition
	/// @param faces An array of element ids, eg: { "robot1_back", "robot1_side", "robot1_front" }
	inline void setFaces(const QStringList& faces) { this->faces = faces; }
	/// Animate a face change of the element using a list of element ids. The animation will have
	/// several transitions as element ids are in the array. Each transition the element id of
	/// the Actor will be changed
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
