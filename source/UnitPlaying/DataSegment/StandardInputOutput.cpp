#include "StandardInputOutput.h"
#include "Assets.h"
#include "Common.h"
#include "GraphicValue.h"
#include "LinearLayout.h"
#include "LogManager.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

#include <QBrush>
#include <QPen>
#include <QTextStream>

const qreal zTube =    zUnitPlaying::standardInputOutput + 0.3;
const qreal zElbow =   zUnitPlaying::standardInputOutput + 0.2;
const qreal zBuffer =  zUnitPlaying::standardInputOutput + 0.1;
const qreal zOpening = zUnitPlaying::standardInputOutput + 0.0;


// InputOutputBuffer class ------------------------------------------------------------------------

InputOutputBuffer::InputOutputBuffer(Scene* scene, qreal zValue, int capacity)
	: QGraphicsRectItem(scene)
	, LinearLayout(Qt::Horizontal)
	, scene(scene)
	, zValue(zValue)
	, capacity(capacity)
{
}

void InputOutputBuffer::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Update the LinearLayout part of this object
	LinearLayout::resize(left, top, width, height);

	// Update the QGraphicsRectIem part of this object
	applyMargins(left, top, width, height);
	setRect(left, top, width, height);
}

int InputOutputBuffer::animateFill()
{
	// The amount of free space that can be filled
	int charsToFill = qMin( getFreeCharacters(), getPendingCharacters() );

	// Create characters, place them and animate them arriving
	for ( int charCounter = 0; charCounter < charsToFill; ++charCounter )
	{
		GraphicValue* character = new GraphicValue(typeChar, scene, zBuffer, text.mid(cursor++, 1));
		character->buildGraphicValue(typeChar);
		addItem(character, 1.0 / (capacity + 3), zBuffer);
		characters.append(character);
	}

	this->updateLayoutItem();
	return 0;
}

int InputOutputBuffer::animateRead(int length)
{
	Q_UNUSED(length);
	return 0;
}


// StandardInputOutput class ----------------------------------------------------------------------

StandardInputOutput::StandardInputOutput(const QString& type, Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Horizontal)
{
	buildStandardInputOutput(type);
}

bool StandardInputOutput::loadFile(const QString& filepath)
{
	// Open the test case's standard input file
	QFile file(filepath);
	if ( file.open(QIODevice::ReadOnly | QIODevice::Text) == false )
		{ qCritical(logApplication) << "Could not open" << filepath; return false; }

	// At least one file has data, we have to read them to compare
	Q_ASSERT(buffer);
	const QString& text = QTextStream(&file).readAll();
	buffer->setText(text);

	animateFill();
	return true;
}

void StandardInputOutput::buildStandardInputOutput(QString type)
{
	Q_ASSERT(scene);
	Q_ASSERT(type == "input" || type == "output");

	// Calculate the percent size of a byte of the data segment
	size_t rowSize = unit.getDataSegmentSize() / unit.getDataSegmentRows();
	double byteWidth = 1.0 / (rowSize + 2);

	// Adjust the byte width because there are two stdin/stdout tubes sharing the width of a data
	// segment row
	byteWidth *= 2;

	// Standard input/output tubes have fixed width parts and variable length parts
	// The opening can extract 8-bytes values
	double openingProportion = 8 * byteWidth;

	// The elbow requires almost a pair of bytes
	double elbowProportion = 2.0 * byteWidth;

	// A tube has three parts: left, middle, and right
	Prop* left = new Prop(QString("up_standard_%1_left").arg(type), scene);
	Prop* middle = new Prop(QString("up_standard_%1_middle").arg(type), scene);
	Prop* right = new Prop(QString("up_standard_%1_right").arg(type), scene);

	// But proportions vary depending on the type of stream
	if ( type == "input" )
	{
		insertItem(left, 0.0, openingProportion, zOpening);
		const qreal middleProportion = 1.0 - openingProportion - elbowProportion;
		insertItem(middle, openingProportion, middleProportion, zTube);
		insertItem(right, openingProportion + middleProportion, elbowProportion, zElbow);
	}
	else
	{
		qreal start = 1.0; // ToDo: fix insert start proportions, this must be 0.0
		insertItem(left, start, elbowProportion, zElbow);
		start += elbowProportion;

		tester = new Prop(QString("up_standard_output_test_inactive"), scene);
		insertItem(tester, start, elbowProportion, zElbow);
		start += elbowProportion;

		const qreal middleProportion = 1.0 - openingProportion - 2 * elbowProportion;
		insertItem(middle, start, middleProportion, zTube);
		start += middleProportion;

		insertItem(right, start, openingProportion, zOpening);

		// Make the middle tube to plug to the tester
		middle->setMarginLeft(-0.011);
	}

	// The buffer has half size of the data segment row, and 3 chars are lost by elbows and edges
	const size_t bufferSize = rowSize / 2 - 3;
	buildBuffer(type, bufferSize, scene);
}

void StandardInputOutput::buildBuffer(const QString& type, size_t bufferSize, Scene* scene)
{
	// Build the area to show characters
	Q_ASSERT(buffer == nullptr);
	buffer = new InputOutputBuffer(scene, zBuffer, bufferSize);
	buffer->setMarginTop( (refTubeHeight - refBufferTop) / refTubeHeight );
	const qreal bufferMarginLeft = refBufferLeft / refTubeWidth / 2.0;
	const qreal bufferMarginRight = (refTubeWidth - refBufferRight) / refTubeWidth / 2.0 + bufferMarginLeft;
	buffer->setMarginLeft( type == "input" ? bufferMarginLeft : bufferMarginRight );
	buffer->setMarginRight( type == "input" ? bufferMarginRight : bufferMarginLeft );
	buffer->setMarginBottom( refBufferBottom / refTubeHeight) ;
	buffer->setPen(Qt::NoPen);
	buffer->setBrush(Qt::NoBrush);
	addItem(buffer, 1.0, zBuffer);
}
