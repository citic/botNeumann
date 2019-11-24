#ifndef UNITSELECTIONSCENE_H
#define UNITSELECTIONSCENE_H

#include "GameScene.h"
#include "UnitManager.h"

class LinearLayout;

class UnitSelectionScene : public GameScene
{
	Q_OBJECT
	Q_DISABLE_COPY(UnitSelectionScene)

  protected:
	/// The context where the units are found. It is a folder where units are to be loaded from
	/// "Training" or "Missions"
	QString context;
	/// Loads the unit list for the given context
	UnitManager unitManager;
	/// Used to set enable the first no completed unit during the load of the scene
	bool currentUnitEnabled;

  public:
	/// Constructor
	/// @param context The folder where units are to be loaded from: "training" or "missions"
	explicit UnitSelectionScene(const QString& context, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~UnitSelectionScene() override;

  signals:
	/// Emitted when Back button is pressed
	void showGameMenuScene();
	/// Emitted when a playable unit was pressed
	void showUnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename);

  protected slots:
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;
	/// Called when an unit is selected
	void unitPressed();

  protected:
	/// Load and create a chips for each unit (game level)
	bool createLevelsUnits();
	/// Animate some air tube carriers/pods representing information flow
	bool animatePods();
	/// Create a line of chips for the given level
	void createLevel(int levelIndex, const UnitLevel& level);
	/// Create a chip for the given unit
	void createUnit(int levelIndex, int unitIndex, int unitCountInLevel, const QString& filename, LinearLayout* levelLayout);
};

#endif // UNITSELECTIONSCENE_H
