#ifndef PLAYERSOLUTION_H
#define PLAYERSOLUTION_H

#include <QObject>
#include <QFileInfo>

class Player;
class Unit;

/**
@brief Manages the list of source files that compound the player's solution to an unit

	This class that manages the list of source files that the player creates to solve the problem
	stated by a specific unit. This class create the solution directory, create source files on it,
	retrieve the list of files if they exists, compiles the solution and interprets it.
 */
class PlayerSolution : public QObject
{
	Q_OBJECT

  protected:
	/// The player who created the files that compound this solution
	Player* player;
	/// The unit that the player is trying to solve
	Unit* unit;
	/// The list of source file full paths that compound the player's solution to the current unit
	/// These files are being edited, but just one at time is shown. A drop down (combo box) control
	/// allows player to choose the source to edit
	QFileInfoList sourceFiles;

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
	inline const QFileInfoList& getSourcePaths() const { return sourceFiles; }
	/// Get a list of source files names, without full path
	const QStringList getSourceNames() const;
	/// Returns true if there is files in the solution
	inline bool hasFiles() const { return sourceFiles.size() > 0; }
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
	/// @see getPlayerUnitSourcePath(Player*, Unit*)
	inline QString getPlayerUnitSourcePath(const QString& basename) const { return getPlayerUnitSourcePath(player, unit, basename); }

  protected:
	/// Loads the list of existing files in the unit solution directory for this player
	/// @remarks Assume the @a player and @a unit class members have been set
	/// @return The number of files found as player solution for this unit, -1 on error
	int loadFileList();
};

#endif // PLAYERSOLUTION_H
