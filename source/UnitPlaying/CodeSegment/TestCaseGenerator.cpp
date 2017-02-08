#include "TestCaseGenerator.h"
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
		// Prepare arguments
		QStringList arguments;
		arguments << QString("%1").arg(currentTestCaseIndex + 1, 2, 10, QLatin1Char('0'));
		arguments << QString("%1").arg(lastIndex, 2, 10, QLatin1Char('0'));

		const QString& args      = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "args");
		const QString& input     = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "input");
		const QString& output_ex = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "output_ex");
		const QString& error_ex  = playerSolution->buildTestCaseFilepath(currentTestCaseIndex + 1, "error_ex");

		if ( programText->type == ProgramText::standardGenerator )
		{
			// Call bash -c bn_gen 02 10 > input.txt 2> args.txt
			const QString& genCall = QString("\"%1\" \"%2\" > \"%3\" 2> \"%4\"")
				.arg( executablePath )
				.arg( arguments.join("\" \"") )
				.arg( input )
				.arg( args );

			// qCCritical(logTemporary) << "bash " << qPrintable( (QStringList() << "-c" << genCall).join(' ') );
			// process->start( "bash", QStringList() << "-c" << genCall );
			system( qPrintable(genCall) );

			// ToDo: Generate the solutions using the random selected solution program
			// Call bn_sol 02 10 `< args` < input > output_ex 2> error_ex
		}
		else if ( programText->type == ProgramText::fileGenerator )
		{
			// Call bn_gen 02 10 input output_ex error_ex args
			arguments << input << output_ex << error_ex << args;
			QProcess* process = new QProcess(this);
			process->start( executablePath, arguments );
		}
		else
		{
			Q_ASSERT(false);
		}
	}

	emit generationFinished();
	return true;
}
