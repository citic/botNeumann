#include "DataSegment.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
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
	Prop* sharedTable = new Prop(":/unit_playing/unit_playing/shared_table.svg", scene);
	addItem(sharedTable, 1);

	// Memory rows are over the table
	size_t rowCount = unit.getDataSegmentRows();
	size_t rowStartByte = unit.getDataSegmentStartByte();
	size_t rowSize = unit.getDataSegmentSize() / rowCount;

	// Each memory row requires a 1 complete row, and the tubes 1/3 of these height
	const double stdInOutHeight = 0.3;
	const double sumRowsTubes = (rowCount + stdInOutHeight);
	const double memoryRowProportion = 1.0 * 74 / 92 / sumRowsTubes;
	const double stdInOutProportion = stdInOutHeight * 74 / 92 / sumRowsTubes;
	const double zOverTable = 0.1;

	// Create the memory rows
	for (size_t i = 0; i < rowCount; ++i)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene);
		addItem(memoryRow, memoryRowProportion, zOverTable);
		rowStartByte += rowSize;
	}

	// Create the stdin and stdout pipes
	buildStandardInOut(stdInOutProportion, zOverTable);
}

void DataSegment::buildStandardInOut(const double stdInOutProportion, const double zStdInOut)
{
	// Create an exclusive layout for the standard menu
	LinearLayout* stdInOutLayout = new LinearLayout(Qt::Horizontal);
	addLayout(stdInOutLayout, stdInOutProportion, zStdInOut);

	// First stdin tube. ToDo: swap tube images
	Prop* inTube = new Prop(":/unit_playing/unit_playing/tube_out.svg", scene);
	stdInOutLayout->addItem(inTube, 0.5, zStdInOut);

	// Second stdout tube
	Prop* outTube = new Prop(":/unit_playing/unit_playing/tube_in.svg", scene);
	stdInOutLayout->addItem(outTube, 0.5, zStdInOut);
}
