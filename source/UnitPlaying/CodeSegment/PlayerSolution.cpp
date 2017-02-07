#include "BotNeumannApp.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include "LogManager.h"
#include "Player.h"
#include "PlayerSolution.h"
#include "Unit.h"
#include "Util.h"

#include <QDateTime>
#include <QDir>
#include <QProcess>
#include <QTextStream>

PlayerSolution::PlayerSolution(QObject *parent)
	: QObject(parent)
{
}

PlayerSolution::~PlayerSolution()
{
}

int PlayerSolution::loadSolutionForUnit(Unit* unit)
{
	Player* player = BotNeumannApp::getInstance()->getCurrentPlayer();
	return loadPlayerSolutionForUnit(player, unit);
}

int PlayerSolution::loadPlayerSolutionForUnit(Player* player, Unit* unit)
{
	// Store the player and unit for future use
	this->player = player;
	this->unit = unit;

	// Clear files from previous loads or units, if any
	editableSourceFiles.clear();
	hiddenSourceFiles.clear();

	// Load the list of files from the player's source
	int fileCount = loadFileList();

	// If player has never tried this unit, assume there is a "main.cpp"
	if ( fileCount == -1 )
	{
		// Ask an initial code for player solution
		Q_ASSERT(unit);
		initialCode = unit->getARandomInitialCode();

		Q_ASSERT(initialCode);
		QFileInfo initialCodeFilePath(getPlayerUnitSourcePath("main.") + initialCode->language);
		editableSourceFiles.append( initialCodeFilePath );

		// Create a directory for the player, if it does not exist
		createDirectoryForUnit();

		// Create an injection C code file
		const QString& path = createBotNeumannSourceFile();
		if ( path.isEmpty() == false )
			hiddenSourceFiles.append( QFileInfo(path) );
	}

	// ToDo: load breakpoints from user configuration

	// Done
	return fileCount;
}

QFileInfo PlayerSolution::getLastEditedFilePath() const
{
	// If there are not files, assume the last edited file is an inexistent "main.cpp"
	Q_ASSERT( editableSourceFiles.size() > 0 );

	// If there is only one file, we dont' have to compare modification dates
	if ( editableSourceFiles.size() == 1 ) return editableSourceFiles[0];

	// There are files, compare their modification dates, and select the latest modified
	int lastModifiedIndex = 0;
	QDateTime lastModifiedDate = editableSourceFiles[0].lastModified();
	for (int i = 1; i < editableSourceFiles.size(); ++i )
		if ( editableSourceFiles[i].lastModified() > lastModifiedDate )
		{
			lastModifiedIndex = i;
			lastModifiedDate = editableSourceFiles[i].lastModified();
		}

	// Return the latest modified found file path
	return editableSourceFiles[lastModifiedIndex];
}

QStringList PlayerSolution::getEditableSourceNames() const
{
	// Extract only the file names, without their path, from the list of files
	QStringList sourceNames;
	foreach ( const QFileInfo& filePath, editableSourceFiles )
		sourceNames.append( filePath.fileName() );
	return sourceNames;
}

int PlayerSolution::findFileIndex(const QFileInfo& filePath) const
{
	for ( int index = 0; index < editableSourceFiles.count(); ++index )
		if ( filePath == editableSourceFiles[index] )
			return index;

	return -1;
}

QString PlayerSolution::getExecutablePath() const
{
	return getPlayerUnitSourcePath( unit->getId() );
}

QString PlayerSolution::getPlayerUnitPath(Player* player, Unit* unit)
{
	Q_ASSERT(player);
	Q_ASSERT(unit);
	return player->getLocalDataDirectory(true).absolutePath() + '/' + unit->getId();
}

QString PlayerSolution::getPlayerUnitSourcePath(Player* player, Unit* unit, const QString& basename)
{
	Q_ASSERT(player);
	Q_ASSERT(unit);
	return getPlayerUnitPath(player, unit) + '/' + basename;
}

