#include "GraphicValue.h"
#include "Assets.h"
#include "BotNeumannApp.h"
#include "Prop.h"
#include "Scene.h"

GraphicValue::GraphicValue(DataType dataType, Scene* scene, qreal zValue)
	: LinearLayout(Qt::Horizontal)
	, dataType(dataType)
	, scene(scene)
	, zValue(zValue)
{
}

VisAddress GraphicValue::getSize() const
{
	// ToDo: depends on the architecture of the botnu file

	// Depends on the type of this variable:
	switch ( dataType )
	{
		case typeUnknown: Q_ASSERT(false); return 0;
		case typeVoid: Q_ASSERT(false); return 0;
		case typeBool: return sizeof(bool);
		case typeChar: return sizeof(char);
		case typeInt: return sizeof(int);
		case typeEnum: return sizeof(unsigned);
		case typeBitField: return sizeof(unsigned);
		case typeFloat: return sizeof(float);
		case typePointer: return sizeof(void*);
		case typeReference: return sizeof(void*);
		case typeRValue: return sizeof(void*);
		case typeFunction: Q_ASSERT(false); return 0;

		// ToDo: sizeof of composite types must be calculated recursively or passed by parameter
		case typeArray: Q_ASSERT(false); return 0;
		case typeStruct: Q_ASSERT(false); return 0;
		case typeClass: Q_ASSERT(false); return 0;
		case typeUnion: Q_ASSERT(false); return 0;
	}

	Q_ASSERT(false);
	return 1;
}

bool GraphicValue::buildGraphicValue(DataType dataType)
{
	this->dataType = dataType;

	// Create the pod, the value, and the label of the variable
	switch ( dataType )
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

bool GraphicValue::buildSingleByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins);

	// Pod:
	// A single-byte variable requires just a prop
	Prop* pod = new Prop(asset, scene);
	addItem(pod, 1.0, zValue + zPodOffset );

	// ToDo: draw value

	// No variable label
	return true;
}

bool GraphicValue::buildMultiByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins);
	buildPod(asset, true, true);
	buildValue(refDataMargins);
	return true;
}

bool GraphicValue::buildArray()
{
	Q_ASSERT(false);
	return false;
}

bool GraphicValue::buildStruct()
{
	Q_ASSERT(false);
	return false;
}

void GraphicValue::applyDataTypeMargins(const qreal refDataMargins[])
{
	setMarginTop( refDataMargins[refMarginTop] );
	setMarginBottom( refDataMargins[refMarginBottom] );
}

bool GraphicValue::buildPod(const QString& asset, bool buildLeftPod, bool buildRightPod)
{
	// Left and right parts always require 1 byte, middle the remaining bytes
	const qreal zPod = zValue + zPodOffset;
	const VisAddress size = getSize();
	const QString& svgFileBase = ":/unit_playing/data_types/" + asset;

	// Pod: left
	Q_ASSERT(podLeft == nullptr);
	if ( buildLeftPod )
	{
		podLeft = asset.startsWith("up_")
				? new ScenicElement(asset + "_left", scene)
				: new ScenicElement(svgFileBase + "_left.svg", scene, true);

		addItem(podLeft, 1.0 / size, zPod);
	}

	// Pod: middle
	podMiddle = asset.startsWith("up_")
			? new ScenicElement(asset + "_middle", scene)
			: new ScenicElement(svgFileBase + "_middle.svg", scene, true);
	qreal middleSize = size - (VisAddress)buildLeftPod - (VisAddress)buildRightPod;
	addItem(podMiddle, middleSize / size, zPod);

	// Pod: right
	if ( buildRightPod )
	{
		podRight = asset.startsWith("up_")
				? new ScenicElement(asset + "_right", scene)
				: new ScenicElement(svgFileBase + "_right.svg", scene, true);
		addItem(podRight, 1.0 / size, zPod);
	}

	return true;
}

bool GraphicValue::buildValue(const qreal refDataMargins[])
{
	return false;
}
