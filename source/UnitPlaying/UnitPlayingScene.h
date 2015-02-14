#ifndef UNITPLAYINGSCENE_H
#define UNITPLAYINGSCENE_H

#include "GameScene.h"
#include "Unit.h"

class CodeEditorDockWidget;

class UnitPlayingScene : public GameScene
{
	Q_OBJECT
	Q_DISABLE_COPY(UnitPlayingScene)

  protected:
	/// One of the following strings: "training", "challenges", "collaboration"
	QString context;
	/// The number of the level and unit separated by a dash, e.g: "2-14"
	QString levelUnit;
	/// he full resource path of the .botnu unit with details of the unit
	QString filename;
	/// Loads the unit from the .botnu xml file
	Unit unit;
	/// This class is tied to the code editor, i.e: to make it visible or not
	CodeEditorDockWidget* codeEditorDockWidget;

  public:
	/// Constructor
	/// @param context One of the following strings: "training", "challenges", "collaboration"
	/// @param levelUnit The number of the level and unit separated by a dash, e.g: "2-14"
	/// @param filename The full resource path of the .botnu unit with details of the unit
	explicit UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~UnitPlayingScene();
	/// Called when a transition starts, indicating this scene is leaving the stage
	virtual void startLeavingStage() override;
	/// Called when a transition has finished, indicating this scene entered into the stage
	virtual void finishedEnteringStage() override;

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);

  protected slots:
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;
};

#endif // UNITPLAYINGSCENE_H
