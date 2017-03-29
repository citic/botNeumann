#include "TestCaseManager.h"
#include "Actor.h"
#include "Common.h"
#include "PlayerSolution.h"
#include "Scene.h"
#include "TestCaseActor.h"
#include "LogManager.h"

#include <QMessageBox>

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
	Actor* backgroundTube = new Actor("up_standard_output_middle1", scene);
	addItem(backgroundTube, 1.0, zUnitPlaying::testCases + 0.0);
}

bool TestCaseManager::testPlayerSolution()
{
	Q_ASSERT(playerSolution);

	// If there are not test cases, do nothing
	int testCasesCount = playerSolution->getTestCasesCount();
	if ( testCasesCount <= 0 )
		return false;

	// If there are old test cases, remove them
	clearAnimation();

	// Create a new layer for the new test cases
	Q_ASSERT(testCaseLayout == nullptr);
	testCaseLayout = new LinearLayout(Qt::Horizontal);
	addLayout(testCaseLayout, 1.0);

	// Estimate the with percent of earch tester maintaining the proportions of graphic design
	// asumming there are enough space
	const qreal testerReferenceWidth = 32.274;
	const qreal sceneReferenceWith = 768.883;
	const qreal aureaProportionFix = 0.75; // 0.6180339887499;
	const qreal testerReferenceProportion = testerReferenceWidth / sceneReferenceWith * aureaProportionFix;

	// Distribute the width percent among all the test cases
	// We reserve space at the left and right of the tube, estimated as 1 more tester
	// The x percent of each tester is x = 2/(3n+1)
	const qreal testerRelativeProportion = 2.0 / ( 3 * testCasesCount + 1);

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
	testCaseLayout->addStretch(testerWidthProportion / 2.0, zTesters);

	// Create the test case and add it to the scene
	TestCaseActor* testCaseActor = new TestCaseActor(index, scene);
	testCaseLayout->addItem( testCaseActor, testerWidthProportion, zTesters );

	// Add the test case to the list of actors
	testCaseActors.append(testCaseActor);
	Q_ASSERT( index == testCaseActors.count() );

	// When the test case actor is activated, user wants to change the active test case
	connect( testCaseActor, SIGNAL(testCaseActivated(int)), this, SLOT(setActiveTestCase(int)) );

	// The active test case does not show the result because it is being visualized
	testCaseActor->setActiveTestCase( index == activeTestCase );

	// Run the player solution against the test
	return testCaseActor->testPlayerSolution(playerSolution);
}

void TestCaseManager::clearAnimation()
{
	// If there are not test cases, done
	if ( testCaseLayout == nullptr )
		return;

	testCaseLayout->removeAllItems(true);
	removeItem(testCaseLayout, true);
	testCaseLayout = nullptr;
	testCaseActors.clear();
}

void TestCaseManager::setActiveTestCase(int newTestCaseNumber)
{
	// If user pressed the current test case being visualized, ignore the event
	if ( activeTestCase == newTestCaseNumber )
		return;

	// Ask confirmation, if user cancels, ignore the event
	const QString& text = QString("Do you want to visualize test case %1?\nThis will stop current visualization.").arg(newTestCaseNumber);
	if ( QMessageBox::question(nullptr, tr("Change test case?"), text) != QMessageBox::Yes )
		return;

	// User confirmed, tell the old test case it is not active now
	Q_ASSERT( activeTestCase - 1 < testCaseActors.count() );
	testCaseActors[activeTestCase - 1]->setActiveTestCase(false, true);
	qCInfo( logPlayer, "Test case changed from %d to %d", activeTestCase, newTestCaseNumber);

	// Make the new test case the active one
	activeTestCase = newTestCaseNumber;
	Q_ASSERT( newTestCaseNumber - 1 < testCaseActors.count() );
	testCaseActors[newTestCaseNumber - 1]->setActiveTestCase(true, true);

	// Alert other objects that we shall start a new visualization
	emit activeTestCaseChanged(activeTestCase);
}
