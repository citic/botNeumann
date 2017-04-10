#include "StandardInputOutput.h"
#include "Actor.h"
#include "Assets.h"
#include "Common.h"
#include "ExecutionThread.h"
#include "GraphicValue.h"
#include "LinearLayout.h"
#include "LogManager.h"
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
		GraphicValue* character = new GraphicValue(typeChar, this, zBuffer, text.mid(fillCursor++, 1));
		character->buildGraphicValue();
		characters.append(character);

		// Characters are slighly inclined, adjust their margins to overlap themselves
		character->setMargins(0.0, -0.11);

		// To animate them arriving, we place them at the first non visible position
		qreal finalPercent = qreal(charCounter) / capacity;
		insertItem(character, finalPercent + 1.0, 1.0 / capacity, zBuffer);

		// The duration of the animation is proportional to the number of spaces that characters
		// have to move, i.e: empty spaces
		/*int duration =*/ character->animateMoveTo( finalPercent, charsToFill * 250 );
	}

	this->updateLayoutItem();
	return 0;
}

int InputOutputBuffer::animateRead(int length, const QList<ExecutionThread*>& waitingQueue)
{
	int threadId = 0;
	if ( waitingQueue.count() > 0 )
		threadId = waitingQueue[0]->getId();
	qCCritical(logTemporary()) << "ToDo: animating read of" << length << "bytes towards thread" << threadId;
	return 0;
}

int InputOutputBuffer::clear()
{
	// Characters in tube are not animated to disappear, because that animation does not convey
	// any meaning. They disappear when they are discarded
	removeAllItems(true);
	characters.clear();
	fillCursor = 0;
	cursor = 0;
	return 0;
}


// StandardInputOutput class ----------------------------------------------------------------------

const char* const ioTypeStr[] =
{
	"input",
	"output",
	"error",
	"",
};

StandardInputOutput::StandardInputOutput(Type type, Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Horizontal)
	, type(type)
{
	buildStandardInputOutput();
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

bool StandardInputOutput::updateCursor(int cursor, const QList<ExecutionThread*>& waitingQueue, int& maxDuration)
{
	// Calculate how many bytes were read or written
	Q_ASSERT(buffer);
	int difference = cursor - buffer->getCursor();

	// If we are at the same cursor, nothing to animate
	if ( difference == 0 )
		return false;

	// There is difference
	// ToDo: We assume no seeking (i.e. player does not call fseek() or seekg())
	Q_ASSERT(difference > 0);

	// According to the type of this tube, we animate a read or write operation
	int duration = 0;
	if ( type == standardInput )
		duration = buffer->animateRead(difference, waitingQueue);
	else
		/*duration = buffer->animateWrite(difference)*/;

	if ( duration > maxDuration )
		maxDuration = duration;

	return duration >= 0;
}

void StandardInputOutput::buildStandardInputOutput()
{
	Q_ASSERT(scene);
	Q_ASSERT(type == standardInput || type == standardOutput);

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

	// Map the type to a string
	Q_ASSERT(type < standardIoUnknown);
	const char* typeStr = ioTypeStr[type];

	// A tube has three parts: left, middle, and right
	Actor* left = new Actor(QString("up_standard_%1_left").arg(typeStr), scene);
	Actor* middle = new Actor(QString("up_standard_%1_middle").arg(typeStr), scene);
	Actor* coupling = new Actor(QString("up_standard_input_output_coupling").arg(typeStr), scene);
	Actor* right = new Actor(QString("up_standard_%1_right").arg(typeStr), scene);

	// Ugly fix: the coupling is part of the opening, but it must be placed in a higher layer
	const qreal couplingStart = (refOpeningWidth - refCouplingWidth) / refOpeningWidth * openingProportion;
	const qreal couplingPercent = refCouplingWidth / refOpeningWidth * openingProportion;
	coupling->setMarginBottom(1.0 - refCouplingHeight / refOpeningHeight);

	// But proportions vary depending on the type of stream
	if ( type == standardInput )
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
		qreal start = 0.0;
		insertItem(left, start, elbowProportion, zElbow);
		start += elbowProportion;

		// Tester
		tester = new Actor(QString("up_standard_output_test_inactive"), scene);
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
	buildBuffer(bufferSize, scene);
}

void StandardInputOutput::buildBuffer(size_t bufferSize, Scene* scene)
{
	// Build the area to show characters
	Q_ASSERT(buffer == nullptr);
	buffer = new InputOutputBuffer(scene, zBuffer, bufferSize);
	buffer->setMarginTop( (refTubeHeight - refBufferTop) / refTubeHeight );
	const qreal bufferMarginLeft = refBufferLeft / refTubeWidth / 2.0 + 0.02;
	const qreal bufferMarginRight = (refTubeWidth - refBufferRight) / refTubeWidth / 2.0 + 0.025;
	buffer->setMarginLeft( type == standardInput ? bufferMarginLeft : bufferMarginRight );
	buffer->setMarginRight( type == standardInput ? bufferMarginRight : bufferMarginLeft );
	buffer->setMarginBottom( refBufferBottom / refTubeHeight) ;
	addItem(buffer, 1.0, zBuffer);
}
