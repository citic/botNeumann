#ifndef SCENICELEMENT_H
#define SCENICELEMENT_H

#include "AlignedItem.h"
#include "Common.h"
#include "LayoutItem.h"

#include <QGraphicsSvgItem>

class Layout;

/// Some disabled elements are shown transparent, this is the default opacity
const qreal defaultDisabledOpacity = 0.2;

/**
	All botNeumann graphics come from SVG files.

	ScenicElements can have other ScenicElements as children. Layouts manage the positioning and
	resizing of children elements.
*/
class ScenicElement : public QGraphicsSvgItem, public LayoutItem, public AlignedItem
{
	Q_OBJECT
	Q_DISABLE_COPY(ScenicElement)

  protected:
	/// Manages the position and size of children items of this item. When an item is added to this
	/// item, they become children and they are managed and destroyed by QGraphicsItem class.
	/// Changes applied to this QGraphicsItem, such as position and placing affect the children
	/// But the initial positioning of children and their size, is managed by this layout
	Layout* layout = nullptr;

  public:
	/// Creates a scenic element with the given SVG ID
	/// @param parentItem The new element will nested as child of this parentItem. Only scenes
	/// can send this parameter null
	/// @param prefixedSvgElementId the id of the object within the SVG to be rendered. If empty,
	/// id can be set later using the setElementId() method. The scene is infered from the first
	/// letters of @a prefixedSvgElementId
	/// @see mapSceneIdFromPrefix() to get the list of supported prefixes
	explicit ScenicElement(const QString& prefixedSvgElementId, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~ScenicElement();
	/// Used to differentiate between pure-layout items and scenic elements
	virtual bool isScenicElement() const override { return true; }
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

  public:
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
};

#endif // SCENICELEMENT_H
