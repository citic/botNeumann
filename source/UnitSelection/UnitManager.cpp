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
	QDir dirContext(":/" + context); // :/training/
	dirContext.setSorting(QDir::NoSort);
	QStringList contextDirList = dirContext.entryList();
	for ( int i = 0; i < contextDirList.size(); ++i )
	{
		// The level contains units, traverse them and create an entry for each unit found
		UnitLevel unitLevel( contextDirList[i] ); // :/training/intro/
		QDir dirLevel( ":/" + context + '/' + contextDirList[i] );
		dirLevel.setSorting(QDir::Name);
		QFileInfoList unitList = dirLevel.entryInfoList();
		for ( int j = 0; j < unitList.size(); ++j )
			unitLevel.units.append( unitList[j].absoluteFilePath() ); // :/training/intro/sum_xy.botnu
		this->levels.append(unitLevel);
	}
	return true;
}

int UnitManager::calculateMaxUnitsPerLevel() const
{
	int max = -1;
	for ( int i = 0; i < levels.size(); ++i )
		if ( levels[i].units.size() > max )
			max = levels[i].units.size();

	return max;
}
