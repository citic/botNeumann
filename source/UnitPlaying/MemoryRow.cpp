#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"

MemoryRow::MemoryRow(size_t start, size_t size, Scene* scene)
	: LinearLayout(Qt::Horizontal)
	, start(start)
	, size(size)
	, scene(scene)
	, labelType(LabelType::memoryAddresses)
{
	buildMemoryRow();
}

MemoryRow::~MemoryRow()
{
}

void MemoryRow::buildMemoryRow()
{
	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftShelf = new Prop(":/unit_playing/unit_playing/shelf_left.svg", scene);
	Prop* middleShelf = new Prop(":/unit_playing/unit_playing/shelf_middle.svg", scene);
	Prop* rightShelf = new Prop(":/unit_playing/unit_playing/shelf_right.svg", scene);

	// Add them to the layout
	addItem(leftShelf, leftRightProportion);
	addItem(middleShelf, middleProportion);
	addItem(rightShelf, leftRightProportion);
}
