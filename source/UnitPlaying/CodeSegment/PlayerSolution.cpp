#include "BotNeumannApp.h"
#include "Compiler.h"
#include "CtagsCall.h"
#include "LogManager.h"
#include "Player.h"
#include "PlayerSolution.h"
#include "TestCaseGenerator.h"
#include "Unit.h"
#include "Util.h"

#include <QDateTime>
#include <QDir>
#include <QTextStream>

const int totalBuildSteps = 2;


// Construction -----------------------------------------------------------------------------------

PlayerSolution::PlayerSolution(QObject *parent)
	: QObject(parent)
{
}

PlayerSolution::~PlayerSolution()
{
}


// Player solution files --------------------------------------------------------------------------

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

		// Generate some source code files into player-solution directory
		generateInitialFiles();
	}
	else
		checkInitialFiles();

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
		{
			if ( fileInfo.fileName().startsWith("bn_player") )
				hiddenSourceFiles.append(fileInfo);
			// else: ignore test cases generators
		}
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

bool PlayerSolution::generateInitialFiles()
{
	// Create an injection C code file
	const QString& path = createBotNeumannSourceFile();
	if ( path.isEmpty() )
		return false;

	// Add the injection file to the list of files to be compiled, but not edited by player
	hiddenSourceFiles.append( QFileInfo(path) );

	// Copy convenience header files used by generators
	ResourceToFileDumper dumper;
	if ( dumper.dumpTextResource(":/source_code/bn_c_generator.h", getPlayerUnitSourcePath("bn_c_generator.h")) == false )
		return false;
	if ( dumper.dumpTextResource(":/source_code/bn_cpp_generator.h", getPlayerUnitSourcePath("bn_cpp_generator.h")) == false )
		return false;

	return true;
}

bool PlayerSolution::checkInitialFiles()
{
	if ( ! QFile::exists( getPlayerUnitSourcePath("bn_c_generator.h") ) && ! generateInitialFiles() )
		return false;
	if ( ! QFile::exists( getPlayerUnitSourcePath("bn_cpp_generator.h") ) && ! generateInitialFiles() )
		return false;
	if ( ! QFile::exists( getPlayerUnitSourcePath("bn_player_solution.c") ) && createBotNeumannSourceFile() == "" )
		return false;

	return true;
}

QString PlayerSolution::createBotNeumannSourceFile()
{
	ResourceToFileDumper dumper;

	// Replace paths to the files that will be used to redirect standard input, output and error
	// ToDo: Test case files have numbers
	dumper.addSearchAndReplace( "\".\"", '"' + getPlayerUnitPath() + '"' );

	// Copy the resource to the player solution directory replacing the strings
	const QString& outputFilename = getPlayerUnitSourcePath("bn_player_solution.c");
	if ( dumper.dumpTextResource(":/source_code/bn_player_solution.c", outputFilename) == false )
		return "";

	return outputFilename;
}


// Compiling player solution code -----------------------------------------------------------------

bool PlayerSolution::buildAll()
{
	this->builtSteps = 0;
	return buildPlayerSolution() && extractSymbols() && generateTestCases();
}

bool PlayerSolution::buildPlayerSolution()
{
	// This method must be only called if there is a player solution with files
	Q_ASSERT( this->hasFiles() );

	// ToDo: If there is an active compiling process, stop it
	//if ( compiler && compiler->isRunning() ) compiler->stop();
	if ( playerSolutionProgram )
		playerSolutionProgram->deleteLater();

	// Create an object in charge of compiling the solution files
	playerSolutionProgram = new CompiledProgram(this);
	playerSolutionProgram->optimizeForDebug(true);

	// Compile and run the player solution. It requires some time and we do not wait until it
	// finishes. When the compilation and linking process has finished, our compilerFinished()
	// slot is called and we continue processing the results there
	connect( playerSolutionProgram, SIGNAL(buildFinished()), this, SLOT(playerSolutionBuiltFinished()) );

	// Start the compiling process with the files in the solution and the expected executable file
	playerSolutionProgram->build( getAllSourceFiles(), getExecutablePath() );
	return true;
}

bool PlayerSolution::playerSolutionBuiltFinished()
{
	emit playerSolutionBuilt(playerSolutionProgram);

	if ( builtSteps >= 0 && ++builtSteps == totalBuildSteps )
		emit allBuilt();

	return true;
}


// Symbol extraction ------------------------------------------------------------------------------

