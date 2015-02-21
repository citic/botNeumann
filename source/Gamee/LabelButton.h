#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include "ScenicElement.h"
#include <QObject>

class LabelButton : public QObject, public ScenicElement<QGraphicsSimpleTextItem>
{
	Q_OBJECT
	Q_DISABLE_COPY(LabelButton)

  public:
	/// Constructor
	explicit LabelButton(QGraphicsItem* parentItem);
	/// Creates an actor with the given svg filename
	explicit LabelButton(const QString& label, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~LabelButton();
};

#endif // LABELBUTTON_H
