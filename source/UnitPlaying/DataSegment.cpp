#include "DataSegment.h"
#include "Prop.h"
#include "Scene.h"

DataSegment::DataSegment(Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Vertical)
{
	buildDataSegment();
	buildStandardInOut();
}

DataSegment::~DataSegment()
{
}

void DataSegment::buildDataSegment()
{
	Q_ASSERT(scene);
	Prop* sharedTable = new Prop(":/unit_playing/unit_playing/shared_table.svg", scene);
	addItem(sharedTable, 1);
}

void DataSegment::buildStandardInOut()
{
}
