#include "UnitManager.h"
#include <QDir>

UnitManager::UnitManager()
{
}

bool UnitManager::load(const QString& context)
{
	this->context = context;

	QDir dirContext(":/" + context); // :/training/
	QFileInfoList contextDirList = dirContext.entryInfoList();
	for ( int i = 0; i < contextDirList.size(); ++i )
	{
		UnitLevel unitLevel( contextDirList[i].fileName() ); // :/training/intro/
		QDir dirLevel( ":/" + context + '/' + contextDirList[i].fileName() );
		QFileInfoList unitList = dirLevel.entryInfoList();
		for ( int j = 0; j < unitList.size(); ++j )
			unitLevel.units.append( unitList[j].fileName() ); // :/training/intro/sum_xy.botnu
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
