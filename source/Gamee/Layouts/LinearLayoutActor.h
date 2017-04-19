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
	/// To animate margin increase or decrease changes
	Q_PROPERTY(qreal marginTop READ getMarginTop WRITE updateMarginTop)
	Q_PROPERTY(qreal marginRight READ getMarginRight WRITE updateMarginRight)
	Q_PROPERTY(qreal marginBottom READ getMarginBottom WRITE updateMarginBottom)
	Q_PROPERTY(qreal marginLeft READ getMarginLeft WRITE updateMarginLeft)

  public:
	/// Constructor
	explicit LinearLayoutActor(Qt::Orientation orientation);
	/// Animate all children of this layout appearing
	int animateAppear(int duration = 1000, int initialDelay = 0, qreal fromOpacity = 0.0, qreal toOpacity = 1.0);
	/// Animate this value moving to the position
	int animateMoveTo(qreal endProportion, int duration, int initialDelay = 0);
	/// Animate this value moving to the position
	int animateMoveToPos(qreal endMainProportion, qreal endCrossProportion, int duration, int initialDelay = 0);
	/// Apply the given percent increase to the margins of this object
	/// @return Duration of animation in milliseconds
	int animateMarginIncrease(qreal topIncrease, qreal rightIncrease, qreal bottomIncrease, qreal leftIncrease, int duration, int initialDelay = 0);

  public:
	/// Updates this object, and also the parent object in order to reflect the position change
	inline void updateMainStartProportion(qreal startProportion) { setMainStartProportion(startProportion); updateParentLayoutItem(); }
	/// Updates this object, and also the parent object in order to reflect the position change
	inline void updateCrossStartProportion(qreal startProportion) { setCrossStartProportion(startProportion); updateParentLayoutItem(); }
	/// Get access to the saved start proportion copy
	inline qreal getSavedStartProportion() const { return savedStartProportion; }
	/// Saves a copy of the current start proportion. Useful before start a moveTo animation
	inline void saveStartProportion() { savedStartProportion = mainStart; }
	/// Set individual margins and updates the parent
	inline void updateMarginTop(qreal value) { setMarginTop(value); updateParentLayoutItem(); }
	inline void updateMarginRight(qreal value) { setMarginRight(value); updateParentLayoutItem(); }
	inline void updateMarginBottom(qreal value) { setMarginBottom(value); updateParentLayoutItem(); }
	inline void updateMarginLeft(qreal value) { setMarginLeft(value); updateParentLayoutItem(); }

  protected:
	/// Utility function to create an animation object for moving to some start percent margin
	QPropertyAnimation* createMoveToAnimation(const char* propertyName, qreal endProportion, qreal startProportion, int duration, int initialDelay = 0);
	/// Utility function to create an animation for a given margin
	QPropertyAnimation* createMarginAnimation(const char* propertyName, qreal increaseFactor, qreal initialValue, int duration, int initialDelay = 0);
	/// Updates the parent that contains this layout
	void updateParentLayoutItem();
};

#endif // LINEARLAYOUTACTOR_H
