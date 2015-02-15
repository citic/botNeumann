#ifndef BOTNEUMANNAPP_H
#define BOTNEUMANNAPP_H

#include <QApplication>

class Player;
class PlayerManager;

/** Object that represents the bot Neumann application as a whole */
class BotNeumannApp : public QApplication
{
	Q_OBJECT
	Q_DISABLE_COPY(BotNeumannApp)

  protected:
	/// Manages the list of player of this game
	PlayerManager* playerManager;

  public:
	/// Constructor
	explicit BotNeumannApp(int argc, char *argv[]);
	/// Family name of the font used for buttons
	static inline const QString& getRobotFontName() { return robotFontName; }
	/// Family name of the mono-spaced font for text editor
	static inline const QString& getMonospacedFontName() { return monospacedFontName; }
	/// Get access to the application's player manager
	inline PlayerManager* getPlayerManager() const { return playerManager; }
	/// Returns a pointer to the current active player using the application
	Player* getCurrentPlayer() const;

  protected:
	/// Family name of the font used for buttons
	static QString robotFontName;
	/// Family name of the mono-spaced font for text editor
	static QString monospacedFontName;

  protected:
	/// Adds the font from the given resource name to the font database
	/// @return The family name of the loaded font, empty on error
	QString loadFont(const QString& fileName);
};

#endif // BOTNEUMANNAPP_H
