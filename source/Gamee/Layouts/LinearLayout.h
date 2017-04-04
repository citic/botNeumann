#ifndef LINEARLAYOUT_H
#define LINEARLAYOUT_H

#include "Layout.h"

class LinearLayout : public Layout
{
	Q_DISABLE_COPY(LinearLayout)

  protected:
	/// Direction to distribute child items
	Qt::Orientation orientation;

  public:
	/// Constructor
	explicit LinearLayout(Qt::Orientation orientation);
	/// Destructor
	virtual ~LinearLayout();
	/// Resize this layout and all its child items
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
};

#endif // LINEARLAYOUT_H
