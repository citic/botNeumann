#include "MemorySegment.h"

MemorySegment::MemorySegment(Unit& unit, Scene* scene, Qt::Orientation orientation)
	: LinearLayout(orientation)
	, unit(unit)
	, scene(scene)
{
}

MemorySegment::~MemorySegment()
{
}
