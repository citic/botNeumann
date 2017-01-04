#include "HeapSegment.h"
#include "MemoryRow.h"
#include "MemoryTop.h"
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

	// Height is calculate as the number of rows
	// The roof requires half row
	const double memoryRoofRows = 0.5;
	// The memory interface requires less than a complete row
	const double interfaceRows = 0.66;
	// The total number of rows to show
	const double allRows = (memoryRoofRows + rowCount + interfaceRows);
	// Distribute 1.0 among all the rows
	const double rowProportion = 1.0 / allRows;
	// ToDo: improve z-order management
	const double zContents = 0.1;

	// Create the memory roof
	MemoryTop* memoryTop = new MemoryTop(rowSize, scene);
	addItem(memoryTop, memoryRoofRows * rowProportion, zContents);

	// Create the memory rows
	for (size_t i = 0; i < rowCount; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		addItem(memoryRow, 1.0 * rowProportion, zContents);
		rowStartByte += rowSize;
	}

	// Create the heap segment interface that robots will use to access it
	Prop* heapInterface = new Prop(":/unit_playing/heap_segment_interface.svg", scene);
	addItem(heapInterface, interfaceRows * rowProportion, zContents);
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/heap_segment_disabled.svg", scene);
	addItem(doors, 1.0);
}
