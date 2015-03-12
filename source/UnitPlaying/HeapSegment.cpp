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
	size_t rows = unit.getHeapSegmentRows();
	size_t rowStart = unit.getHeapSegmentStartByte();
	size_t rowSize = unit.getHeapSegmentSize() / rows;

	qDebug() << "Heap segment rows:" << rows << "size:" << unit.getHeapSegmentSize();
	for (size_t i = 0; i < rows; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStart, rowSize, scene);
		addItem(memoryRow, 1.0 / rows);
		rowStart += rowSize;
	}
}

void HeapSegment::hideSegment()
{
	Q_ASSERT(scene);
	Prop* doors = new Prop(":/unit_playing/unit_playing/shelves_closed.svg", scene);
	addItem(doors, 1.0);
}
