#ifndef SPACER_H
#define SPACER_H

#include "LayoutItem.h"

class Spacer : public LayoutItem
{
  public:
	/// Constructor
	Spacer();
	/// Destructor
	virtual ~Spacer();
	/// Resize this item and all its child items
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
};

#endif // SPACER_H
