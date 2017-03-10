#include "BotNeumannApp.h"
#include "Assets.h"
#include "GraphicVariable.h"
#include "LabelButton.h"
#include "LogManager.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
#include "Spacer.h"

#include <QBrush>
#include <QFont>

MemoryRow::MemoryRow(VisAddress start, VisAddress size, Scene* scene, qreal zValue, bool withGarbage)
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
		buildGarbage();
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
	for (VisAddress index = start; index < start + size; ++index)
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

void MemoryRow::buildGarbage()
{
	// We place garbage in each byte
	const qreal byteProportion = getByteProportion();
	// Memory address labels are in a higher layer
	qreal zGarbage = this->zValue + 0.4;

	// Create a spacer for the left extreme
	addItem(new Spacer(), byteProportion, zGarbage);

	// Create a garbage artifact for each byte
	for ( VisAddress index = 0; index < size; ++index )
	{
		// We selected the garbage prop for this byte randomly
		int garbageNum = qrand() % 3 + 1;
		const QString& resource = QString("up_uninitialized_%1").arg( garbageNum );

		// Create the garbage artifact and place it in its byte within the memory row
		Prop* garbage = new Prop(resource, scene);

		// We use margins because artifacts are smaller than the memory row's height
		garbage->setMarginLeft(0.15);
		garbage->setMarginRight(0.15);
		garbage->setMarginTop( garbageMarginTop[garbageNum - 1] );
		garbage->setMarginBottom( 14.500 / refRowHeight );
		//garbage->setOpacity(0.75);

		// We ad the garbage to the scene, and also to the list of artifacts
		addItem(garbage, byteProportion, zGarbage);
		this->garbage.append( garbage );
	}
}

bool MemoryRow::showGarbage(VisAddress firstByte, VisAddress lastByte, bool visible)
{
	// Map addresses to array indexes
	lastByte  -= start;
	firstByte -= start;

	// Check index out of bounds
	if ( firstByte < 0 || firstByte >= garbage.count() ) return false;
	if ( lastByte  < 0 || lastByte  >= garbage.count() ) return false;

	// Make the garbage visible or hidden
	while ( firstByte <= lastByte )
		garbage[firstByte++]->setVisible(visible);

	// Success
	return true;
}

bool MemoryRow::allocate(MemoryAllocation* variable)
{
	Q_ASSERT(variable);

	// If the variable should be entirely or partially allocated, the intersection with the
	// addresses of this memory rows is not empty
	VisAddress firstByte = 0;
	VisAddress lastByte = 0;

	// If intersection of the variable with this row is empty, we do not allocate it here
	if ( ! calculateIntersection(variable, firstByte, lastByte) )
		return false;

	// If variable is free space, we do not need to allocate values in its bytes, just garbage
	if ( variable->isFreeFragment() )
		return showGarbage(firstByte, lastByte);

	// There is some intersection. We place a part of the variable (intersection) in this memory row
	// We ask the variable to create a graphical variable to draw only this intersection
	GraphicVariable* graphicVariable = variable->getGraphicVariableFor(firstByte, lastByte, this);
	Q_ASSERT(graphicVariable);

	// Dimensions of the row are calculated in terms of the lenght of a byte
	const qreal byteProportion = getByteProportion();
	const qreal zVariable = this->zValue + 0.3;

	// If the variable is already allocated, we just update it
	if ( graphicVariables.contains(graphicVariable) == false )
	{
		// Add the graphical variable to the list of variables allocated in this memory row
		graphicVariables.append(graphicVariable);

		// We add the variable to an absolute position in layout, +1 for memory legs at the left
		insertItem( graphicVariable, (firstByte - start + 1) * byteProportion, (lastByte - firstByte + 1) * byteProportion, zVariable );
		qCCritical(logTemporary, "MemRow: %s proportion %lf", qPrintable(variable->name), (lastByte - firstByte + 1) * byteProportion);
		this->updateLayoutItem();
	}

	// Return true if the variable was entirely allocated in this memory row
	qCCritical(logTemporary, "Memory row: allocating %s in [%lld, %lld]", qPrintable(variable->name), firstByte, lastByte);
	return lastByte >= variable->visualizationAddress + variable->size - 1;
}

bool MemoryRow::calculateIntersection(const MemoryAllocation* variable, VisAddress& firstByte, VisAddress& lastByte)
{
	Q_ASSERT(variable);

	// If var.min and row.min is the first byte of the variable and the row respectively, and
	// var.max and row.max the last byte of the variable and the row respectively;
	// the intersection is [ max(var.min, row.min), min(var.max, row.max) ]
	// that is, intersection is "the maximum of the minimums, and the minimum of the maximums".
	firstByte = qMax( variable->visualizationAddress, this->start );
	lastByte = qMin( variable->visualizationAddress + variable->size, this->start + this->size ) - 1;

	// Intersection [firstByte, lastByte] is valid if they are ordered and there is at least one
	// byte-length as intersection
	return firstByte <= lastByte;
}
