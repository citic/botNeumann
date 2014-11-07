#ifndef BASESCENE_H
#define BASESCENE_H

#include <QGraphicsScene>

class QGraphicsSvgItem;

/**
   @brief Base class for all the scenes
 */
class BaseScene : public QGraphicsScene
{
	Q_OBJECT
	Q_DISABLE_COPY(BaseScene)

  protected:
	/// Each scene has a unique name, eg: "game_menu", "unit_selection", "unit_playing"...
	QString sceneName;
	/// A rectangle is used as canvas to paint and store each other item
	QGraphicsRectItem* canvas;
	/// Background image for this scene
	QGraphicsSvgItem* background;

  public:
	/// Constructor
	explicit BaseScene(QObject* parent = nullptr, const QString& sceneName = "", qreal width = 10.0, qreal height = 10.0);
	/// Changes the background image
	virtual void setBackground(const QString& filename = "background.svg");

  signals:

  public slots:

};

#endif // BASESCENE_H