bool PlayerSolution::extractSymbols()
{
	// If there is another extractor, remove it
	if ( ctagsCall )
		ctagsCall->deleteLater();

	// Create an object that does the process. When it finishes, alert us
	ctagsCall = new CtagsCall(this);
	connect( ctagsCall, &CtagsCall::extractionFinished, this, &PlayerSolution::ctagsFinished );
	connect( ctagsCall, &CtagsCall::extractionFailed, this, &PlayerSolution::ctagsFailed );

	// Start the symbol extraction process
	return ctagsCall->extractSymbols( getEditableSourceFiles(), getPlayerUnitPath() );
}

bool PlayerSolution::ctagsFinished()
{
	// Alert other objects that ctags finished
	Q_ASSERT(ctagsCall);
	emit symbolsExtracted();

	// For debugging purposes
	qCDebug(logApplication, "Ctags extracted %d global variables and %i funcion definitions"
		, ctagsCall->getGlobalVariables().count(), ctagsCall->getFunctionDefinitions().count() );
	return true;
}

void PlayerSolution::ctagsFailed(QProcess::ProcessError error)
{
	qCCritical(logApplication) << "Ctags call failed with code" << error;

	// We delete the object indicating the symbol extraction failed
	Q_ASSERT(ctagsCall);
	ctagsCall->deleteLater();
	ctagsCall = nullptr;
}


// Test case generation --------------------------------------------------------------------------

int PlayerSolution::generateTestCases()
{
	// Copy the literal test cases provided in botnu unit to files in player solution
	// This method updates the testCasesCount class member
	int testCases = generateUnitTestCases();
	qCInfo(logApplication) << "Unit test cases generated:" << testCases;

	// If there are at least one test case generator in unit, use it to generate more test cases
	// The testCasesCount class member is updated with each extra test case
	const ProgramText* randomGenerator = unit->getARandomGenerator();
	if ( randomGenerator )
		generateExtraTestCases(randomGenerator);

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

		if ( ! dumpTestCase("input", testCase.input) ) return testCasesCount;
		if ( ! dumpTestCase("output_ex", testCase.output) ) return testCasesCount;
		if ( ! dumpTestCase("error_ex", testCase.error) ) return testCasesCount;
		if ( ! dumpTestCaseArgs(testCase.args) ) return testCasesCount;
	}

	return testCasesCount;
}

bool PlayerSolution::dumpTestCase(const QString& caseType, const QString& data)
{
	ResourceToFileDumper dumper;
	const QString& filepath = buildTestCaseFilepath(testCasesCount + 1, caseType);
	return dumper.dumpString( data, filepath );
}

bool PlayerSolution::dumpTestCaseArgs(const QString& args)
{
	ResourceToFileDumper dumper;
	const QString& filepath = buildTestCaseFilepath(testCasesCount + 1, "args");
	return dumper.dumpStringList( args.split(QRegExp("\\s+"), QString::SkipEmptyParts), filepath );
}

QString PlayerSolution::buildTestCaseFilepath(int number, const QString& type) const
{
	return getPlayerUnitSourcePath( QString("bn_%1_%2.txt").arg(number, 2, 10, QLatin1Char('0')).arg(type) );
}

bool PlayerSolution::generateExtraTestCases(const ProgramText* generator)
{
	Q_ASSERT(generator);

	// This object will compile the generator's source code randomly selected from Unit
	if ( testCaseGeneratorProgram )
		testCaseGeneratorProgram->deleteLater();
	testCaseGeneratorProgram = new TestCaseGenerator(this, unit, this);

	// The generator will generate test cases files, when it finishes we need to know the number
	// of test cases that were actually generated
	connect(testCaseGeneratorProgram, SIGNAL(generationFinished()), this, SLOT(testCaseGeneratorFinished()));
	testCaseGeneratorProgram->generate(generator, testCasesCount);

	return true;
}

bool PlayerSolution::testCaseGeneratorFinished()
{
	Q_ASSERT(testCaseGeneratorProgram);
	int previousTestCasesCount = testCasesCount;
	testCasesCount = testCaseGeneratorProgram->getLastGeneratedTestCaseIndex();

	qCInfo(logApplication) << testCasesCount - previousTestCasesCount << "extra test cases generated, total:" << testCasesCount;
	emit testCasesGenerated(testCasesCount);

	if ( builtSteps >= 0 && ++builtSteps == totalBuildSteps )
		emit allBuilt();

	return true;
}
