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
	Prop* backgroundTube = new Prop("up_standard_output_middle", scene);
	addItem(backgroundTube, 1.0, zUnitPlaying::testCases + 0.0);
}
