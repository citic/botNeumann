#include "StandardInputOutput.h"
#include "Common.h"
#include "Prop.h"
#include "Scene.h"
#include "Unit.h"

StandardInputOutput::StandardInputOutput(const QString& type, Unit& unit, Scene* scene)
	: MemorySegment(unit, scene, Qt::Horizontal)
	, type(type)
{
	Q_ASSERT(type == "input" || type == "output");
	buildStandardInputOutput();
}

void StandardInputOutput::buildStandardInputOutput()
{
	Q_ASSERT(scene);

	// Calculate the percent size of a byte of the data segment
	size_t rowSize = unit.getDataSegmentSize() / unit.getDataSegmentRows();
	double byteWidth = 1.0 / (rowSize + 2);

	// Adjust the byte width because there are two stdin/stdout tubes sharing the width of a data
	// segment row
	byteWidth *= 2;

	// Standard input/output tubes have fixed width parts and variable length parts
	// The opening can extract 8-bytes values
	double opening = 8 * byteWidth;

	// The elbow requires almost a pair of bytes
	double elbow = 2.0 * byteWidth;

	// ToDo: review the z-order mechanism
	const double zValue = zUnitPlaying::standardInputOutput;

	// A tube has three parts: left, middle, and right
	Prop* left = new Prop(QString(":/unit_playing/standard_%1_left.svg").arg(type), scene);
	Prop* middle = new Prop(QString(":/unit_playing/standard_%1_middle.svg").arg(type), scene);
	Prop* right = new Prop(QString(":/unit_playing/standard_%1_right.svg").arg(type), scene);

	// But proportions vary depending on the type of stream
	if ( type == "input" )
	{
		addItem(left, opening, zValue);
		addItem(middle, 1.0 - opening - elbow, zValue);
		addItem(right, elbow, zValue);
	}
	else
	{
		addItem(left, elbow, zValue);
		Prop* tester = new Prop(QString(":/unit_playing/standard_output_test_inactive.svg"), scene);
		addItem(tester, elbow, zValue);
		addItem(middle, 1.0 - opening - 2 * elbow, zValue);
		addItem(right, opening, zValue);

		// Make the middle tube to plug to the tester
		middle->setMarginLeft(-0.011);
	}
}
