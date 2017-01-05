#include "DataSegment.h"
#include "MemoryFrame.h"
#include "Scene.h"
#include "StandardInputOutput.h"
#include "Unit.h"

// The standard input/output requires less than a complete row
const double stdInOutRows = 0.66;

DataSegment::DataSegment(Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
{
	buildDataSegment();
}

DataSegment::~DataSegment()
{
}

double DataSegment::getHeightInRows() const
{
	Q_ASSERT(memoryFrame);
	return memoryFrame->getHeightInRows() + stdInOutRows;
}

void DataSegment::buildDataSegment()
{
	// Memory rows are over the table
	size_t rowCount = unit.getDataSegmentRows();
	size_t rowStartByte = unit.getDataSegmentStartByte();
	size_t rowSize = unit.getDataSegmentSize() / rowCount;

	// Create the memory rows and their roof
	Q_ASSERT(scene);
	memoryFrame = new MemoryFrame(scene, rowCount, rowStartByte, rowSize);

	// Distribute child elements according to the number of rows they require
	const double allRows = memoryFrame->getHeightInRows() + stdInOutRows;
	// ToDo: improve z-order management
	const double zContents = 0.1;

	// Add the memory rows and their roof to the scene
	addItem(memoryFrame, memoryFrame->getHeightInRows() / allRows, zContents);

	// Create the stdin and stdout pipes
	buildStandardInOut(stdInOutRows / allRows, zContents);
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
