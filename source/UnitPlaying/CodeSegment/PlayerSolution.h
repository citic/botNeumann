#ifndef PLAYERSOLUTION_H
#define PLAYERSOLUTION_H

#include <QFileInfo>
#include <QObject>

class Player;
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
	Player* player;
	/// The unit that the player is trying to solve
	Unit* unit;
	/// The list of source file full paths that compound the player's solution to the current unit
	/// These files are being edited, but just one at time is shown. A drop down (combo box) control
	/// allows player to choose the source to edit
	QFileInfoList editableSourceFiles;
	/// Some source files that are injected by botNeumann in player's solution. These files support
	/// the data extraction during the visualization process. Player is not allowed to edit them.
	/// These files are compiled as normal files in the build process.
	QFileInfoList hiddenSourceFiles;

  public:
	/// Constructor
	explicit PlayerSolution(QObject *parent = nullptr);
	/// Destructor
	~PlayerSolution();
	/// Load code solution for the given unit. Assumes the current active player
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadSolutionForUnit(Unit* unit);
	/// Load solution created for the given player for the given unit
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadPlayerSolutionForUnit(Player* player, Unit* unit);
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
	/// Get the file path of the standard input file that will be used each time the player's
	/// solution is executed. This file may be overwritten each time the visualization starts
	/// @remarks Files beginning with "bn_" must be ignored in CodeSegment
	inline QString getStandardInputFilename() const { return getPlayerUnitPath() + "/bn_input.txt"; }
	/// Get the file path of the standard output file that will be used to capture the player's
	/// solution output. This file will be overwritten each time the visualization starts
	/// @remarks Files beginning with "bn_" must be ignored in CodeSegment
	inline QString getStandardOutputFilename() const { return getPlayerUnitPath() + "/bn_output.txt"; }
	/// Get the file path of the standard error file that will be used to capture the player's
	/// solution standard error. This file will be overwritten each time the visualization starts
	/// @remarks Files beginning with "bn_" must be ignored in CodeSegment
	inline QString getStandardErrorFilename() const { return getPlayerUnitPath() + "/bn_error.txt"; }

  protected:
	/// Loads the list of existing files in the unit solution directory for this player
	/// @remarks Assume the @a player and @a unit class members have been set
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadFileList();
	/// Creates a directory for a unit that this player is trying to solve
	/// If unitName is omitted and there is a ponter to the unit, its name will be used
	bool createDirectoryForUnit();
	/// Creates a source code file within the player solution's directory. The file contains
	/// C code that will be evaluated later with help of GDB when the visualization is running.
	/// The name of the file will begin with "bn_". Files with this prefix must be ignored by
	/// CodeSegment, in order to avoid players editing that code.
	/// @remarks If the player's solution directory does not exist, it will be created. If there
	/// already exists a botNeumann source code file, it will overwritten
	/// @return The path to the new created source code file in player's solution directory
	QString createBotNeumannSourceFile();
};

#endif // PLAYERSOLUTION_H
