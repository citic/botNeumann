#include "HeapSegment.h"
#include "MemoryFrame.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

HeapSegment::HeapSegment(Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
{
	unit.requiresHeapSegment() ? buildSegment() : hideSegment();
}

HeapSegment::~HeapSegment()
{
}

void HeapSegment::buildSegment()
{
	// Get the number of memory rows that the heap segment requires
	size_t rowCount = unit.getHeapSegmentRows();
	size_t rowStartByte = unit.getHeapSegmentStartByte();
	size_t rowSize = unit.getHeapSegmentSize() / rowCount;

	memoryFrame = new MemoryFrame(scene, rowCount, rowStartByte, rowSize);

	// Height is calculate as the number of rows
	// The memory interface requires less than a complete row
	const double interfaceRows = 0.66;
	// The total number of rows to show
	const double allRows = memoryFrame->getHeightInRows() + interfaceRows;
	// ToDo: improve z-order management
	const double zContents = 0.1;

	addItem(memoryFrame, memoryFrame->getHeightInRows() / allRows, zContents);

	// Create the heap segment interface that robots will use to access it
	heapInterface = new Prop(":/unit_playing/heap_segment_interface.svg", scene);
	addItem(heapInterface, interfaceRows / allRows, zContents);
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/heap_segment_disabled.svg", scene);
	addItem(doors, 1.0);
}
