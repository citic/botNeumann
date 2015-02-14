#include "Transition.h"
#include "Scene.h"

Transition::Transition(Scene* previousScene, Scene* nextScene, bool deletePreviousScene, QObject *parent)
	: QObject(parent)
	, previousScene(previousScene)
	, nextScene(nextScene)
	, deletePreviousScene(deletePreviousScene)
{
	// Call transitionFinished() anytime when the animation is done
	connect(&timeLine, SIGNAL(finished()), this, SLOT(transitionFinished()));
}

Transition::~Transition()
{
}

void Transition::start(int duration, int frames)
{
	timeLine.setDuration(duration ? duration : defaultDuration);
	timeLine.setFrameRange(0, frames ? frames : defaultFrameCount);
	timeLine.setCurveShape(QTimeLine::EaseInCurve);

	// Notify both scenes that they will be transitioned
	if ( previousScene ) previousScene->startLeavingStage();
	if ( nextScene ) nextScene->startEnteringStage();

	connect(&timeLine, SIGNAL(frameChanged(int)), SLOT(animate(int)));

	timeLine.start();
}

void Transition::run(bool deleteAfterFinish, int duration, int frames)
{
	if ( deleteAfterFinish )
		connect(&timeLine, SIGNAL(finished()), this, SLOT(deleteLater()));

	start(duration, frames);
}

void Transition::transitionFinished()
{
	// Notify both scenes that the transition finished
	if ( previousScene ) previousScene->finishedLeavingStage();
	if ( nextScene ) nextScene->finishedEnteringStage();

	// Delete the previous scene if it was asked
	if ( deletePreviousScene )
	{
		Q_ASSERT(previousScene);
		previousScene->deleteLater();
		previousScene = nullptr;
	}

	// Propagate the finished signal
	emit finished();
}