int PlayerSolution::loadFileList()
{
	// Open the directory where the source files for this solution are
	QDir dir( getPlayerUnitPath() );

	// If the folder does not exist, the player has not trid to solve it (in this machine)
	if ( ! dir.exists() ) return -1;

	// Only load C/C++ code
	QStringList filters;
	filters << "*.h" << "*.H" << "*.hpp" << ".hh"
			<< "*.c" << "*.C" << "*.cpp" << "*.CPP" << "*.cxx" << "*.cc" << "*.c++" << "*.cp";
	dir.setNameFilters(filters);

	// There are source files, load its list
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	const QFileInfoList& files = dir.entryInfoList();

	// Source files in player soluton that begin with "bn_" are not editable by user
	foreach ( const QFileInfo& fileInfo, files )
	{
		if ( fileInfo.fileName().startsWith("bn_") )
			hiddenSourceFiles.append(fileInfo);
		else
			editableSourceFiles.append(fileInfo);
	}

	// Done
	return editableSourceFiles.size();
}

bool PlayerSolution::createDirectoryForUnit()
{
	const QString& path = getPlayerUnitPath();
	if ( Util::createDirectory( path ) )
		return true;

	qCCritical(logPlayer()) << "Unable to create directory for unit:" << path;
	return false;
}

QString PlayerSolution::createBotNeumannSourceFile()
{
	// Get the contents of the source file from a resource
	const QString inputResourceName(":/source_code/bn_player_solution.c");
	QFile inputFile(inputResourceName);

	// Open the source resource for reading
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == false )
	{
		// Log an error and return an invalid file path
		qCritical(logApplication) << "Could not open" << inputResourceName;
		return "";
	}

	// Open the target file for overwriting
	const QString& outputFilename = getPlayerUnitPath() + "/bn_player_solution.c";
	QFile outputFile(outputFilename);
	if ( outputFile.open(QIODevice::WriteOnly | QIODevice::Text) == false )
	{
		// Log an error and return an invalid file path
		qCritical(logApplication) << "Could not create" << outputFilename;
		return "";
	}

	// Both, source resource and target file are open successfully. We need deal with text
	QTextStream inputCode(&inputFile);
	QTextStream outputCode(&outputFile);

	// Copy each line of the source to the target file, but some search/replacements may be required
	while ( ! inputCode.atEnd() )
	{
		// Get a line from the source resource
		QString line = inputCode.readLine();

		// Replace paths to the files that will be used to redirect standard input, output and error
		line.replace( "./input.txt", getStandardInputFilename() );
		line.replace( "./output.txt", getStandardOutputFilename() );
		line.replace( "./error.txt", getStandardErrorFilename() );

		// The line is done, write it to the target file
		outputCode << line << '\n';
	}

	// Destructors will close the files
	return outputFilename;
}

int PlayerSolution::extractSymbols()
{
	// ToDo: implement call to ctags
	return 0;
}

int PlayerSolution::generateTestCases()
{
	// Copy the literal test cases provided in botnu unit to files in player solution
	// This method updates the testCasesCount class member
	generateUnitTestCases();

	// If there are at least one test case generator in unit, use it to generate more test cases
	// The testCasesCount class member is updated with each extra test case
	int generatorIndex = 0;
	const ProgramText* randomGenerator = unit->getARandomGenerator( &generatorIndex );
	if ( randomGenerator )
		generateExtraTestCases(randomGenerator, generatorIndex);

	// Done
	return testCasesCount;
}

int PlayerSolution::generateUnitTestCases()
{
	// Get all the literal test cases from the unit
	const Unit::TestCases& testCases = unit->getTestCases();

	// Copy each test case to their respective files
	for ( testCasesCount = 0; testCasesCount < testCases.count(); ++testCasesCount )
	{
		const TestCase& testCase = testCases[testCasesCount];

		if ( ! dumpTestCase("args", testCase.args) ) return testCasesCount;
		if ( ! dumpTestCase("input", testCase.input) ) return testCasesCount;
		if ( ! dumpTestCase("output_ex", testCase.output) ) return testCasesCount;
		if ( ! dumpTestCase("error_ex", testCase.error) ) return testCasesCount;
	}

	return testCasesCount;
}

bool PlayerSolution::dumpTestCase(const QString& caseType, const QString& data)
{
	ResourceToFileDumper dumper;
	const QString& filepath = buildTestCaseFilepath(testCasesCount + 1, caseType);
	return dumper.dumpString( data, filepath );
}

