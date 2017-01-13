#include "ExecutionThreadActor.h"
#include "Actor.h"
#include "Common.h"
#include "LabelButton.h"
#include "Scene.h"

#include <QColor>

/// The line number must appear within the display of the robot, but each robot has the display
/// at different top
const qreal lineNumberTopMargin[] = {0.795, 0.887, 0.867, 0.805};

static const int alpha = 127;

/// Each actor (robot) has its own color
static const QColor threadColors[] =
{
	QColor(  0,  90, 170, alpha), // thread 1 #005ab3 blue
	QColor(255, 192,   0, alpha), // thread 2 #ffc000 yellow
	QColor( 22, 107,   0, alpha), // thread 3 #de6b00 orange
	QColor(114, 140,   0, alpha), // thread 4 #728c00 green
};


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

const QColor& ExecutionThreadActor::getHighlightColor() const
{
	Q_ASSERT(actorNumber - 1 < sizeof(threadColors) / sizeof(threadColors[0]));
	return threadColors[actorNumber - 1];
}
