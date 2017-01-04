#include "MemoryTop.h"
#include "Prop.h"
#include "Scene.h"

MemoryTop::MemoryTop(size_t size, Scene* scene)
	: LinearLayout(Qt::Horizontal)
	, size(size)
	, scene(scene)
{
	buildMemoryTop();
}

void MemoryTop::buildMemoryTop()
{
	Q_ASSERT(scene);

	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftRoof = new Prop(":/unit_playing/memory_top_left.svg", scene);
	Prop* middleRoof = new Prop(":/unit_playing/memory_top_middle.svg", scene);
	Prop* rightRoof = new Prop(":/unit_playing/memory_top_right.svg", scene);

	// Add them to the layout
	addItem(leftRoof, leftRightProportion);
	addItem(middleRoof, middleProportion);
	addItem(rightRoof, leftRightProportion);
}
