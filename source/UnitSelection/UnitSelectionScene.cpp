#include "LinearLayout.h"
#include "SvgButton.h"
#include "UnitSelectionScene.h"

// Default width percent of the pneumatic tube of the whole unit
const qreal tubeWidthPercent = 0.43;
// Default width percent of the chip. Chip + tube must be 1
const qreal chipWidthPercent = 1.0 - tubeWidthPercent;

UnitSelectionScene::UnitSelectionScene(const QString& context, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_selection", stage, parent)
	, context(context)
{
	createStandardMenu(context);
	createLevelsUnits();
	animatePods();
}

UnitSelectionScene::~UnitSelectionScene()
{
}

void UnitSelectionScene::backButtonPressed()
{
	emit newSceneAsked(SceneId::gameMenu);
}


bool UnitSelectionScene::createLevelsUnits()
{
	// Load levels and units from context list, e.g: "Training.lst" or "Missions.lst"
	if ( ! unitManager.load(context) ) return false;

	// Get the list of levels
	const QList<UnitLevel>& levels = unitManager.getLevels();

	// For each level, create a line of chips
	for ( int i = 0; i < levels.size(); ++i )
		createLevel(i, levels[i]);

	return true;
}

#include <QDebug>

void UnitSelectionScene::createLevel(int levelIndex, const UnitLevel& level)
{
	// Separate from game header or previous level
	this->layout->addStretch(0.05);

	// Create a layout for this level, and add it to the scene global layout
	qDebug() << level.name << ":";
	LinearLayout* levelLayout = new LinearLayout(Qt::Horizontal);

	// Calculate the average height proportional percent of the remaining scene for
	// all the leves. 0.1% is the height of the standard menu
	// If there are a few unit lines, we keep with a maximum conservative height of 15%
	qreal averageHeightPercent = (1.0 - 0.1) / unitManager.getLevels().size();
	this->layout->addLayout(levelLayout, qMin(averageHeightPercent, 0.15));

	// For each unit in this level, create a chip
	for (int i = 0, count = level.units.size(); i < count; ++i )
		createUnit(levelIndex, i, count, level.units[i], levelLayout);
}

void UnitSelectionScene::createUnit(int levelIndex, int unitIndex, int unitCountInLevel, const QString& filename, LinearLayout* levelLayout)
{
	qDebug() << levelIndex + 1 << '-' << unitIndex + 1 << ": " << filename;

	// Calculate the average width of each unit (tube + chip) !for this level?
	// A more pneumatic tube is added in case the number of units exceed the screen
	//qreal averageUnitWidth = qMax(1.0 / (unitCountInLevel + tubeWidthPercent), 0.19);
	qreal averageUnitWidth = 1.0 / (unitCountInLevel);

	// Add a pneumatic tube
	ScenicElement* pneumaticTube = new ScenicElement(":/unit_selection/unit_selection/air_tube_short.svg", this);
	pneumaticTube->setMargins(0.3, 0.0, 0.3);
	levelLayout->addItem(pneumaticTube, tubeWidthPercent * averageUnitWidth);

	// Add the chip
	const QString& label = QString("%1-%2").arg(levelIndex + 1).arg(unitIndex + 1);
	QString buttonBackground(":/unit_selection/unit_selection/chip_damaged.svg");
	SvgButton* button = SvgButton::createLabelButton(label, buttonBackground, this);
	button->setMargins(0.0);
	levelLayout->addItem(button, chipWidthPercent * averageUnitWidth);

	// Create the label and center it inside the button sprite
//	menuItem->setName(filename);
}


bool UnitSelectionScene::animatePods()
{
	return true;
}

void UnitSelectionScene::unitPressed()
{
/*	for (size_t i = 0; i < menuItems.size(); ++i )
		if ( menuItems[i] == sender )
			showUnitPlayingScene(context, menuItems[i]->getName());
			// log("Unit %s was selected", menuItems[i]->getName().c_str() );
*/
}
