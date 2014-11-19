#ifndef SCENICELEMENT_H
#define SCENICELEMENT_H

#include <QGraphicsSvgItem>

class ScenicElement : public QGraphicsSvgItem
{
	Q_OBJECT
	Q_DISABLE_COPY(ScenicElement)

  public:
	/// Constructor
	explicit ScenicElement(QGraphicsItem* parentItem = nullptr);
	/// Creates an actor with the given svg filename
	explicit ScenicElement(const QString& fileName, QGraphicsItem* parentItem = nullptr);
};

#endif // SCENICELEMENT_H
