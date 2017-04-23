#include "ExecutionThreadActor.h"
#include "Assets.h"
#include "BotNeumannApp.h"
#include "Common.h"
#include "LabelButton.h"
#include "LinearLayout.h"
#include "Scene.h"
#include "VisualizationSpeed.h"

#include <QFont>
#include <QTimer>

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
	int duration = 0;
	QTimer* timer = new QTimer(this);
	timer->setSingleShot(true);

	if ( initialDelay > 0 )
	{
		connect(timer, &QTimer::timeout, [this, direction]{ animateTurn(direction, 0); } );
		duration += VisualizationSpeed::getInstance().adjust(750);
		timer->start(initialDelay);
	}
	else
	{
		lineNumber->setVisible(false);
		duration += transitionFaces(direction, 750);
		connect(timer, &QTimer::timeout, [this]{ this->lineNumber->setVisible(true); } );
		timer->start(duration);
	}

	return duration;
}

void ExecutionThreadActor::setLineNumberVisible()
{
	lineNumber->setVisible(true);
}
