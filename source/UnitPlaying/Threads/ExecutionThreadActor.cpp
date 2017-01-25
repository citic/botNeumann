#include "ExecutionThreadActor.h"
#include "Common.h"
#include "LabelButton.h"
#include "LinearLayout.h"
#include "Scene.h"

#include <QColor>

/// The line number must appear within the display of the robot, but each robot has the display
/// at different top
const qreal lineNumberTopMargin[] = {34, 64, 62, 38};
const qreal lineNumberRefWidth =  33; // px
//const qreal lineNumberRefHeight = 14; // px

static const int alpha = 127;

/// Each actor (robot) has its own color
static const QColor threadColors[] =
{
	QColor(  0,  90, 170, alpha), // thread 1 #005ab3 blue
	QColor(255, 192,   0, alpha), // thread 2 #ffc000 yellow
	QColor(222, 107,   0, alpha), // thread 3 #de6b00 orange
	QColor(114, 140,   0, alpha), // thread 4 #728c00 green
};


ExecutionThreadActor::ExecutionThreadActor(int threadId, QGraphicsItem* parentItem)
	: Actor("", parentItem)
	, actorNumber( mapActorNumber(threadId) )
{
	setRenderer(sceneUnitPlaying);
	setElementId( QString("up_thread%1_back").arg(actorNumber) );
	buildActor();
}

int ExecutionThreadActor::mapActorNumber(int id)
{
	Q_UNUSED(id);
	// return (id - 1) % 4 + 1;
	return qrand() % 4 + 1;
}

void ExecutionThreadActor::buildActor()
{
	// Adjust the robot
	alignCenter();

	// Create the line number
	lineNumber = new LabelButton("00", this);
	lineNumber->setAlignment(Qt::AlignCenter);

	// Center the line number within the robot's display
	qreal lineNumberLeft = (this->boundingRect().width() - lineNumberRefWidth) * 0.5;
	qreal lineNumberTop = lineNumberTopMargin[actorNumber - 1];
	lineNumber->setPos(lineNumberLeft, lineNumberTop);
}

int ExecutionThreadActor::updateLineNumber(int updatedLineNumber)
{
	Q_ASSERT(lineNumber);
	lineNumber->setText( QString::number(updatedLineNumber), false );
	// ToDo: animate the line change
	return 0;
}

const QColor& ExecutionThreadActor::getHighlightColor() const
{
	Q_ASSERT( actorNumber - 1 < (int)(sizeof(threadColors) / sizeof(threadColors[0])) );
	return threadColors[actorNumber - 1];
}
