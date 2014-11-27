#ifndef BASESCENE_H
#define BASESCENE_H

#include <QGraphicsScene>

class QGraphicsSvgItem;

/**
   @brief Base class for all the scenes
 */
class Scene : public QGraphicsScene
{
	Q_OBJECT
	Q_DISABLE_COPY(Scene)

  protected:
	/// Each scene has a unique name, eg: "game_menu", "unit_selection", "unit_playing"...
	QString sceneName;
	/// A rectangle is used as canvas to paint and store each other item
	QGraphicsRectItem* canvas;
	/// Background image for this scene
	QGraphicsSvgItem* background;

  public:
	/// Constructor
	explicit Scene(QObject* parent = nullptr, const QString& sceneName = "", qreal width = 10.0, qreal height = 10.0);
	/// Destructor
	virtual ~Scene();
	/// Changes the background image
	virtual void setBackground(const QString& filename = "background.svg");
	/**	@brief Get full resouce filename for an assert that is contained in a folder with the same
		name of the scene. For example
		@code
			Scene gameMenuScene(stage, "GameMenu", stage->width(), stage->heigth());
			const QString& fullPath = gameMenuScene.getResourcePathFor("game_title.svg");
			// fullPath value would be ":/GameMenu/game_title.svg"
		@endcode
	**/
	virtual QString getResourcePathFor(const QString& assertName) const;
	/// Call this method when the stage room has changed its dimensions
	virtual void resize(qreal width, qreal height);

  signals:

  public slots:

};

#endif // BASESCENE_H
