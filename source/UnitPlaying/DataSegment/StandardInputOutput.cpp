#include "StandardInputOutput.h"
#include "Prop.h"
#include "Scene.h"

StandardInputOutput::StandardInputOutput(const QString& type, Scene* scene)
	: LinearLayout(Qt::Horizontal)
	, type(type)
	, scene(scene)
{
	Q_ASSERT(type == "input" || type == "output");
	buildStandardInputOutput();
}

void StandardInputOutput::buildStandardInputOutput()
{
	Q_ASSERT(scene);

	// ToDo: review the z-order mechanism
	const double zStdInOut = 0.1;

	// A tube has three parts: left, middle, and right
	Prop* left = new Prop(QString(":/unit_playing/standard_%1_left.svg").arg(type), scene);
	Prop* middle = new Prop(QString(":/unit_playing/standard_%1_middle.svg").arg(type), scene);
	Prop* right = new Prop(QString(":/unit_playing/standard_%1_right.svg").arg(type), scene);

	// But proportions vary depending on the type of stream
	if ( type == "input" )
	{
		addItem(left, 0.92 / 2, zStdInOut);
		addItem(middle, 0.92 / 2, zStdInOut);
		addItem(right, 0.08, zStdInOut);
	}
	else
	{
		addItem(left, 0.08, zStdInOut);
		Prop* test = new Prop(QString(":/unit_playing/standard_output_test_inactive.svg"), scene);
		addItem(test, 0.08, zStdInOut);
		addItem(middle, 0.84 / 2, zStdInOut);
		addItem(right, 0.84 / 2, zStdInOut);
	}
}
