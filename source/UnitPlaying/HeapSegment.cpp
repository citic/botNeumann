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

#include <QDebug>
void HeapSegment::buildSegment()
{
	size_t rowCount = unit.getHeapSegmentRows();
	size_t rowStartByte = unit.getHeapSegmentStartByte();
	size_t rowSize = unit.getHeapSegmentSize() / rowCount;

	qDebug() << "Heap segment rows:" << rowCount << "size:" << rowSize;
	for (size_t i = 0; i < rowCount; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		addItem(memoryRow, 1.0 / rowCount);
		rowStartByte += rowSize;
	}
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/unit_playing/shelves_closed.svg", scene);
	addItem(doors, 1.0);
}
