#include "ExecutionThreadActor.h"
#include "Assets.h"
#include "BotNeumannApp.h"
#include "Common.h"
#include "LabelButton.h"
#include "LinearLayout.h"
#include "Scene.h"
#include "Util.h"
#include "VisualizationSpeed.h"

#include <QFont>
#include <QBrush>

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
	lineNumber->setFont( QFont(BotNeumannApp::getMonospacedFontName()) );
	lineNumber->setAlignment(Qt::AlignCenter);
  #ifdef ABSTRACT
	lineNumber->setBrush( QBrush(Qt::black) );
  #endif

	// Center the line number within the robot's display
	qreal lineNumberLeft = (this->boundingRect().width() - lineNumberRefWidth) * 0.5;
	qreal lineNumberTop = lineNumberTopPadding[actorNumber - 1];
	lineNumber->resizeItem(lineNumber, lineNumberLeft, lineNumberTop, lineNumberRefWidth, lineNumberRefHeight, false);

	// The faces used when the robot turns to read or write data to stdin/stdout buffers
	QStringList robotFaces;
	robotFaces << QString("up_thread%1_back").arg(actorNumber);
	robotFaces << QString("up_thread%1_side").arg(actorNumber);
	robotFaces << QString("up_thread%1_front").arg(actorNumber);
	setFaces( robotFaces );
}

int ExecutionThreadActor::updateLineNumber(int updatedLineNumber)
{
	Q_ASSERT(lineNumber);
	return lineNumber->animateSetText( QString::number(updatedLineNumber) );
}

const QColor& ExecutionThreadActor::getHighlightColor() const
{
	Q_ASSERT( actorNumber - 1 < (int)(sizeof(threadColors) / sizeof(threadColors[0])) );
	return threadColors[actorNumber - 1];
}

int ExecutionThreadActor::animateTurn(QTimeLine::Direction direction, int initialDelay)
{
	// If this method must be called some time later, schedule a function call with delay 0
	if ( initialDelay > 0 )
	{
		Util::createTimer(initialDelay, this, [this, direction]{ animateTurn(direction, 0); } );
		return VisualizationSpeed::getInstance().adjust(750);
	}

	// We do not have to wait, start to animate turn right now
	// Hide the line number while the actor is turning
	lineNumber->setVisible(false);

	// Turn the actor
	int duration = transitionFaces(direction, 750);

	// When actor has finished turning, make the line number visible again
	Util::createTimer(duration, this, [this]{ this->lineNumber->setVisible(true); } );
	return duration;
}
