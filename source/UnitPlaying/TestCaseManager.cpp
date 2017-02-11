#include "TestCaseManager.h"
#include "Common.h"
#include "PlayerSolution.h"
#include "Prop.h"
#include "Scene.h"
#include "TestCaseActor.h"

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

#include "LogManager.h"
bool TestCaseManager::testPlayerSolution()
{
	Q_ASSERT(playerSolution);

	// If there are not test cases, do nothing
	int testCasesCount = playerSolution->getTestCasesCount();
	if ( testCasesCount <= 0 )
		return false;

	// Estimate the with percent of earch tester maintaining the proportions of graphic design
	// asumming there are enough space
	const qreal testerReferenceWidth = 32.274;
	const qreal sceneReferenceWith = 768.883;
	const qreal aureaProportionFix = 0.75; // 0.6180339887499;
	const qreal testerReferenceProportion = testerReferenceWidth / sceneReferenceWith * aureaProportionFix;

	// Distribute the width percent among all the test cases
	// We reserve space at the left and right of the tube, estimated as 1 more tester
	const qreal testerRelativeProportion = 1.0 / (testCasesCount + 1);

	// When there are a few number of test cases, the percents become huge, we use the smaller
	const qreal testerWidthProportion = qMin( testerReferenceProportion, testerRelativeProportion );

	// Create a tester for each test case and run the test case
	for ( int index = 1; index <= testCasesCount; ++index )
		if ( ! createAndRunTestCase( index, testerWidthProportion ) )
			return false;

	updateLayoutItem();
	return true;
}

bool TestCaseManager::createAndRunTestCase(int index, const qreal testerWidthProportion)
{
	// Separate this tester from previous or left edge
	qreal zTesters = zUnitPlaying::testCases + 0.1;
	this->addStretch(testerWidthProportion / 2.0, zTesters);

	// Create the test case and add
	TestCaseActor* testCaseActor = new TestCaseActor(index, scene);
	this->testCases.append( testCases );
	this->addItem( testCaseActor, testerWidthProportion, zTesters );

	// ToDo: run the test
	return true;
}
