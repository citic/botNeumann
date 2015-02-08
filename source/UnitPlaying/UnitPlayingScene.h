#ifndef UNITPLAYINGSCENE_H
#define UNITPLAYINGSCENE_H

#include "GameScene.h"

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

  public:
	/// Constructor
	/// @param context One of the following strings: "training", "challenges", "collaboration"
	/// @param levelUnit The number of the level and unit separated by a dash, e.g: "2-14"
	/// @param filename The full resource path of the .botnu unit with details of the unit
	explicit UnitPlayingScene(const QString& context, const QString& levelUnit, const QString& filename, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~UnitPlayingScene();

  signals:
	/// Emitted when a game mode button is pressed
	void showUnitSelectionScene(const QString& context, bool forward);

  protected slots:
	/// Called when the Back button is pressed
	virtual void backButtonPressed() override;
};

#endif // UNITPLAYINGSCENE_H
