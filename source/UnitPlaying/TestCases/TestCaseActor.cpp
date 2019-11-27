#include "TestCaseActor.h"
#include "PlayerSolution.h"
#include "LogManager.h"
#include "Unit.h"
#include "Util.h"

TestCaseActor::TestCaseActor(int index, QGraphicsItem* parentItem)
	: Actor("up_standard_output_test_inactive1", parentItem)
	, index(index)
{
	Q_ASSERT(index >= 1);
	setFlag( QGraphicsItem::ItemIsSelectable );
	 updateToolTip("running");
}

bool TestCaseActor::testPlayerSolution(PlayerSolution* playerSolution)
{
	// Copy player solution for later use
	this->playerSolution = playerSolution;

	// Build the paths for this test case
	args      = playerSolution->buildTestCaseFilepath(index, "args");
	input     = playerSolution->buildTestCaseFilepath(index, "input");
	output_ex = playerSolution->buildTestCaseFilepath(index, "output_ex");
	output_ps = playerSolution->buildTestCaseFilepath(index, "output_ps");
	error_ex  = playerSolution->buildTestCaseFilepath(index, "error_ex");
	error_ps  = playerSolution->buildTestCaseFilepath(index, "error_ps");

	// Read arguments from file, one argument per line
	QStringList arguments = Util::readAllLines(args);

	// Create an object to call the player solution executable
	if ( process ) process->deleteLater();
	process = new QProcess(this);

	// When the player solution has finished, call a method to check results
	connect( process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(playerSolutionFinished(int,QProcess::ExitStatus)) );
	qCDebug(logTemporary) << qPrintable(playerSolution->getExecutablePath()) << arguments << "<" << qPrintable(input) << ">" << qPrintable(output_ps) << "2>" << qPrintable(error_ps);

	// Call the player solution
	process->setProcessChannelMode(QProcess::SeparateChannels);
	process->setStandardInputFile( input );
	process->setStandardOutputFile( output_ps );
	process->setStandardErrorFile( error_ps );
	process->start( playerSolution->getExecutablePath(), arguments );

	// Get the maximum time for player solution allowed to run
	Q_ASSERT(playerSolution->getUnit());
	int timeout = playerSolution->getUnit()->getTimeout();

	// Start a timer to know if player solution has exceed the allowed running time (timeout)
	if ( timeout > 0 )
		CREATE_TIMER(timeout, this, &TestCaseActor::playerSolutionTimeout)

	// Done
	return true;
}

void TestCaseActor::playerSolutionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode)
	Q_UNUSED(exitStatus)
	Q_ASSERT(playerSolution);
	Q_ASSERT(playerSolution->getUnit());

	// Unit knows if comparison must be done in case-sensitive and ignorign whitespace
	bool ignoreWhitespace = playerSolution->getUnit()->shouldIgnoreWhitespace();
	bool caseSensitive = playerSolution->getUnit()->shouldCompareCaseSenstive();

	// A human reason in case of test cases failed
	QString reason;

	// Compare the output generated by player solution considering whitespace and case sensitivity
	// We get the first character where the files differ, -1 if they are equals
	firstDiffOutput = Util::findFirstDiff(output_ex, output_ps, ignoreWhitespace, caseSensitive);
	firstDiffError = Util::findFirstDiff(error_ex, error_ps, ignoreWhitespace, caseSensitive);

	// If there is difference, give some human readable reason
	if ( firstDiffOutput >= 0 )
		reason = "Standard output mismatch";
	if ( firstDiffError >= 0 )
	{
		if ( ! reason.isEmpty() )
			reason += " and ";
		reason += "Standard error mismatch";
	}

	// If player solution's output matches expected output, turn this actor green
	// else turn this actor red
	bool testCasePassed = firstDiffOutput == -1 && firstDiffError == -1;
	setTestCaseResult( testCasePassed ? passed : failed, reason);
}

void TestCaseActor::playerSolutionTimeout()
{
	// If player solution already finished is ok, but it is still running we have to kill it
	Q_ASSERT(process);
	if ( process->state() == QProcess::Running )
	{
		// Kill the process, the test case failed by timeout
		process->kill();
		setTestCaseResult(failed, "Timeout");
	}
}

void TestCaseActor::setTestCaseResult(TestCaseActor::TestCaseResult result, const QString& reason)
{
	// Copy the result internally
	this->result = result;
	this->failReason = reason;

	// Update the graphical interface to the result
	reflectTestCaseResult(true);

	if ( ! failReason.isEmpty() )
		qCInfo( logPlayer, "Test case %d failed: %s", index, qUtf8Printable(failReason) );
}

void TestCaseActor::reflectTestCaseResult(bool hideActiveTestCaseResult)
{
	TestCaseResult state = result;
	if ( hideActiveTestCaseResult && isTheActiveTestCase )
		state = unknown;

	switch (state)
	{
		case unknown: setElementId("up_standard_output_test_inactive1"); updateToolTip("?"); break;
		case passed: setElementId("up_standard_output_test_valid1"); updateToolTip("passed"); break;
		case failed: setElementId("up_standard_output_test_invalid1"); updateToolTip("failed"); break;
	}
}

void TestCaseActor::updateToolTip(const QString& result)
{
	setToolTip(tr("Test case %1 (%2)\nClick for visualizing it").arg(index).arg(result));
}

void TestCaseActor::setActiveTestCase(bool active, bool updateInterface)
{
	isTheActiveTestCase = active;
	if ( updateInterface )
		reflectTestCaseResult(true);
}

void TestCaseActor::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	Q_ASSERT(index > 0);
	emit testCaseActivated(index);
	Actor::mouseReleaseEvent(event);
}
