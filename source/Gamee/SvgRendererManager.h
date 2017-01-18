#ifndef SVGRENDERERMANAGER_H
#define SVGRENDERERMANAGER_H

#include "Common.h"
#include <QSvgRenderer>

/**
	Singleton in charge of loading (parsing) and sharing the SVG files, one per scene
*/
class SvgRendererManager : public QObject
{
	Q_OBJECT
	DISABLE_COPY_CLASS(SvgRendererManager);

  private:
	/// Pointer to the instance of the manger in heap
	static SvgRendererManager* manager;
	/// An svg renderer for each scene
	QSvgRenderer* renderers[sceneCount] = {0};

  public:
	/// Get access to the unique instance of this object
	static SvgRendererManager* getInstance();
	/// Removes the unique instance of this object
	static void deleteInstance();
	/// Get the svg renderer for the given scene
	QSvgRenderer* getRenderer(SceneId sceneId);
	/// Get the svg renderer for the given scene
	static inline QSvgRenderer* findRenderer(SceneId sceneId) { return getInstance()->getRenderer(sceneId); }

  private:
	/// Private default constructor
	explicit SvgRendererManager();
};

#endif // SVGRENDERERMANAGER_H
