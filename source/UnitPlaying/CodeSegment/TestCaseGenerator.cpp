#include "TestCaseGenerator.h"
#include "LogManager.h"
#include "PlayerSolution.h"
#include "Unit.h"

#include <QProcess>

TestCaseGenerator::TestCaseGenerator(PlayerSolution* playerSolution, Unit* unit, QObject* parent)
	: CompiledProgram(parent)
	, playerSolution(playerSolution)
	, unit(unit)
{
}

bool TestCaseGenerator::generate(const ProgramText* generator, int testCaseStartIndex)
{
	// We need the starting index when the building process finishes
	this->currentTestCaseIndex = testCaseStartIndex;

	// Compile the generator's source code. It requires some time and we do not wait until it
	// finishes. When the compilation and linking process has finished, our compilerFinished()
	// is called and we continue processing the results there
	connect(this, SIGNAL(buildFinished()), this, SLOT(buildFinished()));
	build( generator, playerSolution->getPlayerUnitPath() );

	// ToDo: If the generator used is a standard one, it will require a solution later
	return true;
}

bool TestCaseGenerator::buildFinished()
{
	// We can only generate test cases if generator's source code compiled without errors
	if ( getErrorCount() > 0 )
		return logErrors();

	// Generator compiled with no errors, run it for each test case
	Q_ASSERT(currentTestCaseIndex >= 0);
	int lastIndex = currentTestCaseIndex + programText->defaultRuns;
	for ( ; currentTestCaseIndex < lastIndex; ++currentTestCaseIndex )
	{
		// Prepare the arguments to call the generators
		number    = QString("%1").arg(currentTestCaseIndex + 1, 2, 10, QLatin1Char('0'));
		total     = QString("%1").arg(lastIndex, 2, 10, QLatin1Char('0'));
		args      = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "args");
		input     = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "input");
		output_ex = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "output_ex");
		error_ex  = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "error_ex");

		// Call the generator according to its type
		switch ( programText->type )
		{
			case ProgramText::standardGenerator: callStandarGenerator(); break;
			case ProgramText::fileGenerator: callFileGenerator(); break;
			default: Q_ASSERT(false); break;
		}
	}

	// Inform that test cases were generated
	emit generationFinished();
	return true;
}

bool TestCaseGenerator::callStandarGenerator()
{
	// Call bash -c bn_gen 02 10 > input.txt 2> args.txt
	const QString& genCall = QString("\"%1\" \"%2\" \"%3\" > \"%4\" 2> \"%5\"")
		.arg( executablePath )
		.arg( number )
		.arg( total )
		.arg( input )
		.arg( args );

	// qCCritical(logTemporary) << "bash " << qPrintable( (QStringList() << "-c" << genCall).join(' ') );
	// process->start( "bash", QStringList() << "-c" << genCall );
	if ( system( qPrintable(genCall) ) != 0 )
	{
		qCCritical(logApplication) << "Generator call failed: " << qPrintable(genCall);
		return false;
	}

	// ToDo: Generate the solutions using the random selected solution program
	// Call bn_sol 02 10 `< args` < input > output_ex 2> error_ex

	return true;
}

bool TestCaseGenerator::callFileGenerator()
{
	// Call bn_gen 02 10 input output_ex error_ex args
	QStringList arguments;
	arguments << number << total << input << output_ex << error_ex << args;
	QProcess* process = new QProcess(this);
	process->start( executablePath, arguments );

	return true;
}
