#ifndef TRANSITIONSLIDE_H
#define TRANSITIONSLIDE_H

#include "Transition.h"

class TransitionSlide : public Transition
{
	Q_OBJECT

  public:
	/// The valid directions of movement of both slides
	enum class Direction { unknown, top, right, bottom, left };

  protected:
	/// The direction of movement of both slides
	Direction direction;

  public:
	/// Constructor
	explicit TransitionSlide(Direction direction, Scene* previousScene, Scene* nextScene, bool deletePreviousScene = false, QObject* parent = nullptr);
	/// Destructor
	virtual ~TransitionSlide();

  protected slots:
	  /// This method
	  virtual void animate(int frame) override;
};

#endif // TRANSITIONSLIDE_H
