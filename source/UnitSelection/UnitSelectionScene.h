#ifndef UNITSELECTIONSCENE_H
#define UNITSELECTIONSCENE_H

#include "GameScene.h"

class UnitSelectionScene : public GameScene
{
	Q_OBJECT
	Q_DISABLE_COPY(UnitSelectionScene)

  protected:
	/// The context where the units are found. It is a folder where units are to be loaded from
	/// "Training" or "Missions"
	QString context;

  public:
	/// Constructor
	/// @param context The folder where units are to be loaded from: "training" or "missions"
	explicit UnitSelectionScene(const QString& context, Stage* stage, QGraphicsItem* parent = nullptr);
	/// Destructor
	virtual ~UnitSelectionScene();
	/// Get the identification number of this scene
	virtual inline SceneId getSceneId() const override { return SceneId::unitSelection; }

  protected slots:
	  /// Called when the Back button is pressed
	  virtual void backButtonPressed() override;
};

#endif // UNITSELECTIONSCENE_H
