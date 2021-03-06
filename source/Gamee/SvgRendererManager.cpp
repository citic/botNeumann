#include "SvgRendererManager.h"

SvgRendererManager* SvgRendererManager::manager = nullptr;


static const char* svgFiles[] =
{
	":/general.svg",

	":/game_menu.svg",
	":/unit_selection.svg",
  #if ABSTRACT
	":/unit_playing_abstract.svg"
  #else
	":/unit_playing_concrete.svg"
  #endif
};

SvgRendererManager::SvgRendererManager()
{
}

SvgRendererManager*SvgRendererManager::getInstance()
{
	if ( manager == nullptr )
		manager = new SvgRendererManager();

	return manager;
}

void SvgRendererManager::deleteInstance()
{
	delete manager;
	manager = nullptr;
}

QSvgRenderer* SvgRendererManager::getRenderer(SceneId sceneId)
{
	Q_ASSERT(sceneId < sceneCount);

	if ( renderers[sceneId] == nullptr )
	{
		renderers[sceneId] = new QSvgRenderer( QString(svgFiles[sceneId]), this);
		Q_ASSERT( renderers[sceneId]->isValid() );
	}

	return renderers[sceneId];
}
