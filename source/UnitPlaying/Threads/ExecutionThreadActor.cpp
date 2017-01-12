#include "ExecutionThreadActor.h"
#include "Actor.h"
#include "Common.h"
#include "LabelButton.h"
#include "Scene.h"

/// The line number must appear within the display of the robot, but each robot has the display
/// at different top
const qreal lineNumberTopMargin[] = {0.795, 0.887, 0.867, 0.805};

ExecutionThreadActor::ExecutionThreadActor(int id, Scene* scene)
	: LinearLayout( Qt::Vertical )
	, actorNumber( mapActorNumber(id) )
	, scene(scene)
{
	buildActor();
}

void ExecutionThreadActor::buildActor()
{
	// Create the robot
	actor = new Actor(QString(":/unit_playing/threads/thread%1_back.svg").arg(actorNumber), scene);
	actor->setMarginTop(0.7);
	actor->alignCenter();
	addItem(actor, 1.0, zUnitPlaying::executionThread + 0.2);

	// Create the line number
	lineNumber = new LabelButton("00", scene);
	lineNumber->setAlignment(Qt::AlignCenter);
	addItem(lineNumber, 1.0, zUnitPlaying::executionThread + 0.3);

	// Center the line number within the robot's display
	const qreal lineNumberHeight = 0.06;
	const qreal lineNumberLeftRight = 0.42;
	qreal marginTop = lineNumberTopMargin[actorNumber - 1];
	qreal marginBottom = 1.0 - marginTop - lineNumberHeight;
	lineNumber->setMargins(marginTop, lineNumberLeftRight, marginBottom);
}

int ExecutionThreadActor::animateAppear()
{
	updateLayoutItem();
	return actor->appear();
}

int ExecutionThreadActor::animateDisappear()
{
	return actor->disappear();
}

int ExecutionThreadActor::updateLineNumber(int updatedLineNumber)
{
	Q_ASSERT(lineNumber);
	lineNumber->setText( QString::number(updatedLineNumber) );
	qDebug(">>>>>>Actor %d text: %s", actorNumber, qUtf8Printable(QString::number(updatedLineNumber)) );
	// ToDo: animate the line change
	return 0;
}
