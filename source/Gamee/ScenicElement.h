#ifndef SCENICELEMENT_H
#define SCENICELEMENT_H

#include "LayoutItem.h"
#include <QGraphicsSvgItem>

class ScenicElement : public QGraphicsSvgItem, public LayoutItem
{
	Q_OBJECT
	Q_DISABLE_COPY(ScenicElement)

  public:
	/// Constructor
	explicit ScenicElement(QGraphicsItem* parentItem);
	/// Creates an actor with the given svg filename
	explicit ScenicElement(const QString& fileName, QGraphicsItem* parentItem);
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
};

#endif // SCENICELEMENT_H
