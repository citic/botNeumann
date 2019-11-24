#ifndef TRANSITION_H
#define TRANSITION_H

#include <QObject>
#include <QTimeLine>

class Scene;

class Transition : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Transition)

  protected:
	/// Controls the animation time
	QTimeLine timeLine;
	/// The scene that is being replaced
	Scene* previousScene;
	/// The new scene to show in the stage
	Scene* nextScene;
	/// If true the transition will delete the previous scene object when the transition animation
	/// has finished. It is useful when the old scene will be not used even more
	bool deletePreviousScene;

  public:
	/// Default duration of a transition in milliseconds
	static const int defaultDuration = 1000 * 2 / 3;
	/// Default number of frames of a transition
	static const int defaultFrameCount = defaultDuration / 10;

  public:
	/// Constructor
	/// @param previousScene Pointer to the scene that is leaving the stage. If pointer is null
	/// not animation is done at all, and @a nextScene is shown immediately
	/// @param deletePreviousScene If true the transition will delete the previous scene object
	/// when the transition animation has finished. It is useful when the old scene will be not
	/// used even more
	explicit Transition(Scene* previousScene, Scene* nextScene, bool deletePreviousScene = false, QObject* parent = nullptr);
	/// Destructor
	virtual ~Transition() override;
	/// Start the animation
	/// @param duration in milliseconds
	/// @param frames the number of updates done while the animation is taking effect
	void start(int duration = defaultDuration, int frames = defaultFrameCount);
	/// Run the whole animation and deletes this object when finish
	void run(bool deleteAfterFinish, int duration = defaultDuration, int frames = defaultFrameCount);

  signals:
	/// Emmited when the transition animation is done
	void finished();

  protected slots:
	/// This method is called each time a frame update must be done in the transition animation
	virtual void animate(int frame) = 0;
	/// Called when this transition is done
	virtual void transitionFinished();
};

#endif // TRANSITION_H
