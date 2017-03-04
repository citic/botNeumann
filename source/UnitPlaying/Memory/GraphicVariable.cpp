#include "GraphicVariable.h"
#include "LabelButton.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "Prop.h"
#include "Scene.h"
#include "SvgButton.h"

// Extra z-values for the parts of the graphic variable
const qreal zPodOffset   = 0.30;
const qreal zValueOffset = 0.31;
const qreal zLabelOffset = 0.32;

GraphicVariable::GraphicVariable(MemoryAllocation* variable, VisAddress firstByte, VisAddress lastByte, MemoryRow* memoryRow)
	: LinearLayout(Qt::Horizontal)
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

	buildGraphicVariable();
}

bool GraphicVariable::buildGraphicVariable()
{
	Q_ASSERT(variable);
	Q_ASSERT(memoryRow);

	// Create the pod, the value, and the label of the variable
	switch ( variable->dataType )
	{
		case typeUnknown: return false;
		case typeVoid: return false;

		// Atomic types
		case typeBool:      buildSingleByteVariable("up_bool_false"); break;
		case typeChar:      buildSingleByteVariable("up_char"); break;
		case typeInt:       buildMultiByteVariable("up_int"); break;
		case typeEnum:      buildMultiByteVariable("up_int"); break;
		case typeBitField:  buildMultiByteVariable("up_int"); break;
		case typeFloat:     buildMultiByteVariable("up_float"); break;

		// Indirection types
		case typePointer:   buildMultiByteVariable("up_pointer"); break;
		case typeReference: buildMultiByteVariable("up_pointer"); break;
		case typeRValue:    buildMultiByteVariable("up_pointer"); break;
		case typeFunction:  buildMultiByteVariable("up_pointer"); break;

		// Composite types
		case typeArray:     buildArray(); break;
		case typeStruct:    buildStruct(); break;
		case typeClass:     buildStruct(); break;
		case typeUnion:     buildStruct(); break;
	}

	return true;
}

bool GraphicVariable::buildSingleByteVariable(const QString& asset)
{
	// Pod
	// A single-byte variable requires just a prop
	Prop* pod = new Prop(asset, memoryRow->getScene());
	addItem(pod, 1.0, memoryRow->getZValue() + zPodOffset );

	// No variable label or value
	return true;
}

bool GraphicVariable::buildMultiByteVariable(const QString& asset)
{
	// Left and right parts always require 1 byte, middle the remaining bytes
	const qreal zPod = memoryRow->getZValue() + zPodOffset;
	const VisAddress size = getSize();

	// Pod: left
	Q_ASSERT(podLeft == nullptr);
	if ( leftComplete )
	{
		podLeft = new Prop(asset + "_left", memoryRow->getScene());
		podLeft->setMarginTop(0.139453539378996);
		podLeft->setMarginBottom(0.415722927836234);
		addItem(podLeft, 1.0 / size, zPod);
	}

	// Pod: middle
	Prop* podMiddle = new Prop(asset + "_middle", memoryRow->getScene());
	qreal middleSize = size - (VisAddress)leftComplete - (VisAddress)rightComplete;
	podMiddle->setMarginTop(0.139453539378996);
	podMiddle->setMarginBottom(0.415722927836234);
	addItem(podMiddle, middleSize / size, zPod);

	// Pod: right
	if ( rightComplete )
	{
		Prop* podRight = new Prop(asset + "_right", memoryRow->getScene());
		podRight->setMarginTop(0.139453539378996);
		podRight->setMarginBottom(0.415722927836234);
		addItem(podRight, 1.0 / size, zPod);
	}

	// Label is created only if there is enough room
	if ( size >= 4 )
	{
		// ToDo: Modify SvgButton to receive an array of asset names
		Q_ASSERT(label == nullptr);
		label = new SvgButton("up_variable_name_middle", memoryRow->getScene(), variable->name);
		label->setMarginLeft(0.15);
		label->setMarginTop(0.139453539378996);
		label->setMarginBottom(0.415722927836234);
		addItem(label, 2.0 / size, memoryRow->getZValue() + zLabelOffset);
	}

	// A label to store the value
	qreal valueProportion = size - 1.0;
	if ( label )
		valueProportion -= 2.0;

	Q_ASSERT(value == nullptr);
	value = new LabelButton( variable->value, memoryRow->getScene() );
	value->setMarginTop(0.139453539378996);
	value->setMarginBottom(0.415722927836234);
	value->alignRight();
	addItem(value, valueProportion / size, memoryRow->getZValue() + zValueOffset);

	// ToDo: If dataType is pointer, we can save a pointer to the pointed data, but it may be
	// more realistic to find the pointed data any time it is required

	return true;
}

bool GraphicVariable::buildArray()
{
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
	return buildMultiByteVariable("up_struct");
}
