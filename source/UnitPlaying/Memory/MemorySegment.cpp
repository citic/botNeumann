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

bool MemorySegment::allocate(MemoryAllocation* memoryAllocation)
{
	Q_UNUSED(memoryAllocation);
	Q_ASSERT(false);
	return false;
}

bool MemorySegment::deallocate(MemoryAllocation* memoryAllocation)
{
	Q_UNUSED(memoryAllocation);
	Q_ASSERT(false);
	return false;
}