QString PlayerSolution::buildTestCaseFilepath(int number, const QString& type) const
{
	return getPlayerUnitSourcePath( QString("bn_%1_%2.txt").arg(number, 2, 10, QLatin1Char('0')).arg(type) );
}

bool PlayerSolution::generateExtraTestCases(const ProgramText* generator, int generatorIndex)
{
	Q_ASSERT(generator);
	Q_ASSERT(generatorIndex >= 0);

	// Build the filenames: source(bn_gen_01.cpp) executable(bn_gen_01)
	const QString& basename = QString("bn_gen_%1").arg(generatorIndex + 1, 2, 10, QLatin1Char('0'));
	const QString& sourcePath = getPlayerUnitSourcePath(basename + '.' + generator->language);
	testCaseGeneratorExecutablePath = getPlayerUnitSourcePath(basename);

	// Copy the source code of the generator to a file in player's solution
	ResourceToFileDumper dumper;
	if ( dumper.dumpString( generator->code, sourcePath ) == false )
		return false;

	// Copy the pointer to the generator to continue the process later
	this->testCaseGenerator = generator;

	// Compile the generator's source code. It requires some time and we do not wait until it
	// finishes. When the compilation and linking process has finished, our compilerFinished()
	// is called and we continue processing the results there
	compiler = new Compiler(this);
	connect(compiler, SIGNAL(finished()), this, SLOT(generatorCompileFinished()));
	compiler->compile(sourcePath, testCaseGeneratorExecutablePath);

	return true;
}

bool PlayerSolution::generatorCompileFinished()
{
	Q_ASSERT(compiler);

	// We can only generate test cases if generator's source code compiled without errors
	if ( compiler->getErrorCount() > 0 )
	{
		qCCritical(logApplication) << "Generator failed to compile";
		const QList<Diagnostic*>& diagnostics = compiler->getAllDiagnostics();
		for ( int index = 0; index < diagnostics.count(); ++index )
			qCCritical(logApplication) << diagnostics[index]->buildUserText();
		return false;
	}

	// Generator compiled with no errors, run it for each test case
	Q_ASSERT(testCaseGenerator);
	int lastIndex = testCasesCount + testCaseGenerator->defaultRuns;
	for ( ; testCasesCount < lastIndex; ++testCasesCount )
	{
		// Prepare arguments
		QStringList arguments;
		arguments << QString("%1").arg(testCasesCount + 1, 2, 10, QLatin1Char('0'));
		arguments << QString("%1").arg(lastIndex, 2, 10, QLatin1Char('0'));

		const QString& args      = buildTestCaseFilepath(testCasesCount + 1, "args");
		const QString& input     = buildTestCaseFilepath(testCasesCount + 1, "input");
		const QString& output_ex = buildTestCaseFilepath(testCasesCount + 1, "output_ex");
		const QString& error_ex  = buildTestCaseFilepath(testCasesCount + 1, "error_ex");

		if ( testCaseGenerator->type == ProgramText::standardGenerator )
		{
			// Call bash -c bn_gen 02 10 > input.txt 2> args.txt
			const QString& genCall = QString("\"%1\" \"%2\" > \"%3\" 2> \"%4\"")
				.arg( testCaseGeneratorExecutablePath )
				.arg( arguments.join("\" \"") )
				.arg( input )
				.arg( args );

			// qCCritical(logTemporary) << "bash " << qPrintable( (QStringList() << "-c" << genCall).join(' ') );
			// process->start( "bash", QStringList() << "-c" << genCall );
			system( qPrintable(genCall) );

			// ToDo: Generate the solutions using the random selected solution program
			// Call bn_sol 02 10 `< args` < input > output_ex 2> error_ex
		}
		else if ( testCaseGenerator->type == ProgramText::fileGenerator )
		{
			// Call bn_gen 02 10 input output_ex error_ex args
			arguments << input << output_ex << error_ex << args;
			QProcess* process = new QProcess(this);
			process->start(testCaseGeneratorExecutablePath, arguments);
		}
		else
		{
			Q_ASSERT(false);
		}
	}

	compiler->deleteLater();
	compiler = nullptr;
	return true;
}
