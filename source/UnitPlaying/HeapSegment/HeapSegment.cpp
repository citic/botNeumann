#include "HeapSegment.h"
#include "MemoryFrame.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

// The memory interface requires less than a complete row
const double interfaceRows = 0.66;

HeapSegment::HeapSegment(Unit& unit, Scene* scene, QObject* parent)
	: GdbResponseListener(parent)
	, MemorySegment(unit, scene, Qt::Vertical)
{
	unit.requiresHeapSegment() ? buildSegment() : hideSegment();
}

HeapSegment::~HeapSegment()
{
}

double HeapSegment::getHeightInRows() const
{
	Q_ASSERT(memoryFrame);
	return memoryFrame->getHeightInRows() + interfaceRows;
}

void HeapSegment::buildSegment()
{
	// Get the number of memory rows that the heap segment requires
	size_t rowCount = unit.getHeapSegmentRows();
	size_t rowStartByte = unit.getHeapSegmentStartByte();
	size_t rowSize = unit.getHeapSegmentSize() / rowCount;

	// Create the memory rows and their roof
	Q_ASSERT(scene);
	memoryFrame = new MemoryFrame(scene, rowCount, rowStartByte, rowSize);

	// Distribute child elements according to the number of rows they require
	const double allRows = this->getHeightInRows();
	// ToDo: improve z-order management
	const double zContents = 0.1;

	// Add the memory rows and their roof to the scene
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
