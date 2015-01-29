#ifndef BASESCENE_H
#define BASESCENE_H

#include "Global.h"
#include <QGraphicsRectItem>
#include <QObject>

class Layout;
class QGraphicsSvgItem;
class Stage;

/**
	@brief Base class for all the scenes
	A scene is defined by a rectangle in the screen. The scene is a canvas
 */
class Scene : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
	Q_DISABLE_COPY(Scene)

  protected:
	/// Each scene has a unique name, eg: "game_menu", "unit_selection", "unit_playing"...
	QString sceneName;
	/// Background image for this scene
	QGraphicsSvgItem* background;
	/// The unique stage where the game is supposed to run (at least at this version)
	Stage* stage;
	/// Layout in charge of placing and resize elements when the window is resized
	Layout* layout;

  public:
	/// Constructor
	explicit Scene(const QString& sceneName, Stage* stage, QGraphicsItem* parent = nullptr);
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
	/// Get the identification number of this scene
	virtual inline SceneId getSceneId() const = 0;
	/// Get access to the layout in charge of placing and resizing objects for this scene
	/// @return A pointer to the layout, nullptr if none
	inline Layout* getLayout() const { return layout; }
	/// Set a new layout for this scene. The previous one will be deleted
	void setLayout(Layout* layout);

  signals:
	/// Emitted when player press one of the buttons: Training, Missions, Collaboration or Create
	void newSceneAsked(SceneId id);

  public slots:
	/// Call this method when the stage room has changed its dimensions
	virtual void resize(qreal width, qreal height);
};

#endif // BASESCENE_H
