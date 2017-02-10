#include "TestCaseManager.h"
#include "Common.h"
#include "Prop.h"
#include "Scene.h"

TestCaseManager::TestCaseManager(Scene* scene, QObject* parent)
	: QObject(parent)
	, LinearLayout(Qt::Horizontal)
	, scene(scene)
{
	buildTestCaseManager();
}

void TestCaseManager::buildTestCaseManager()
{
	// We use the "up_standard_output_middle1" graphic that does not contain a bottom empty space
	// required by standard input/output
	Prop* backgroundTube = new Prop("up_standard_output_middle1", scene);
	addItem(backgroundTube, 1.0, zUnitPlaying::testCases + 0.0);
}
