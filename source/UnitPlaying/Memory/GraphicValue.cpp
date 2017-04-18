#include "GraphicValue.h"
#include "Actor.h"
#include "Assets.h"
#include "BotNeumannApp.h"
#include "LabelButton.h"

#include <QBrush>
#include <QFont>

GraphicValue::GraphicValue(DataType dataType, QGraphicsItem* graphicsParent, qreal zValue, const QString& value)
	: LinearLayoutActor(Qt::Horizontal)
	, dataType(dataType)
	, graphicsParent(graphicsParent)
	, zValue(zValue)
	, value(value)
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

void GraphicValue::setValue(const QString& value)
{
	this->value = value;
	if ( valueLabel )
		valueLabel->setText( value, true );
}

int GraphicValue::animateValueChange(const QString& value)
{
	this->value = value;
	if ( valueLabel )
		return valueLabel->animateSetText(value);
	return 0;
}

bool GraphicValue::buildGraphicValue()
{
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

void GraphicValue::setZ(qreal newZValue)
{
	LinearLayoutActor::setZ(newZValue);
	if ( podLeft ) podLeft->setZValue(newZValue + zPodOffset);
	if ( podMiddle ) podMiddle->setZValue(newZValue + zPodOffset);
	if ( podRight ) podRight->setZValue(newZValue + zPodOffset);

	if ( valueLabel ) valueLabel->setZValue(newZValue + zLabelValueOffset);
}

bool GraphicValue::buildSingleByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins);

	// Pod:
	// A single-byte variable requires just one graphic
	Q_ASSERT(podMiddle == nullptr);
	podMiddle = new Actor(asset, graphicsParent);
	addItem(podMiddle, 1.0, zValue + zPodOffset );

	// Characters can contain a single value
	buildValueLabel(refDataMargins);

	// No variable name
	return true;
}

bool GraphicValue::buildMultiByteVariable(const QString& asset, const qreal refDataMargins[])
{
	// Apply margins according to the height of the data type and the nesting on composite data types
	applyDataTypeMargins(refDataMargins);
	buildPod(asset, true, true);
	buildValueLabel(refDataMargins);
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
				? new Actor(asset + "_left", graphicsParent)
				: new Actor(svgFileBase + "_left.svg", graphicsParent, true);

		addItem(podLeft, 1.0 / size, zPod);
	}

	// Pod: middle
	podMiddle = asset.startsWith("up_")
			? new Actor(asset + "_middle", graphicsParent)
			: new Actor(svgFileBase + "_middle.svg", graphicsParent, true);
	qreal middleSize = size - (VisAddress)buildLeftPod - (VisAddress)buildRightPod;
	addItem(podMiddle, middleSize / size, zPod);

	// Pod: right
	if ( buildRightPod )
	{
		podRight = asset.startsWith("up_")
				? new Actor(asset + "_right", graphicsParent)
				: new Actor(svgFileBase + "_right.svg", graphicsParent, true);
		addItem(podRight, 1.0 / size, zPod);
	}

	return true;
}

bool GraphicValue::buildValueLabel(const qreal refDataMargins[], qreal proportion)
{
	// Variable value
	Q_ASSERT(valueLabel == nullptr);
	QString visibleValue = value;
	if ( dataType == typeChar )
		visibleValue = processInvisibleChars();
	valueLabel = new LabelButton( visibleValue, graphicsParent );
	valueLabel->setMarginTop( refDataMargins[refLabelTop] + 0.1 );
	valueLabel->setMarginBottom( refDataMargins[refLabelBottom] );
	if ( dataType == typeChar )
	{
		//valueLabel->alignCenter();
		valueLabel->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
		valueLabel->setMarginLeft(1.0 / 3.0);
		valueLabel->setMarginRight(0.1);
	}
	else
		valueLabel->alignRight();
	valueLabel->setBrush(QBrush(Qt::black));
	addItem(valueLabel, proportion, zValue + zLabelValueOffset);
	return true;
}

QString GraphicValue::processInvisibleChars()
{
	QString result;
	for ( int index = 0; index < value.length(); ++index )
	{
		switch ( value[index].toLatin1() )
		{
			case '\n': result += "\\n"; break; // newline
			case '\t': result += "\\t"; break; // horizontal tab
			case '\\': result += "\\\\"; break; // backslash
			case '\r': result += "\\r"; break; // carriage return
			case '\a': result += "\\a"; break; // alert
			case '\b': result += "\\b"; break; // backspace
			case '\f': result += "\\f"; break; // formfeed
			case '\v': result += "\\v"; break; // vertical tab

			default: result += value[index];
		}
	}

	return result;
}
