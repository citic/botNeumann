#include "UnitManager.h"
#include <fstream>

UnitManager::UnitManager()
{
	UnitLevel l1("intro");
	l1.units.append("sum");
	l1.units.append("abs");
	l1.units.append("diff");
	levels.append(l1);

	UnitLevel l2("ahorasi");
	l2.units.append("duro1");
	l2.units.append("duro2");
	l2.units.append("duro3");
	l2.units.append("duro4");
	l2.units.append("duro5");
	l2.units.append("duro6");
	l2.units.append("duro7");
	l2.units.append("duro8");
	levels.append(l2);
}

bool UnitManager::load(const QString& context)
{

	this->context = context;
/*	QString filename = context + ".lst";
	QString fullpath = FileUtils::getInstance()->fullPathForFilename(filename);

	std::ifstream file(fullpath);
	QString line;
	while ( std::getline(file, line) )
		if ( line.length() > 0 )
			loadLine(line);
*/
	return true;
}

bool UnitManager::loadLine(const QString& line)
{
	Q_UNUSED(line);
	// A file extract may be:

	// intro:        <-- level 1
	// sum_xy        <-- unit 1-1
	// abs_diff      <-- unit 1-2
	// average_n     <-- unit 1-3
	//
	// dyn_mem:      <-- level 2
	// median        <-- unit 2-1
/*
	// Levels ends with :
	if ( line[ line.length() - 1] == ':' )
	{
		levels.append(UnitLevel(line.substr(0, line.length() - 1)));
		return true;
	}

	// If reached this point, the line contains an unit, not a level
	// A least a level must be provided in the file
	assert(levels.size() > 0);
	if ( levels.size() == 0 ) return false;

	levels[levels.size() - 1].units.push_back(line);
*/
	return true;
}
