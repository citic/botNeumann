#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include <QObject>

class DebuggerCall;
class QFileInfo;
class UnitPlayingScene;

/**
	@brief Controller class in charge of controlling the visualization process.

	The visualizator mediates between the model (DebuggerCall) and the views (UnitPlayingScene and
	its helping classes).
*/
class Visualizator : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Visualizator)

  protected:
	/// The model is a debugger session with the executable of the player
	DebuggerCall* debuggerCall;
	/// The view is the unit playing scene and its components
	UnitPlayingScene* unitPlayingScene;

  public:
	/// Constructor
	/// @param executablePath The absolute path to the player solution's executable file to be run
	/// @param unitPlayingScene is the view, and it is set as the parent QObject for this object
	explicit Visualizator(const QFileInfo& executablePath, UnitPlayingScene* unitPlayingScene);
	/// Destructor
	~Visualizator();
	/// Start the visualization process
	void start();
};

#endif // VISUALIZATOR_H
