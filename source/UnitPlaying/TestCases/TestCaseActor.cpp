#include "TestCaseActor.h"
#include "PlayerSolution.h"
#include "LogManager.h"

TestCaseActor::TestCaseActor(int index, QGraphicsItem* parentItem)
	: Actor("up_standard_output_test_inactive1", parentItem)
	, index(index)
{
	Q_ASSERT(index >= 1);
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

	// ToDo: read arguments from file
	QStringList arguments;

	// Create an object to call the player solution executable
	if ( process ) process->deleteLater();
	process = new QProcess(this);

	// When the player solution has finished, call a method to check results
	connect( process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(playerSolutionFinished(int,QProcess::ExitStatus)) );
	qCInfo(logTemporary) << qPrintable(playerSolution->getExecutablePath()) << arguments << "< " << qPrintable(input) << " > " << qPrintable(output_ps) << " 2> " << qPrintable(error_ps);

	// Call the player solution
	process->setReadChannelMode(QProcess::SeparateChannels);
	process->setStandardInputFile( input );
	process->setStandardOutputFile( output_ps );
	process->setStandardErrorFile( error_ps );
	process->start( playerSolution->getExecutablePath(), arguments );

	// ToDo: Set a timeout using Unit::getTimeout()

	return true;
}

void TestCaseActor::playerSolutionFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);
	// ToDo: compare the output generated by player solution considering whitespace and case sensitivity

	// Todo: compare standard error too

	// If player solution's output matches expected output, turn this actor green
	// else turn this actor red

	qCInfo(logTemporary) << "Finished: " << qPrintable(playerSolution->getExecutablePath()) /*<< arguments*/ << "< " << qPrintable(input) << " > " << qPrintable(output_ps) << " 2> " << qPrintable(error_ps);
}
