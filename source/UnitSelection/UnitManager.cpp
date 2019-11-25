#include "UnitManager.h"
#include <QDir>

UnitManager::UnitManager()
{
}

bool UnitManager::load(const QString& context)
{
	// Copy the new context and remove any units from previous loads, if any
	this->context = context;
	levels.clear();

	// Traverse the context resource, it contains levels
	// Levels (1-3, 2-2) are grouped in worlds. Major number is the world e.g: 1-4
	QDir dirContext(":/" + context); // :/training/
	dirContext.setSorting(QDir::NoSort);
	QStringList contextDirList = dirContext.entryList();
	for ( int major = 0; major < contextDirList.size(); ++major )
	{
		// The level contains units, traverse them and create an entry for each unit found
		UnitLevel unitLevel( contextDirList[major] ); // :/training/intro/
		QDir dirLevel( ":/" + context + '/' + contextDirList[major] );
		dirLevel.setSorting(QDir::Name);
		QFileInfoList unitList = dirLevel.entryInfoList();
		for ( int minor = 0; minor < unitList.size(); ++minor )
			unitLevel.units.append( unitList[minor].absoluteFilePath() ); // :/training/intro/sum_xy.botnu
		this->levels.append(unitLevel);
	}
	return true;
}

int UnitManager::calculateMaxUnitsPerLevel() const
{
	int max = -1;
	for ( int index = 0; index < levels.size(); ++index )
		if ( levels[index].units.size() > max )
			max = levels[index].units.size();

	return max;
}
