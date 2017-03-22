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


#include <QObject>

class LinearLayoutActor : public QObject, public LinearLayout
{
	Q_OBJECT
	Q_DISABLE_COPY(LinearLayoutActor)

  protected:
	/// To animate
	Q_PROPERTY(qreal startProportion READ getStartProportion WRITE updateStartProportion)

  public:
	/// Constructor
	explicit LinearLayoutActor(Qt::Orientation orientation);
	/// Animate this value moving to the position
	int animateMoveTo(qreal endProportion, int duration, int initialDelay = 0);
	/// Updates this object, and also the parent object in order to reflect the position change
	void updateStartProportion(qreal startProportion);
};

#endif // LINEARLAYOUT_H
