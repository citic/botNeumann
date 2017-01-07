#ifndef PROP_H
#define PROP_H

#include "ScenicElement.h"
#include <QGraphicsSvgItem>

class Prop : public ScenicElement<QGraphicsSvgItem>
{
	Q_OBJECT
	Q_DISABLE_COPY(Prop)

  public:
	/// Constructor
	explicit Prop(QGraphicsItem* parentItem);
	/// Creates an actor with the given svg filename
	explicit Prop(const QString& filename, QGraphicsItem* parentItem);
	/// Destructor
	virtual ~Prop();
};

#endif // PROP_H
