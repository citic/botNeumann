#ifndef LINEARLAYOUTACTOR_H
#define LINEARLAYOUTACTOR_H

#include "LinearLayout.h"
#include <QObject>

class QPropertyAnimation;

class LinearLayoutActor : public QObject, public LinearLayout
{
	Q_OBJECT
	Q_DISABLE_COPY(LinearLayoutActor)

  protected:
	/// To animate main start proportion changes
	Q_PROPERTY(qreal mainStart READ getMainStartProportion WRITE updateMainStartProportion)
	/// To animate cross start proportion changes
	Q_PROPERTY(qreal crossStart READ getCrossStartProportion() WRITE updateCrossStartProportion)
	/// Saves a copy of the start proportion that can be restored after a moveTo animation
	qreal savedStartProportion = 0.0;
	/// To animate padding increase or decrease changes
	Q_PROPERTY(qreal paddingTop READ getPaddingTop WRITE updatePaddingTop)
	Q_PROPERTY(qreal paddingRight READ getPaddingRight WRITE updatePaddingRight)
	Q_PROPERTY(qreal paddingBottom READ getPaddingBottom WRITE updatePaddingBottom)
	Q_PROPERTY(qreal paddingLeft READ getPaddingLeft WRITE updatePaddingLeft)

  public:
	/// Constructor
	explicit LinearLayoutActor(Qt::Orientation orientation);
	/// Animate all children of this layout appearing
	int animateAppear(int duration = 1000, int initialDelay = 0, qreal fromOpacity = 0.0, qreal toOpacity = 1.0);
	/// Animate this value moving to the position
	int animateMoveTo(qreal endProportion, int duration, int initialDelay = 0);
	/// Animate this value moving to the position
	int animateMoveToPos(qreal endMainProportion, qreal endCrossProportion, int duration, int initialDelay = 0);
	/// Apply the given percent increase to the paddings of this object
	/// @return Duration of animation in milliseconds
	int animatePaddingIncrease(qreal topIncrease, qreal rightIncrease, qreal bottomIncrease, qreal leftIncrease, int duration, int initialDelay = 0);

  public:
	/// Updates this object, and also the parent object in order to reflect the position change
	inline void updateMainStartProportion(qreal startProportion) { setMainStartProportion(startProportion); updateParentLayoutItem(); }
	/// Updates this object, and also the parent object in order to reflect the position change
	inline void updateCrossStartProportion(qreal startProportion) { setCrossStartProportion(startProportion); updateParentLayoutItem(); }
	/// Get access to the saved start proportion copy
	inline qreal getSavedStartProportion() const { return savedStartProportion; }
	/// Saves a copy of the current start proportion. Useful before start a moveTo animation
	inline void saveStartProportion() { savedStartProportion = mainStart; }
	/// Set individual paddings and updates the parent
	inline void updatePaddingTop(qreal value) { setPaddingTop(value); updateParentLayoutItem(); }
	inline void updatePaddingRight(qreal value) { setPaddingRight(value); updateParentLayoutItem(); }
	inline void updatePaddingBottom(qreal value) { setPaddingBottom(value); updateParentLayoutItem(); }
	inline void updatePaddingLeft(qreal value) { setPaddingLeft(value); updateParentLayoutItem(); }

  protected:
	/// Utility function to create an animation object for moving to some start percent
	QPropertyAnimation* createMoveToAnimation(const char* propertyName, qreal endProportion, qreal startProportion, int duration, int initialDelay = 0);
	/// Utility function to create an animation for a given padding
	QPropertyAnimation* createPaddingAnimation(const char* propertyName, qreal increaseFactor, qreal initialValue, int duration, int initialDelay = 0);
	/// Updates the parent that contains this layout
	void updateParentLayoutItem();
};

#endif // LINEARLAYOUTACTOR_H
