#include "MemoryFrame.h"
#include "MemoryRow.h"
#include "MemoryTop.h"
#include "Scene.h"

// The roof requires half memory row
const double memoryRoofRows = 0.5;

MemoryFrame::MemoryFrame(Scene* scene, size_t rowCount, size_t startByte, size_t rowSize)
	: LinearLayout(Qt::Vertical)
	, scene(scene)
	, rowCount(rowCount)
	, startByte(startByte)
	, rowSize(rowSize)
{
	buildMemoryFrame();
}

double MemoryFrame::getHeightInRows() const
{
	return rowCount + memoryRoofRows;
}

void MemoryFrame::buildMemoryFrame()
{
	// ToDo: improve z-order management
	Q_ASSERT(scene);
	const double zContents = 0.1;

	// Create the memory roof
	MemoryTop* memoryTop = new MemoryTop(rowSize, scene);
	addItem(memoryTop, memoryRoofRows / getHeightInRows(), zContents);

	// Create the memory rows
	size_t rowStartByte = startByte;
	for (size_t index = 0; index < rowCount; ++index)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		addItem(memoryRow, 1.0 / getHeightInRows(), zContents);
		rowStartByte += rowSize;
	}
}
