#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	Q_DISABLE_COPY(GraphicsScene)

  public:
	GraphicsScene();
	virtual ~GraphicsScene() override;

  protected:
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
	virtual void dropEvent(QGraphicsSceneDragDropEvent* event) override;
};

#endif // GRAPHICSSCENE_H
