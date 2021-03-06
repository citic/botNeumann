#ifndef SPACER_H
#define SPACER_H

#include "LayoutItem.h"

class Spacer : public LayoutItem
{
	Q_DISABLE_COPY(Spacer)

  public:
	/// Constructor
	Spacer();
	/// Destructor
	virtual ~Spacer() override;
	/// zValue is currenlty only set for QGraphicsItem objects
	virtual void setZ(qreal) override { }
};

#endif // SPACER_H
