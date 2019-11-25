#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>

#include "GraphicsScene.h"
#include "LogManager.h"

GraphicsScene::GraphicsScene()
{
}

GraphicsScene::~GraphicsScene()
{
}

void GraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
	if ( event->mimeData()->hasUrls() )
		event->acceptProposedAction();
}

void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
	if ( event->mimeData()->hasUrls() )
	{
		Q_ASSERT( event->mimeData()->urls().count() > 0 );
		emit unitDropped( event->mimeData()->urls()[0].toLocalFile() );
	}
}
