#include "BotNeumannApp.h"
#include "LinearLayout.h"
#include "Player.h"
#include "Prop.h"
#include "SvgButton.h"
#include "UnitSelectionScene.h"
#include <QFileInfo>

// Default width percent of the pneumatic tube of the whole unit
const qreal tubeWidthPercent = 0.43;
// Default width percent of the chip. Chip + tube must be 1
const qreal chipWidthPercent = 1.0 - tubeWidthPercent;

UnitSelectionScene::UnitSelectionScene(const QString& context, Stage* stage, QGraphicsItem* parent)
	: GameScene("unit_selection", stage, parent)
	, context(context)
	, currentUnitEnabled(true)
{
	createStandardMenu(context, false);
	createLevelsUnits();
	animatePods();
}

UnitSelectionScene::~UnitSelectionScene()
{
}

void UnitSelectionScene::backButtonPressed()
{
	emit showGameMenuScene();
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

void UnitSelectionScene::createLevel(int levelIndex, const UnitLevel& level)
{
	// Separate from game header or previous level
	this->layout->addStretch(0.05);

	// Create a layout for this level, and add it to the scene global layout
	LinearLayout* levelLayout = new LinearLayout(Qt::Horizontal);

	// Position the layout a little bit at the left, to create the sensation of
	// incomplete air tubes at the beginning and the end
	levelLayout->setMarginLeft(-0.01);

	// Calculate the average height proportional percent of the remaining scene for
	// all the leves. 0.1% is the height of the standard menu
	// If there are a few unit lines, we keep with a maximum conservative height of 15%
	qreal averageHeightPercent = (1.0 - 0.1) / unitManager.getLevels().size();
	this->layout->addLayout(levelLayout, qMin(averageHeightPercent, 0.15));

	// Use the same width for all units
	int maxUnitCountPerLevel = unitManager.calculateMaxUnitsPerLevel();

	// For each unit in this level, create a chip
	for (int i = 0, count = level.units.size(); i < count; ++i )
		createUnit(levelIndex, i, maxUnitCountPerLevel, level.units[i], levelLayout);
}

void UnitSelectionScene::createUnit(int levelIndex, int unitIndex, int unitCountInLevel, const QString& filename, LinearLayout* levelLayout)
{
	// Calculate the average width of each unit (tube + chip) !for this level?
	// A more pneumatic tube is added in case the number of units exceed the screen
	//qreal averageUnitWidth = qMax(1.0 / (unitCountInLevel + tubeWidthPercent), 0.19);
	qreal averageUnitWidth = 1.0 / (unitCountInLevel);

	// The chip and its state depends on the status the current player has reached for it
	const QString& unitId = QFileInfo(filename).baseName();
	bool playerCompletedUnit = BotNeumannApp::getInstance()->getCurrentPlayer()->hasCompletedUnit(unitId);

	// Add a pneumatic tube
	Prop* pneumaticTube = new Prop(":/unit_selection/unit_selection/air_tube_short.svg", this);
	pneumaticTube->setMargins(0.3, 0.0, 0.3);
	levelLayout->addItem(pneumaticTube, tubeWidthPercent * averageUnitWidth);
	if ( ! currentUnitEnabled ) pneumaticTube->setOpacity(defaultDisabledOpacity);

	// Add the chip
	const QString& label = QString("%1-%2").arg(levelIndex + 1).arg(unitIndex + 1);
	QString buttonBackground(playerCompletedUnit ? ":/unit_selection/unit_selection/chip_restored.svg" : ":/unit_selection/unit_selection/chip_damaged.svg");
	SvgButton* button = new SvgButton(buttonBackground, this, label);
	button->setMargins(0.0);
	levelLayout->addItem(button, chipWidthPercent * averageUnitWidth);

	// The button will be enabled if player completed the unit it or it is the first not completed
	button->setEnabled(currentUnitEnabled);
	if ( ! playerCompletedUnit ) currentUnitEnabled = false;

	// Each button represents an unit. When the button is pressed, the respective unit should be
	// loaded, pass the filename by a QObject's dynamic property
	button->setObjectName(label);
	button->setProperty("unitFilename", filename);
	connect(button, SIGNAL(pressed()), this, SLOT(unitPressed()));
}

bool UnitSelectionScene::animatePods()
{
	return true;
}

void UnitSelectionScene::unitPressed()
{
	QObject* unit = sender();
	emit showUnitPlayingScene(context, unit->objectName(), unit->property("unitFilename").toString());
}
