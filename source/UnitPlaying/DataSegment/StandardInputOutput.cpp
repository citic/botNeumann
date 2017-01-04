#include "StandardInputOutput.h"
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

	// The elbow requires 1.5 bytes
	double elbow = 1.5 * byteWidth;

	// ToDo: review the z-order mechanism
	const double zStdInOut = 0.1;

	// A tube has three parts: left, middle, and right
	Prop* left = new Prop(QString(":/unit_playing/standard_%1_left.svg").arg(type), scene);
	Prop* middle = new Prop(QString(":/unit_playing/standard_%1_middle.svg").arg(type), scene);
	Prop* right = new Prop(QString(":/unit_playing/standard_%1_right.svg").arg(type), scene);

	// But proportions vary depending on the type of stream
	if ( type == "input" )
	{
		addItem(left, opening, zStdInOut);
		addItem(middle, 1.0 - opening - elbow, zStdInOut);
		addItem(right, elbow, zStdInOut);
	}
	else
	{
		addItem(left, elbow, zStdInOut);
		Prop* test = new Prop(QString(":/unit_playing/standard_output_test_inactive.svg"), scene);
		addItem(test, elbow, zStdInOut);
		addItem(middle, 1.0 - opening - 2 * elbow, zStdInOut);
		addItem(right, opening, zStdInOut);
	}
}
