#ifndef VISUALIZATOR_H
#define VISUALIZATOR_H

#include <QObject>
#include <QFileInfo>

class GdbCall;
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
	/// Full path to the player's executable to be debugged
	QFileInfo executablePath;
	/// @todo Allow user to write executable parameters
	QString userProgramArguments;
	/// The model is a debugger session with the executable of the player
	/// @todo Using GdbCall interface directly, replace it by DebuggerCall interface
	GdbCall* debuggerCall;
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
	/// @return true on success, false otherwise
	bool start();
};

#endif // VISUALIZATOR_H
