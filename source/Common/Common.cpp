#include "Common.h"

#include <QtGlobal>

static const char* SceneName[] =
{
	"unknown",
	"game_menu",
	"training",
	"missions",
	"collaboration",
	"create",
	"unit_selection",
	"unit_playing",
};

const char* mapSceneName(SceneId sceneId)
{
	Q_ASSERT(sceneId <= sceneUnitPlaying);
	return SceneName[sceneId];
}
