#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include "Actor.h"

class QGraphicsSimpleTextItem;

/**
	@brief Creates text and graphic push buttons

	A button is compound of the following layers, from top-most to bottom-most:

	|#| Layer      | Type   | Normal           | Pressed           | Hover |
	|-|------------|--------|------------------|-------------------|-------|
	|1| Label      | String | NormalStyle      | PressedStyle      | NA    |
	|2| Foreground | Image  | NormalForeground | PressedForeground | NA    |
	|3| Background | Image  | NormalBackground | PressedBackground | NA    |

	But... QtGraphicsSvgItem does not support to change the image. Therefore only one image can
	be shown while the object exists. For this reason only the NormalBackground image is supported
	and optionally a Label.

	If you need only a label, @see LabelButton class.
 */
class SvgButton : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(SvgButton)

  protected:
	/// The text of the button
	QGraphicsSimpleTextItem* label = nullptr;

  public:
	/// Constructor: creates a graphical button
	/// @param parentItem Mandatory graphic item where this button will be shown
	/// @param text An optional text to be drawn on top of the image (i.e., background image)
	/// @see Actor::Actor()
	explicit SvgButton(const QString& prefixedSvgElementId, QGraphicsItem* parentItem, const QString& text = "");
	/// Destructor
	virtual ~SvgButton();

  signals:
	/// Emitted when this button is pressed
	/// If you need to have a pointer to the SvgButton instance, call sender()
	void pressed();

  public slots:
	/// When disabled, the button becomes semi-transparent
	/// @remarks This method is not virtual at QGraphicsItem
	void setEnabled(bool enabled) /*override*/;

  protected:
	/// Builds the label in top of the button
	void buildLabel(const QString& text);
	/// Overriden to manage click or tap events
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SVGBUTTON_H
