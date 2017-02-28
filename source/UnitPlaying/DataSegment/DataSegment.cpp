#include "DataSegment.h"
#include "Common.h"
#include "MemoryFrame.h"
#include "Scene.h"
#include "StandardInputOutput.h"
#include "Unit.h"

// The standard input/output requires less than a complete row
const double stdInOutRows = 0.66;

DataSegment::DataSegment(Unit& unit, Scene* scene, QObject* parent)
	: GdbResponseListener(parent)
	, MemorySegment(unit, scene, Qt::Vertical)
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

bool DataSegment::allocate(MemoryAllocation* memoryAllocation)
{
	return memoryFrame->allocate(memoryAllocation);
}

void DataSegment::buildDataSegment()
{
	// Memory rows are over the table
	size_t rowCount = unit.getDataSegmentRows();
	size_t rowStartByte = unit.getDataSegmentStartByte();
	size_t rowSize = unit.getDataSegmentSize() / rowCount;

	// Create the memory rows and their roof
	Q_ASSERT(scene);
	memoryFrame = new MemoryFrame(scene, rowCount, rowStartByte, rowSize, zUnitPlaying::dataSegment, false);

	// Distribute child elements according to the number of rows they require
	const double allRows = memoryFrame->getHeightInRows() + stdInOutRows;

	// Add the memory rows and their roof to the scene
	addItem(memoryFrame, memoryFrame->getHeightInRows() / allRows, zUnitPlaying::dataSegment);

	// Create the stdin and stdout pipes
	buildStandardInOut(stdInOutRows / allRows);
}

void DataSegment::buildStandardInOut(const double stdInOutProportion)
{
	// Create an exclusive layout for the standard input/output tubes
	LinearLayout* stdInOutLayout = new LinearLayout(Qt::Horizontal);
	addLayout(stdInOutLayout, stdInOutProportion, zUnitPlaying::standardInputOutput);

	// First stdin tube
	Q_ASSERT(standardInput == nullptr);
	standardInput = new StandardInputOutput("input", unit, scene);
	stdInOutLayout->addItem(standardInput, 0.5, zUnitPlaying::standardInputOutput);

	// Second stdout tube
	Q_ASSERT(standardOutput == nullptr);
	standardOutput = new StandardInputOutput("output", unit, scene);
	stdInOutLayout->addItem(standardOutput, 0.5, zUnitPlaying::standardInputOutput);
}

void DataSegment::clearAnimation()
{
	// ToDo: remove any global variables and convert them to garbage
	// ToDo: remove all input or output traversing the stddin/stdout tubes
}
