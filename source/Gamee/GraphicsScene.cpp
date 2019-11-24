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
		foreach ( const QUrl& url, event->mimeData()->urls() )
			qCDebug( logTemporary(), "GraphicsScene: dropped %s", qPrintable(url.toLocalFile()) );
}
