#ifndef PLAYERSOLUTION_H
#define PLAYERSOLUTION_H

#include <QFileInfo>
#include <QProcess>

class CompiledProgram;
class Compiler;
class CtagsCall;
class Player;
struct ProgramText;
class TestCaseGenerator;
class Unit;

/**
@brief Manages the list of source files that compound the player's solution for an unit

	This class that manages the list of source files that the player creates to solve the problem
	stated by a specific unit. This class create the solution directory, create source files on it,
	retrieve the list of files if they exists, compiles the solution and interprets it.

	A player solution contains several types of files. Source code files are separated in two groups
	- Source files created by player: They can be edited by user in CodeEditor
	- Source files injected by botNeumann. These files contain code that support the visualization
	  process. For example, to know the number of bytes read from standard input
 */
class PlayerSolution : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PlayerSolution)

  protected:
	/// The player who created the files that compound this solution
	Player* player = nullptr;
	/// The unit that the player is trying to solve
	Unit* unit = nullptr;
	/// The initial code if it is the first time user tries a problem
	const ProgramText* initialCode = nullptr;
	/// The list of source file full paths that compound the player's solution to the current unit
	/// These files are being edited, but just one at time is shown. A drop down (combo box) control
	/// allows player to choose the source to edit
	QFileInfoList editableSourceFiles;
	/// Some source files that are injected by botNeumann in player's solution. These files support
	/// the data extraction during the visualization process. Player is not allowed to edit them.
	/// These files are compiled as normal files in the build process.
	QFileInfoList hiddenSourceFiles;

  protected:
	/// The number of available test cases for this player solution
	/// -1 means that test cases have not been generated
	int testCasesCount = -1;
	/// Counts the number of steps to have the entire player solution built and emit the
	/// @a allBuilt() signal
	int builtSteps = -1;
	/// The number of build steps reached in order to consider the player solution built
	int totalBuildSteps = 0;
	/// The executable from a random selected Unit's generator
	TestCaseGenerator* testCaseGeneratorProgram = nullptr;
	/// Manages the process of building the executable from player solution's source code
	CompiledProgram* playerSolutionProgram = nullptr;
	/// The executable from a random selected Unit's solution
	CompiledProgram* unitSolutionProgram = nullptr;
	/// Object that generates the bn_tags.txt file with symbols extracted from source files
	CtagsCall* ctagsCall = nullptr;

  public:
	/// Constructor
	explicit PlayerSolution(QObject *parent = nullptr);
	/// Destructor
	virtual ~PlayerSolution() override;
	/// Load code solution for the given unit. Assumes the current active player
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadSolutionForUnit(Unit* unit);
	/// Load solution created for the given player for the given unit
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadPlayerSolutionForUnit(Player* player, Unit* unit);
	/// Get access to the unit this player solution is trying to solve
	/// @remarks Valid only after load methos have been called
	inline Unit* getUnit() const { return unit; }
	/// Get the initial code if it is the first time user tries a problem, otherwise nullptr
	inline const ProgramText* getInitialCode() const { return initialCode; }
	/// Get access to the compiled executable from player solution source code
	inline CompiledProgram* getPlayerSolutionProgram() const { return playerSolutionProgram; }
	/// Returns the full file path of the last edited file for the player in this solution
	/// If the player has never tried to solve this unit before (i.e. there are not solution
	/// files), it will assume an inexistent "main.cpp" in the solution folder.
	/// @remarks This method uses the modification dates of files to know which one is the
	/// latest modify. If player modifies files outside botNeumann, it will affect this method
	QFileInfo getLastEditedFilePath() const;
	/// Get access to the list of source files that compound the player's solution
	inline const QFileInfoList& getEditableSourceFiles() const { return editableSourceFiles; }
	/// Get a list of source files names, without full path
	QStringList getEditableSourceNames() const;
	/// Get all the source files that comprise the player's solution, including both: editable and
	/// hidden source files. These are the files that should be compiled
	inline QFileInfoList getAllSourceFiles() const { return editableSourceFiles + hiddenSourceFiles; }
	/// Returns true if there is files in the solution
	inline bool hasFiles() const { return editableSourceFiles.size() > 0; }
	/// Returns the index if the given file is part of the player's solution, -1 othwerwise
	int findFileIndex(const QFileInfo& filePath) const;
	/// Returns the absolute path to the executable file that results of building this solution
	QString getExecutablePath() const;
	/// Get the player's directory to store source files for this unit. E.g: in Mac OS X:
	/// /Users/<user>/Library/Application Support/botNeumann/<player>/<unit>
	static QString getPlayerUnitPath(Player* player, Unit* unit);
	/// @see getPlayerUnitPath(Player*, Unit*)
	inline QString getPlayerUnitPath() const { return getPlayerUnitPath(player, unit); }
	/// Get the full path of a source file in the player's directory for this unit. E.g: in MacOSX:
	/// /Users/<user>/Library/Application Support/botNeumann/<player>/<unit>/main.cpp
	/// @param basename The base name of the attempted C/C++ file, for example, main.cpp. This file
	/// is part of the solution created by the player
	static QString getPlayerUnitSourcePath(Player* player, Unit* unit, const QString& basename);
	/// @see getPlayerUnitSourcePath(Player*, Unit*, const QString&)
	inline QString getPlayerUnitSourcePath(const QString& basename) const { return getPlayerUnitSourcePath(player, unit, basename); }
	/// Get access to the Ctags call object. The object can be used to get the extracted symbols
	/// @return Pointer to the CtagsCall if the symbol extraction process finished successfully,
	/// otherwise it returns nullptr
	inline CtagsCall* getCtagsCall() const { return ctagsCall; }

  public:
	/// Starts to build the executable from the player solution's source files
	/// Emits @a playerSolutionBuilt when building process is finished
	bool buildPlayerSolution();
	/// Extract symbols from all the source files made by the player. Symbols are global variables
	/// and function definitions. They are extracted with ctags command. These symbols are required
	/// later to set breakpoints (function definitions) and GDB variable-objects (global variables)
	/// Emits @a symbolsExtracted() when done
	/// @return true on success, false otherwise
	bool extractSymbols();
	/// Create a set of test case files to test this player solution. Each test case is a set of
	/// four files that are generated in player-solution's directory. Each test case is uniquely
	/// identified by an integer value nn:
	///
	///   /path/player/solution/bn_nn_input.txt      // redirecto to player solution stdin
	///   /path/player/solution/bn_nn_output_ex.txt  // expected player soltuion output
	///   /path/player/solution/bn_nn_error_ex.txt   // expecter standard error output
	///   /path/player/solution/bn_nn_args.txt       // command line arguments for player solution
	///
	/// Test cases are generated from Unit (botnu XML file). All unit's test cases are copied first
	/// literally. Then, if there are generators within the .botnu file, they will be used to
	/// generate extra test cases.
	/// Emits testCasesGenerated() when done
	/// @return The amount of test cases generated, or a negative code on error
	int generateTestCases();
	/// Builds the player solution executable, generate tests cases and extract symbols from
	/// the player's source code. It emits the signal for each respective step. When all the steps
	/// are complete, it emits @a allBuilt.
	bool buildAll();
	/// Utility function to generate the name of a input or output file path
	/// @param type One of "args", "input", "output_ex", "error_ex", "output_ps" or "error_ps".
	QString buildTestCaseFilepath(int number, const QString& type) const;
	/// Get the number of generated test cases for this player solution. Only valid after the
	/// @a testCasesGenerated() signal has been emitted
	inline int getTestCasesCount() const { return testCasesCount; }
	/// Copies the entire player solution's code to the application event log
	/// @param heading Any text to be written before the source code
	void logSolution(const QString& heading);

  signals:
	/// Emitted when building of player solution executable is finished
	/// @param compiledProgram Provides a pointer to the object that controlled the compilation and
	/// linking process. Interested objects can ask to it the list of diagnostics generated.
	void playerSolutionBuilt(CompiledProgram* compiledProgram);
	/// Emitted when the symbols (global variables and function definitions) have been extracted
	void symbolsExtracted();
	/// Emitted when all test cases were generated to test the player's solution
	void testCasesGenerated(int testCasesCount);
	/// Emitted when all built steps were accomplished: player solution's executable is built,
	/// symbols were extracted and test cases were generated
	void allBuilt();

  protected:
	/// Loads the list of existing files in the unit solution directory for this player
	/// @remarks Assume the @a player and @a unit class members have been set
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadFileList();
	/// Creates a directory for a unit that this player is trying to solve
	/// If unitName is omitted and there is a ponter to the unit, its name will be used
	bool createDirectoryForUnit();
	/// Creates or copies some initial files to the player directory. This process is done just
	/// when the player solution directory is created (for first time)
	bool generateInitialFiles();
	/// Checks if initial botNeumann files exist in player solution directory. If they do not
	/// exist, copy them to the folder. This method is called each time player solution is loaded,
	/// because player may delete some files from the folder
	bool checkInitialFiles();
	/// Creates a source code file within the player solution's directory. The file contains
	/// C code that will be evaluated later with help of GDB when the visualization is running.
	/// The name of the file will begin with "bn_". Files with this prefix must be ignored by
	/// CodeSegment, in order to avoid players editing that code.
	/// @remarks If the player's solution directory does not exist, it will be created. If there
	/// already exists a botNeumann source code file, it will overwritten
	/// @return The path to the new created source code file in player's solution directory
	QString createBotNeumannSourceFile();
	/// Copy the literal test cases provided in botnu unit to files in player solution
	/// This method updates the testCasesCount class member
	/// @return The number of test cases generated
	int generateUnitTestCases();
	/// Convenience function to dump a test case to a file
	bool dumpTestCase(const QString& caseType, const QString& data);
	/// Convenience function to dump a test case's arguments to a file
	bool dumpTestCaseArgs(const QString& args);
	/// Generate more test cases using the given test case generator
	/// The testCasesCount class member is updated with each extra test case
	/// @return True on success, false otherwise
	/// @remarks This function starts a compilation process. When the process finishes it will call
	/// back @a generatorCompileFinished. At that time, if no errors are produced, the extra test
	/// cases will be generated
	bool generateExtraTestCases(const ProgramText* generator);

  protected slots:
	/// Called when the @a playerSolutionProgram has finished building the player solution executable
	bool playerSolutionBuiltFinished();
	/// Called when a generator has finished to generate test cases in order to update the index
	/// @return True on success, false otherwise
	bool testCaseGeneratorFinished();
	/// Called when CtagsCall has finished the symbol extraction process successfully
	bool ctagsFinished();
	/// Called when CtagsCall failed
	void ctagsFailed(QProcess::ProcessError error);
};

#endif // PLAYERSOLUTION_H
