#include "HeapSegment.h"
#include "MemoryRow.h"
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
	size_t rowCount = unit.getHeapSegmentRows();
	size_t rowStartByte = unit.getHeapSegmentStartByte();
	size_t rowSize = unit.getHeapSegmentSize() / rowCount;

	// Each memory row requires a 1 complete row, and the interface 0.5 of these height
	const double memoryRowProportion = 1.0 / (rowCount + 0.5);
	const double interfaceProportion = 0.5 / (rowCount + 0.5);

	// Create the memory rows
	for (size_t i = 0; i < rowCount; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		addItem(memoryRow, memoryRowProportion);
		rowStartByte += rowSize;
	}

	// Create the heap segment interface that robots will use to access it
	Prop* heapInterface = new Prop(":/unit_playing/heap_segment_interface.svg", scene);
	addItem(heapInterface, interfaceProportion);
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/heap_segment_disabled.svg", scene);
	addItem(doors, 1.0);
}
