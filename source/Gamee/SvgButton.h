#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include "ScenicElement.h"

class QGraphicsSimpleTextItem;

/**
	@brief Creates text and graphic push buttons

	A button is compound of the following layers, from top-most to bottom-most:

	|#| Layer      | Type   | Normal           | Pressed           | Hover |
	|-|------------|--------|------------------|-------------------|-------|
	|1| Label      | String | NormalStyle      | PressedStyle      | NA    |
	|2| Foreground | Image  | NormalForeground | PressedForeground | NA    |
	|3| Background | Image  | NormalBackground | PressedBackground | NA    |
 */
class SvgButton : public ScenicElement
{
	Q_OBJECT
	Q_DISABLE_COPY(SvgButton)

  protected:
	/// The text of the button
	QGraphicsSimpleTextItem* label;

  public:
	/// Constructor: creates a simple text button or an image button
	explicit SvgButton(const QString& label, const QString& normalForeground, const QString& normalBackground, QGraphicsItem* parentItem);
	/// Convenience method: creates a simple text button
	static SvgButton* createLabelButton(const QString& label, const QString& normalBackground, QGraphicsItem* parentItem);
	/// Convenience method: creates a simple image button
	static SvgButton* createImageButton(const QString& normalForeground, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~SvgButton();
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;

  signals:
	/// Emitted when this button is pressed
	/// If you need to have a pointer to the SvgButton instance, call sender()
	void pressed();

  protected:
	/// Called when this element is selected
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // SVGBUTTON_H
