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
	connect(this, SIGNAL(buildFinished()), this, SLOT(generatorBuildFinished()));
	build( generator, playerSolution->getPlayerUnitPath() );

	// If the generator used is a standard one, it will require a solution later
	if ( programText->type == ProgramText::standardGenerator )
		return buildASolution();

	return true;
}

bool TestCaseGenerator::generatorBuildFinished()
{
	generatorBuilt = true;

	// If we built a standard generator, we have to wait until the solution has been compiled
	// before generating the test cases
	if ( programText->type == ProgramText::standardGenerator )
	{
		if ( solutionBuilt )
			generateTestCases();
	}
	else
	{
		// This should be a file generator, we can generate test cases now
		generateTestCases();
	}

	return true;
}

bool TestCaseGenerator::solutionBuildFinished()
{
	solutionBuilt = true;

	// We were waiting for the solution built to generate the test cases
	if ( generatorBuilt )
		return generateTestCases();

	return true;
}

bool TestCaseGenerator::buildASolution()
{
	// Get a random solution from Unit
	const ProgramText* randomSolution = unit->getARandomSolution();
	if ( randomSolution == nullptr )
	{
		qCCritical(logApplication) << "Test cases generation failed: no solutions in Unit";
		return false;
	}

	// Compile the random selected solution
	Q_ASSERT(solution == nullptr);
	solution = new CompiledProgram(this);

	// Let's continue after the compilation of the random solution is done
	Q_ASSERT(randomSolution->type == ProgramText::solution);
	connect( solution, SIGNAL(buildFinished()), this, SLOT(solutionBuildFinished()) );
	solution->build( randomSolution, playerSolution->getPlayerUnitPath() );

	return true;
}

bool TestCaseGenerator::generateTestCases()
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
	qCDebug(logTemporary) << qPrintable(genCall);
	if ( system( qPrintable(genCall) ) != 0 )
	{
		qCCritical(logApplication) << "Generator call failed: " << qPrintable(genCall);
		return false;
	}

	// Generate the solutions using the random selected solution program
	// Call bn_sol `< args` < input > output_ex 2> error_ex
	Q_ASSERT(solution);
	const QString& solCall = QString("\"%1\" `< \"%2\"` < \"%3\" > \"%4\" 2> \"%5\"")
		.arg( solution->getExecutablePath() )
		.arg( args )
		.arg( input )
		.arg( output_ex )
		.arg( error_ex );

	// Call the solution to generate the expected output
	qCDebug(logTemporary) << qPrintable(solCall);
	if ( system( qPrintable(solCall) ) != 0 )
	{
		qCCritical(logApplication) << "Solution call failed: " << qPrintable(solCall);
		return false;
	}

	return true;
}

bool TestCaseGenerator::callFileGenerator()
{
	// Call bn_gen 02 10 input output_ex error_ex args
	QStringList arguments;
	arguments << number << total << input << output_ex << error_ex << args;
	QProcess* process = new QProcess(this);
	qCDebug(logTemporary) << executablePath << arguments;
	process->start( executablePath, arguments );

	return true;
}
