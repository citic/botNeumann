#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include "AlignedItem.h"
#include "LayoutItem.h"

#include <QGraphicsSimpleTextItem>
#include <QObject>

class LabelButton : public QObject, public QGraphicsSimpleTextItem, public LayoutItem, public AlignedItem
{
	Q_OBJECT
	Q_DISABLE_COPY(LabelButton)

  public:
	/// Creates an actor with the given svg filename
	explicit LabelButton(const QString& label, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~LabelButton();
	/// Used to differentiate between pure-layout items and scenic elements
	/// ToDo: rename to isPureLayoutObject()
	virtual bool isScenicElement() const override { return true; }
	/// Called on QGraphicItem in order to remove them from the scene
	virtual void removeFromScene() override;
	/// Overrides the setText in order to adjust the dimensions of the new text to the dimensions
	/// of the button
	/// @param updateLayout true if the LayoutItem::updateLayoutItem() should be called
	/// to update the dimensions of the label
	void setText(const QString &text, bool updateLayout);
	/// Sets the Z-index provided by layouts to the QGraphicsItem system
	virtual void setZ(qreal zValue) override { setZValue(zValue); }

  signals:
	/// Emitted when this button is pressed
	/// If you need to have a pointer to the SvgButton instance, call sender()
	void pressed();

  protected:
	/// Overriden to manage click or tap events
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
};

#endif // LABELBUTTON_H
