#include "BotNeumannApp.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include "Player.h"
#include "PlayerSolution.h"
#include "Unit.h"

#include <QDateTime>
#include <QDir>

PlayerSolution::PlayerSolution(QObject *parent)
	: QObject(parent)
	, player(nullptr)
	, unit(nullptr)
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
	sourceFiles.clear();

	// Load the list of files from the player's source
	int fileCount = loadFileList();

	// If player has never tried this unit, assume there is a "main.cpp"
	if ( fileCount == -1 )
		sourceFiles.append( getLastEditedFilePath() );

	// ToDo: load breakpoints from user configuration

	// Done
	return fileCount;
}

QFileInfo PlayerSolution::getLastEditedFilePath() const
{
	// If there are not files, assume the last edited file is an inexistent "main.cpp"
	if ( sourceFiles.size() <= 0 ) return QFileInfo(getPlayerUnitSourcePath("main.cpp"));

	// If there is only one file, we dont' have to compare modification dates
	if ( sourceFiles.size() == 1 ) return sourceFiles[0];

	// There are files, compare its modification dates, and select the latest modified
	int lastModifiedIndex = 0;
	QDateTime lastModifiedDate = sourceFiles[0].lastModified();
	for (int i = 1; i < sourceFiles.size(); ++i )
		if ( sourceFiles[i].lastModified() > lastModifiedDate )
		{
			lastModifiedIndex = i;
			lastModifiedDate = sourceFiles[i].lastModified();
		}

	// Return the latest modified found file path
	return sourceFiles[lastModifiedIndex];
}

const QStringList PlayerSolution::getSourceNames() const
{
	// Extract only the file names, without their path, from the list of files
	QStringList sourceNames;
	sourceNames.reserve( sourceFiles.size() );
	foreach ( const QFileInfo& filePath, sourceFiles )
		sourceNames.append( filePath.fileName() );
	return sourceNames;
}

int PlayerSolution::findFileIndex(const QFileInfo& filePath) const
{
	for ( int i = 0; i < sourceFiles.size(); ++i )
		if ( filePath == sourceFiles[i] )
			return i;

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
	return player->getLocalDataDirectory().absolutePath() + '/' + unit->getId();
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
	sourceFiles = dir.entryInfoList();

	// Done
	return sourceFiles.size();
}
