#ifndef BASESCENE_H
#define BASESCENE_H

#include <QGraphicsScene>

/**
   @brief Base class for all the scenes
 */
class BaseScene : public QGraphicsScene
{
	Q_OBJECT
	Q_DISABLE_COPY(BaseScene)

  public:
	/// Constructor
	explicit BaseScene(QObject* parent = nullptr);

  signals:

  public slots:

};

#endif // BASESCENE_H
