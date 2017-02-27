#include "MemoryFrame.h"
#include "MemoryRow.h"
#include "MemoryTop.h"
#include "Scene.h"

// The roof requires half memory row
const double memoryRoofRows = 0.5;

MemoryFrame::MemoryFrame(Scene* scene, size_t rowCount, size_t startByte, size_t rowSize, qreal zValue, bool withGarbage)
	: LinearLayout(Qt::Vertical)
	, scene(scene)
	, rowCount(rowCount)
	, startByte(startByte)
	, rowSize(rowSize)
{
	buildMemoryFrame(zValue, withGarbage);
}

double MemoryFrame::getHeightInRows() const
{
	return rowCount + memoryRoofRows;
}

void MemoryFrame::buildMemoryFrame(qreal zValue, bool withGarbage)
{
	// Create the memory roof
	Q_ASSERT(scene);
	MemoryTop* memoryTop = new MemoryTop(rowSize, scene, zValue);
	addItem(memoryTop, memoryRoofRows / getHeightInRows(), zValue);

	// Create the memory rows
	size_t rowStartByte = startByte;
	for (size_t index = 0; index < rowCount; ++index)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene, zValue, withGarbage);
		addItem(memoryRow, 1.0 / getHeightInRows(), zValue);
		rowStartByte += rowSize;
	}
}
