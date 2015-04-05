#ifndef GLOBAL_H
#define GLOBAL_H

enum class SceneId
{
	  sceneUnknown
	, sceneGameMenu
	, sceneTraining
	, sceneMissions
	, sceneCollaboration
	, sceneCreate
	, sceneUnitSelection
	, sceneUnitPlaying
};

typedef unsigned long long ullint;

inline ullint ipow(ullint base, unsigned int exp)
{
	ullint result = 1llu;
	for ( ; exp; exp >>= 1, base *= base )
		if (exp & 1)
			result *= base;

	return result;
}

#endif // GLOBAL_H
