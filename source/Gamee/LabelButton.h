#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include "ScenicElement.h"
#include <QObject>

class LabelButton : public QObject, public ScenicElement<QGraphicsSimpleTextItem>
{
	Q_OBJECT
	Q_DISABLE_COPY(LabelButton)

  public:
	/// Creates an actor with the given svg filename
	explicit LabelButton(const QString& label, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~LabelButton();

  signals:
	/// Emitted when this button is pressed
	/// If you need to have a pointer to the SvgButton instance, call sender()
	void pressed();

  protected:
	/// Overriden to manage click or tap events
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // LABELBUTTON_H
