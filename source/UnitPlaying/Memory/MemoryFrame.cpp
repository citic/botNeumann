#include "MemoryFrame.h"
#include "LogManager.h"
#include "MemoryAllocation.h"
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

bool MemoryFrame::allocate(MemoryAllocation* memoryAllocation)
{
	// ToDo: implement allocation
	qCCritical(logTemporary) << "Memory Frame: Allocating" << memoryAllocation->name << "for" << memoryAllocation->size << "bytes";
	return true;
}

bool MemoryFrame::deallocate(MemoryAllocation* memoryAllocation)
{
	// ToDo: implement deallocation
	qCCritical(logTemporary) << "Memory Frame: Deallocating" << memoryAllocation->name << "releasing" << memoryAllocation->size << "bytes";
	return true;
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
