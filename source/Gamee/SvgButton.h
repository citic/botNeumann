#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include "ScenicElement.h"
#include <QGraphicsSvgItem>

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
class SvgButton : public ScenicElement<QGraphicsSvgItem>
{
	Q_OBJECT
	Q_DISABLE_COPY(SvgButton)

  protected:
	/// The text of the button
	QGraphicsSimpleTextItem* label;

  public:
	/// Constructor: creates a graphical button
	/// @param imageFileName Mandatory file name of the image to be used as button
	/// @param parentItem Mandatory graphic item where this button will be shown
	/// @param label An optional text to be drawn on top of the image (i.e., background image)
	explicit SvgButton(const QString& imageFileName, QGraphicsItem* parentItem, const QString& label = "");
	/// Destructor
	virtual ~SvgButton();
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;

  signals:
	/// Emitted when this button is pressed
	/// If you need to have a pointer to the SvgButton instance, call sender()
	void pressed();

  public slots:
	/// When disabled, the button becomes semi-transparent
	/// @remarks This method is not virtual at QGraphicsItem
	void setEnabled(bool enabled) /*override*/;

  protected:
	/// Overriden to manage click or tap events
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SVGBUTTON_H
