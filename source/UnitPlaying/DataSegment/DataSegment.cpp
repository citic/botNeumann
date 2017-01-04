#include "DataSegment.h"
#include "MemoryRow.h"
#include "MemoryTop.h"
#include "Scene.h"
#include "StandardInputOutput.h"
#include "Unit.h"

DataSegment::DataSegment(Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
{
	buildDataSegment();
}

DataSegment::~DataSegment()
{
}

void DataSegment::buildDataSegment()
{
	// The background: a shared table
	Q_ASSERT(scene);

	// Memory rows are over the table
	size_t rowCount = unit.getDataSegmentRows();
	size_t rowStartByte = unit.getDataSegmentStartByte();
	size_t rowSize = unit.getDataSegmentSize() / rowCount;

	// Height is calculate as the number of rows
	// The roof requires half row
	const double memoryRoofRows = 0.5;
	// The standard input/output requires less than a complete row
	const double stdInOutRows = 0.66;
	// The total number of rows to show
	const double allRows = (memoryRoofRows + rowCount + stdInOutRows);
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

	// Create the stdin and stdout pipes
	buildStandardInOut(stdInOutRows * rowProportion, zContents);
}

void DataSegment::buildStandardInOut(const double stdInOutProportion, const double zStdInOut)
{
	// Create an exclusive layout for the standard input/output tubes
	LinearLayout* stdInOutLayout = new LinearLayout(Qt::Horizontal);
	addLayout(stdInOutLayout, stdInOutProportion, zStdInOut);

	// First stdin tube
	Q_ASSERT(standardInput == nullptr);
	standardInput = new StandardInputOutput("input", unit, scene);
	stdInOutLayout->addItem(standardInput, 0.5, zStdInOut);

	// Second stdout tube
	Q_ASSERT(standardOutput == nullptr);
	standardOutput = new StandardInputOutput("output", unit, scene);
	stdInOutLayout->addItem(standardOutput, 0.5, zStdInOut);
}
