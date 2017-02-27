#include "BotNeumannApp.h"
#include "LabelButton.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
#include "Spacer.h"

#include <QBrush>
#include <QFont>

MemoryRow::MemoryRow(size_t start, size_t size, Scene* scene, qreal zValue, bool withGarbage)
	: LinearLayout(Qt::Horizontal)
	, start(start)
	, size(size)
	, scene(scene)
	, labelType(LabelType::memoryAddresses)
	, zValue(zValue)
{
	buildMemoryRow(withGarbage);
}

MemoryRow::~MemoryRow()
{
}

void MemoryRow::buildMemoryRow(bool withGarbage)
{
	Q_ASSERT(scene);

	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = getByteProportion();
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftShelf = new Prop("up_memory_row_left", scene);
	Prop* middleShelf = new Prop("up_memory_row_middle", scene);
	Prop* rightShelf = new Prop("up_memory_row_right", scene);

	// Add them to the layout
	addItem(leftShelf, leftRightProportion, zValue);
	addItem(middleShelf, middleProportion, zValue);
	addItem(rightShelf, leftRightProportion, zValue);

	// Create the memory addresses in a new layer
	buildMemoryAddresses();
	if ( withGarbage )
		fillWithGarbage();
}

void MemoryRow::buildMemoryAddresses()
{
	// Each memory address is for just one byte
	const qreal byteProportion = getByteProportion();
	// Memory address labels are in a higher layer
	qreal zMemoryAddress = this->zValue + 0.2;

	// Create a spacer for the left extreme
	addItem(new Spacer(), byteProportion, zMemoryAddress);

	// Create a label for each byte
	for (size_t index = start; index < start + size; ++index)
	{
		const QString& label = QString("%1 ").arg(index, 3, 10, QChar(' '));
		LabelButton* memoryAddress = new LabelButton(label, scene);
		memoryAddress->setMarginTop(0.77);
		memoryAddress->setMarginBottom(0.073);
		memoryAddress->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
		memoryAddress->setBrush(QBrush(Qt::yellow));
		addItem(memoryAddress, byteProportion, zMemoryAddress);
	}
}

const qreal refRowHeight = 34.879;

const qreal garbageMarginTop[] =
{
	1.0 - ( 14.500 + 15.515 ) / refRowHeight,
	1.0 - ( 14.500 + 11.480 ) / refRowHeight,
	1.0 - ( 14.500 +  7.478 ) / refRowHeight,
};

void MemoryRow::fillWithGarbage()
{
	// We place garbage in each byte
	const qreal byteProportion = getByteProportion();
	// Memory address labels are in a higher layer
	qreal zGarbage = this->zValue + 0.3;

	// Create a spacer for the left extreme
	addItem(new Spacer(), byteProportion, zGarbage);

	// Create a label for each byte
	for ( size_t index = 0; index < size; ++index )
	{
		int garbageNum = qrand() % 3 + 1;
		const QString& resource = QString("up_uninitialized_%1").arg( garbageNum );
		Prop* garbage = new Prop(resource, scene);
		garbage->setMarginLeft(0.15);
		garbage->setMarginRight(0.15);
		garbage->setMarginTop( garbageMarginTop[garbageNum - 1] );
		garbage->setMarginBottom( 14.500 / refRowHeight );
		//garbage->setOpacity(0.75);
		addItem(garbage, byteProportion, zGarbage);
	}
}
