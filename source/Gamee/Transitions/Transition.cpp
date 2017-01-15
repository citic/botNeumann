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
	// If there are not previous scene, do not animate
	if ( previousScene != nullptr )
	{
		timeLine.setDuration(duration > 0 ? duration : defaultDuration);
		timeLine.setFrameRange(0, frames ? frames : defaultFrameCount);
		timeLine.setCurveShape(QTimeLine::EaseInCurve);
	}

	// Notify both scenes that they will be transitioned
	if ( previousScene ) previousScene->startLeavingStage();
	if ( nextScene ) nextScene->startEnteringStage();

	if ( previousScene != nullptr )
	{
		connect(&timeLine, SIGNAL(frameChanged(int)), SLOT(animate(int)));
		timeLine.start();
	}
	else
		transitionFinished();
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
	if ( previousScene && deletePreviousScene )
	{
		previousScene->deleteLater();
		previousScene = nullptr;
	}

	// Propagate the finished signal
	emit finished();
}
