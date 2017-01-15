#ifndef GLOBAL_H
#define GLOBAL_H

#define DISABLE_COPY_CLASS(Class) \
	Class(const Class&) = delete; \
	Class(Class&&) = delete; \
	Class& operator=(const Class&) = delete; \
	Class& operator=(Class&&) = delete

/// Used to identify all QSettings keys in source code (sk=setting key)
#define sk(key) key

enum SceneId
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

enum class UnitPlayingState
{
	editing,     // user edits code
	building,    // compiling and linking
	starting,    // setting debugger and visualization
	animating,   // visualization is in progress at normal speed or seeking
	paused,      // user paused visualization
};

enum zUnitPlaying
{
	background,
	heapSegment,
	cpuCores,
	cpuCore,
	stackFrame,
	executionThread,
	dataSegment,
	standardInputOutput = dataSegment,
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

/** Returns the number of digits required by an unsigned integer number
	It only counts the number of digits. It does not include space for a minus or plus sign
**/
template <typename Integer>
inline int digitsRequiredBy(Integer number)
{
	int digits = 1;
	for( number /= 10; number; number /= 10 )
			++digits;
	return digits;
}

#endif // GLOBAL_H
