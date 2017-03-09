#include "GraphicVariable.h"
#include "Assets.h"
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
		case typeBool:      buildSingleByteVariable("up_bool_false", refBoolMargins); break;
		case typeChar:      buildSingleByteVariable("up_char", refCharMargins); break;
		case typeInt:       buildMultiByteVariable("int", refIntMargins); break;
		case typeEnum:      buildMultiByteVariable("int", refIntMargins); break;
		case typeBitField:  buildMultiByteVariable("int", refIntMargins); break;
		case typeFloat:     buildMultiByteVariable("float", refFloatMargins); break;

		// Indirection types
		case typePointer:   buildMultiByteVariable("pointer", refPointerMargins); break;
		case typeReference: buildMultiByteVariable("pointer", refPointerMargins); break;
		case typeRValue:    buildMultiByteVariable("pointer", refPointerMargins); break;
		case typeFunction:  buildMultiByteVariable("pointer", refPointerMargins); break;

		// Composite types
		case typeArray:     buildArray(); break;
		case typeStruct:    buildStruct(); break;
		case typeClass:     buildStruct(); break;
		case typeUnion:     buildStruct(); break;
	}

	return true;
}

void GraphicVariable::applyDataTypeMargins(const qreal dataMarginTop, const qreal dataMarginBottom)
{
	setMarginTop( parent ? dataMarginTop + 0.1 : dataMarginTop );
	setMarginBottom( parent ? dataMarginBottom + 0.1 : dataMarginBottom );
}

bool GraphicVariable::buildSingleByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins[refMarginTop], refDataMargins[refMarginBottom]);

	// Pod
	// A single-byte variable requires just a prop
	Prop* pod = new Prop(asset, memoryRow->getScene());
	addItem(pod, 1.0, memoryRow->getZValue() + zPodOffset );

	// No variable label or value
	return true;
}

bool GraphicVariable::buildMultiByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins[refMarginTop], refDataMargins[refMarginBottom]);

	// Left and right parts always require 1 byte, middle the remaining bytes
	const qreal zPod = memoryRow->getZValue() + zPodOffset;
	const VisAddress size = getSize();
	const QString& svgFileBase = ":/unit_playing/data_types/" + asset;

	// Pod: left
	Q_ASSERT(podLeft == nullptr);
	if ( leftComplete )
	{
		podLeft = asset.startsWith("up_")
				? new ScenicElement(asset + "_left", memoryRow->getScene())
				: new ScenicElement(svgFileBase + "_left.svg", memoryRow->getScene(), true);

		addItem(podLeft, 1.0 / size, zPod);
	}

	// Pod: middle
	ScenicElement* podMiddle = asset.startsWith("up_")
			? new ScenicElement(asset + "_middle", memoryRow->getScene())
			: new ScenicElement(svgFileBase + "_middle.svg", memoryRow->getScene(), true);
	qreal middleSize = size - (VisAddress)leftComplete - (VisAddress)rightComplete;
	addItem(podMiddle, middleSize / size, zPod);

	// Pod: right
	if ( rightComplete )
	{
		ScenicElement* podRight = asset.startsWith("up_")
				? new ScenicElement(asset + "_right", memoryRow->getScene())
				: new ScenicElement(svgFileBase + "_right.svg", memoryRow->getScene(), true);
		addItem(podRight, 1.0 / size, zPod);
	}

	// Variable name: label is created only if there is enough room
	if ( size >= 4 )
	{
		// ToDo: Modify SvgButton to receive an array of asset names
		Q_ASSERT(label == nullptr);
		label = new SvgButton("up_variable_name_middle", memoryRow->getScene(), variable->name);
		label->setMarginLeft(0.15);
		label->setMarginTop( refDataMargins[refLabelTop] );
		label->setMarginBottom( refDataMargins[refLabelBottom] );
		addItem(label, 2.0 / size, memoryRow->getZValue() + zLabelOffset);
	}

	// Variable value
	qreal valueProportion = size - 1.0;
	if ( label )
		valueProportion -= 2.0;

	Q_ASSERT(value == nullptr);
	value = new LabelButton( variable->value, memoryRow->getScene() );
	value->setMarginTop( refDataMargins[refLabelTop] );
	value->setMarginBottom( refDataMargins[refLabelBottom] );
	value->alignRight();
	addItem(value, valueProportion / size, memoryRow->getZValue() + zValueOffset);

	// ToDo: If dataType is pointer, we can save a pointer to the pointed data, but it may be
	// more realistic to find the pointed data any time it is required

	return true;
}

bool GraphicVariable::buildArray()
{
	// We use the same proportions of structs for arrays
	applyDataTypeMargins(refStructMargins[refMarginTop], refStructMargins[refMarginBottom]);

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
	return buildMultiByteVariable("up_struct", refStructMargins);
}
