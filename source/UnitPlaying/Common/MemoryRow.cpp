#include "BotNeumannApp.h"
#include "LabelButton.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
#include "Spacer.h"

#include <QBrush>
#include <QFont>

MemoryRow::MemoryRow(size_t start, size_t size, Scene* scene, qreal zValue)
	: LinearLayout(Qt::Horizontal)
	, start(start)
	, size(size)
	, scene(scene)
	, labelType(LabelType::memoryAddresses)
{
	buildMemoryRow(zValue);
}

MemoryRow::~MemoryRow()
{
}

void MemoryRow::buildMemoryRow(qreal zValue)
{
	Q_ASSERT(scene);

	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftShelf = new Prop(":/unit_playing/memory_row_left.svg", scene);
	Prop* middleShelf = new Prop(":/unit_playing/memory_row_middle.svg", scene);
	Prop* rightShelf = new Prop(":/unit_playing/memory_row_right.svg", scene);

	// Add them to the layout
	addItem(leftShelf, leftRightProportion, zValue);
	addItem(middleShelf, middleProportion, zValue);
	addItem(rightShelf, leftRightProportion, zValue);

	// Create the memory addresses in a new layer
	buildMemoryAddresses(zValue);
}

void MemoryRow::buildMemoryAddresses(qreal zValue)
{
	// Each memory address is for just one byte
	const double byteProportion = 1.0 / (size + 2.0);
	// Memory address labels are in a higher layer
	zValue += 0.2;

	// Create a spacer for the left extreme
	addItem(new Spacer(), byteProportion, zValue);

	// Create a label for each byte
	for (size_t i = start; i < start + size; ++i)
	{
		const QString& label = QString("%1 ").arg(i, 3, 10, QChar(' '));
		LabelButton* memoryAddress = new LabelButton(label, scene);
		memoryAddress->setMarginTop(0.77);
		memoryAddress->setMarginBottom(0.073);
		memoryAddress->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
		memoryAddress->setBrush(QBrush(Qt::yellow));
		addItem(memoryAddress, byteProportion, zValue);
	}
}
