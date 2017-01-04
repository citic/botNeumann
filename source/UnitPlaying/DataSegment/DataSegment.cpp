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
	// The standard input/oput requires less than a complete row
	const double stdInOutRows = 0.66;
	// Each memory row requires a row, and the i/o tubes 1 more complete row
	const double allRows = (memoryRoofRows + rowCount + stdInOutRows);
	// Distribute 1.0 among all the rows
	const double rowProportion = 1.0 / allRows;
	// ToDo: improve z-order management
	const double zContents = 0.1;

	// The background requires the whole segment area, but memory rows and stdin/out pipes must
	// occupy the content area, that is, they should not cover the border of the background
	LinearLayout* contentsLayout = new LinearLayout(Qt::Vertical);
	contentsLayout->setMargins(2.0, 1.0, -3.0, 1.0); // px each
	addLayout(contentsLayout, 1.0, zContents);

	// Create the memory roof
	MemoryTop* memoryTop = new MemoryTop(rowSize, scene);
	contentsLayout->addItem(memoryTop, memoryRoofRows * rowProportion, zContents);

	// Create the memory rows
	for (size_t i = 0; i < rowCount; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		contentsLayout->addItem(memoryRow, rowProportion, zContents);
		rowStartByte += rowSize;
	}

	// Create the stdin and stdout pipes
	buildStandardInOut(contentsLayout, stdInOutRows * rowProportion, zContents);
}

void DataSegment::buildStandardInOut(LinearLayout* contentsLayout, const double stdInOutProportion, const double zStdInOut)
{
	// Create an exclusive layout for the standard input/output tubes
	LinearLayout* stdInOutLayout = new LinearLayout(Qt::Horizontal);
	contentsLayout->addLayout(stdInOutLayout, stdInOutProportion, zStdInOut);

	// First stdin tube
	Q_ASSERT(standardInput == nullptr);
	standardInput = new StandardInputOutput("input", unit, scene);
	stdInOutLayout->addItem(standardInput, 0.5, zStdInOut);

	// Second stdout tube
	Q_ASSERT(standardOutput == nullptr);
	standardOutput = new StandardInputOutput("output", unit, scene);
	stdInOutLayout->addItem(standardOutput, 0.5, zStdInOut);
}
