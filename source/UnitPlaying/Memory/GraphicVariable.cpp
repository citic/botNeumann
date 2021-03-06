#include "GraphicVariable.h"
#include "Actor.h"
#include "Assets.h"
#include "BotNeumannApp.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "MultiSvgButton.h"

#include <QBrush>
#include <QFont>

GraphicVariable::GraphicVariable(MemoryAllocation* variable, VisAddress firstByte, VisAddress lastByte, MemoryRow* memoryRow)
	: GraphicValue( variable->dataType, memoryRow->getGraphicsParentItem(), memoryRow->getZValue(), variable->value )
	, variable(variable)
	, firstByte(firstByte)
	, lastByte(lastByte)
	, memoryRow(memoryRow)
{
	// Determine if we have to represent the first byte and last byte of the variable
	// because we use special pods to represent these parts
	Q_ASSERT(variable);
	leftComplete = firstByte == variable->visualizationAddress;
	rightComplete = lastByte == variable->visualizationAddress + variable->size - 1;

	buildGraphicValue();
}

void GraphicVariable::applyDataTypePaddings(const qreal refDataPaddings[])
{
	setPaddingTop( parent ? refDataPaddings[refPaddingTop] + 0.1 : refDataPaddings[refPaddingTop] );
	setPaddingBottom( parent ? refDataPaddings[refPaddingBottom] + 0.1 : refDataPaddings[refPaddingBottom] );
}

bool GraphicVariable::buildVariableName(const qreal refDataPaddings[])
{
	const VisAddress size = getSize();

	// Amount of bytes required by the variable name
	qreal valueBytes = qMin( variable->value.length() / 2.5, size - 2.0 );
	qreal labelBytes = qMin( variable->name.length() / 2.5, size - valueBytes  );
	if ( valueBytes + labelBytes < size - 2 )
		valueBytes = size - 2 - labelBytes;

	// Variable name: label is created only if there is enough room
	if ( size >= 4 )
	{
		Q_ASSERT(nameLabel == nullptr);

		// Create a label that uses an array of images (represented by a piece of tape)
		QStringList labelAssets;
		labelAssets << "up_variable_name_left" << "up_variable_name_middle" << "up_variable_name_right";

		// Calculate proportions of each part of the tape, using the bytes they require
		const qreal leftRightProportion = 3.246 / 11.951 / labelBytes;
		const qreal middleProportion =  1.0 - 2 * leftRightProportion;
		QList<qreal> proportions;
		proportions << leftRightProportion << middleProportion << leftRightProportion;

		// Create the label and add it to the scene
		nameLabel = new MultiSvgButton(labelAssets, proportions, memoryRow->getGraphicsParentItem(), variable->name, memoryRow->getZValue() + zLabelValueOffset );
		nameLabel->setPaddingLeft(0.15);
		nameLabel->setPaddingTop( refDataPaddings[refLabelTop] );
		nameLabel->setPaddingBottom( refDataPaddings[refLabelBottom] );
		nameLabel->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
		nameLabel->setBrush(QBrush(Qt::black));
		//label->setShear(-0.2, 0.0);
		addItem(nameLabel, labelBytes / size, memoryRow->getZValue() + zLabelValueOffset);
	}
	else
		valueBytes = size - 2;

	// ToDo: If dataType is pointer, we can save a pointer to the pointed data, but it may be
	// more realistic to find the pointed data any time it is required

	return buildValueLabel(refDataPaddings, valueBytes / size);
}

bool GraphicVariable::buildMultiByteVariable(const QString& asset, const qreal refDataPaddings[])
{
	// Apply paddings according to the height of the data type and the nesting on composite data types
	applyDataTypePaddings(refDataPaddings);
	buildPod(asset, leftComplete, rightComplete);
	buildVariableName(refDataPaddings);
	return true;
}

bool GraphicVariable::buildArray()
{
	// We use the same proportions of structs for arrays
	applyDataTypePaddings(refStructPaddings);

	// We create an sub-array of the variable
	Q_ASSERT( variable->children.count() > 0 );
	VisAddress elementSize = variable->children[0]->size;
	int count = getSize() / elementSize;
	Q_ASSERT( count <= variable->children.count() );
	children.reserve(count);

	for ( int index = 0; index < count; ++index )
	{
		// ToDo: We need to connect arrays of structures with "up_array"
		VisAddress elementFirstByte = firstByte + elementSize * index;
		VisAddress elementLastByte = firstByte + elementSize * (index + 1) - 1;
		GraphicVariable* arrayElement = new GraphicVariable( variable->children[index], elementFirstByte, elementLastByte, memoryRow );
		arrayElement->parent = this;
		children.append(arrayElement);
		addItem(arrayElement, 1.0 / count, memoryRow->getZValue() + zPodOffset);
	}

	return true;
}

bool GraphicVariable::buildStruct()
{
	// ToDo: We have to traverse the data members of the structure
	// Label asset is not proportioned for structs. We fix it
	const qreal refStructPaddingsFixed[] = { refStructPaddings[refPaddingTop], refStructPaddings[refPaddingBottom], 0.1, 0.1 };
	return buildMultiByteVariable("up_struct", refStructPaddingsFixed);
}
