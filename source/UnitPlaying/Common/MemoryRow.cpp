#include "BotNeumannApp.h"
#include "LabelButton.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
#include "Spacer.h"
#include <QBrush>
#include <QFont>

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
	Prop* leftShelf = new Prop(":/unit_playing/memory_row_left.svg", scene);
	Prop* middleShelf = new Prop(":/unit_playing/memory_row_middle.svg", scene);
	Prop* rightShelf = new Prop(":/unit_playing/memory_row_right.svg", scene);

	// Add them to the layout
	addItem(leftShelf, leftRightProportion);
	addItem(middleShelf, middleProportion);
	addItem(rightShelf, leftRightProportion);

	// Create the memory addresses in a new layer
	buildMemoryAddresses();
}

void MemoryRow::buildMemoryAddresses()
{
	// Each memory address is for just one byte
	const double byteProportion = 1.0 / (size + 2.0);
	// Memory address labels are in a higher layer
	const double z = 0.2;

	// Create a spacer for the left extreme
	addItem(new Spacer(), byteProportion, z);

	// Create a label for each byte
	for (size_t i = start; i < start + size; ++i)
	{
		const QString& label = QString("%1 ").arg(i, 3, 10, QChar(' '));
		LabelButton* memoryAddress = new LabelButton(label, scene);
		memoryAddress->setMarginTop(0.91);
		memoryAddress->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
		memoryAddress->setBrush(QBrush(Qt::black));
		addItem(memoryAddress, byteProportion, z);
	}
}
