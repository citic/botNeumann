#include "StandardInputOutput.h"
#include "Assets.h"
#include "Common.h"
#include "GraphicValue.h"
#include "LinearLayout.h"
#include "LogManager.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

#include <QTextStream>

const qreal zTube =    zUnitPlaying::standardInputOutput + 0.3;
const qreal zElbow =   zUnitPlaying::standardInputOutput + 0.2;
const qreal zBuffer =  zUnitPlaying::standardInputOutput + 0.1;
const qreal zOpening = zUnitPlaying::standardInputOutput + 0.0;


// InputOutputBuffer class ------------------------------------------------------------------------

InputOutputBuffer::InputOutputBuffer(Scene* scene, qreal zValue, int capacity)
	: RectLayoutItem(Qt::Horizontal, zValue, scene)
	, capacity(capacity)
{
}

int InputOutputBuffer::animateFill()
{
	// The amount of free space that can be filled
	int charsToFill = qMin( getFreeCharacters() + 1, getPendingCharacters() );

	// Create characters, place them and animate them arriving
	for ( int charCounter = 0; charCounter < charsToFill; ++charCounter )
	{
		// Create the character as a value
		GraphicValue* character = new GraphicValue(typeChar, this, zBuffer, text.mid(cursor++, 1));
		character->buildGraphicValue();
		characters.append(character);

		// Characters are slighly inclined, adjust their margins to overlap themselves
		character->setMargins(0.0, -0.11);

		// To animate them arriving, we place them at the first not visible position
		qreal finalPercent = getMarginLeft() + 0.015 + qreal(charCounter) / capacity;
		insertItem(character, finalPercent + 1.0, 1.0 / capacity, zBuffer);

		// The duration of the animation is proportional to the number of spaces that characters
		// have to move, i.e: empty spaces
		/*int duration =*/ character->animateMoveTo( finalPercent, charsToFill * 250 );
	}

	this->updateLayoutItem();
	return 0;
}

int InputOutputBuffer::animateRead(int length)
{
	Q_UNUSED(length);
	return 0;
}

int InputOutputBuffer::clear()
{
	// Characters in tube are not animated to disappear, because that animation does not convey
	// any meaning. They disappear when they are discarded
	removeAllItems(true);
	characters.clear();
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
	Prop* coupling = new Prop(QString("up_standard_input_output_coupling").arg(type), scene);
	Prop* right = new Prop(QString("up_standard_%1_right").arg(type), scene);

	// Ugly fix: the coupling is part of the opening, but it must be placed in a higher layer
	const qreal couplingStart = (refOpeningWidth - refCouplingWidth) / refOpeningWidth * openingProportion;
	const qreal couplingPercent = refCouplingWidth / refOpeningWidth * openingProportion;
	coupling->setMarginBottom(1.0 - refCouplingHeight / refOpeningHeight);

	// But proportions vary depending on the type of stream
	if ( type == "input" )
	{
		// Opening
		insertItem(left, 0.0, openingProportion, zOpening);

		// Middle tube
		const qreal middleProportion = 1.0 - openingProportion - elbowProportion;
		insertItem(middle, openingProportion, middleProportion, zTube);

		// Coupling: ugly fix (it should be alone, currently it is part of the opening)
		insertItem(coupling, couplingStart, couplingPercent, zTube);

		// Elbow
		insertItem(right, openingProportion + middleProportion, elbowProportion, zElbow);
	}
	else
	{
		// Elbow
		qreal start = 1.0; // ToDo: fix insert start proportions, this must be 0.0
		insertItem(left, start, elbowProportion, zElbow);
		start += elbowProportion;

		// Tester
		tester = new Prop(QString("up_standard_output_test_inactive"), scene);
		insertItem(tester, start, elbowProportion, zElbow);
		start += elbowProportion;

		// Middle tube
		const qreal middleProportion = 1.0 - openingProportion - 2 * elbowProportion;
		insertItem(middle, start, middleProportion, zTube);
		start += middleProportion;

		// Opening
		insertItem(right, start, openingProportion, zOpening);

		// Coupling: ugly fix (it should be alone, currently it is part of the opening)
		insertItem(coupling, start, couplingPercent, zTube);

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
	const qreal bufferMarginLeft = refBufferLeft / refTubeWidth / 2.0 + 0.02;
	const qreal bufferMarginRight = (refTubeWidth - refBufferRight) / refTubeWidth / 2.0 + 0.025;
	buffer->setMarginLeft( type == "input" ? bufferMarginLeft : bufferMarginRight );
	buffer->setMarginRight( type == "input" ? bufferMarginRight : bufferMarginLeft );
	buffer->setMarginBottom( refBufferBottom / refTubeHeight) ;
	addItem(buffer, 1.0, zBuffer);
}
