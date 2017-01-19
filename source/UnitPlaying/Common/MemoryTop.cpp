#include "MemoryTop.h"
#include "Prop.h"
#include "Scene.h"

MemoryTop::MemoryTop(size_t size, Scene* scene, qreal zValue)
	: LinearLayout(Qt::Horizontal)
	, size(size)
	, scene(scene)
{
	buildMemoryTop(zValue);
}

void MemoryTop::buildMemoryTop(qreal zValue)
{
	Q_ASSERT(scene);

	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftRoof = new Prop("up_memory_top_left", scene);
	Prop* middleRoof = new Prop("up_memory_top_middle", scene);
	Prop* rightRoof = new Prop("up_memory_top_right", scene);

	// Add them to the layout
	addItem(leftRoof, leftRightProportion, zValue);
	addItem(middleRoof, middleProportion, zValue);
	addItem(rightRoof, leftRightProportion, zValue);
}
