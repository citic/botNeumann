#include "HeapSegment.h"
#include "Prop.h"
#include "Scene.h"

HeapSegment::HeapSegment(bool inUse, size_t ramSize, Scene* scene)
	: LinearLayout(Qt::Vertical)
	, inUse(inUse)
	, size(ramSize * 0.58) // Heap Segment is 58% of total RAM due to screen space limitations
	, scene(scene)
{
	inUse ? buildSegment() : hideSegment();
}

HeapSegment::~HeapSegment()
{
}

void HeapSegment::buildSegment()
{
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/unit_playing/shelves_closed.svg", scene);
	addItem(doors, 1);
}
