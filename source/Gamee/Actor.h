#ifndef ACTOR_H
#define ACTOR_H

#include <QGraphicsSvgItem>

/**
   @brief Any graphic object that moves or reacts on the scene
 */
class Actor : public QGraphicsSvgItem
{
	Q_OBJECT

  public:
	/// Constructor
	explicit Actor(QGraphicsItem* parent = 0);
	/// Creates an actor with the given svg filename
	explicit Actor(const QString& fileName, QGraphicsItem* parentItem = nullptr);
};

#endif // ACTOR_H
