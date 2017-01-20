#ifndef BASESCENE_H
#define BASESCENE_H

#include <QGraphicsRectItem>
#include <QObject>

class Layout;
class Prop;
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
	/// ToDo: Make scenes inherit ScenicElement, therefore, background is their image
	Prop* background = nullptr;
	/// The unique stage where the game is supposed to run (at least at this version)
	Stage* stage = nullptr;
	/// Layout in charge of placing and resize elements when the window is resized
	Layout* layout = nullptr;

  public:
	/// Constructor
	explicit Scene(const QString& sceneName, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~Scene();
	/// Changes the background image
	virtual void setBackground(const QString& svgElementId);
	/// Get access to the layout in charge of placing and resizing objects for this scene
	/// @return A pointer to the layout, nullptr if none
	inline Layout* getLayout() const { return layout; }
	/// Set a new layout for this scene. The previous one will be deleted
	void setLayout(Layout* layout);
	/// Called when a transition starts, indicating this scene is leaving the stage
	virtual void startLeavingStage() { }
	/// Called when a transition starts, indicating this scene is entering into the stage
	virtual void startEnteringStage() { }
	/// Called when a transition has finished, indicating this scene leaved the stage
	virtual void finishedLeavingStage() { }
	/// Called when a transition has finished, indicating this scene entered into the stage
	virtual void finishedEnteringStage() { }

  public slots:
	/// Call this method when the stage room has changed its dimensions
	virtual void resize(qreal width, qreal height);
};

#endif // BASESCENE_H
