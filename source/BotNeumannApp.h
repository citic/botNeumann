#ifndef BOTNEUMANNAPP_H
#define BOTNEUMANNAPP_H

#include <QApplication>

class Player;
class PlayerManager;
class QFont;

/** Object that represents the bot Neumann application as a whole */
class BotNeumannApp : public QApplication
{
	Q_OBJECT
	Q_DISABLE_COPY(BotNeumannApp)

  protected:
	/// Manages the list of players of this game
	PlayerManager* playerManager = nullptr;

  protected:
	/// Family name of the font used for buttons
	static QString robotFontName;
	/// Family name of the mono-spaced font for text editor
	static QString monospacedFontName;

  public:
	/// Constructor
	explicit BotNeumannApp(int& argc, char *argv[]);
	/// Get a pointer to the unique instance of this class
	static inline BotNeumannApp* getInstance() { return static_cast<BotNeumannApp*>(qApp); }
	/// Get the default game font
	static QFont getRobotFont();
	/// Family name of the font used for buttons
	static inline const QString& getRobotFontName() { return robotFontName; }
	/// Get the default monospaced font to be used in several editors in the game
	static QFont getMonospacedFont();
	/// Family name of the mono-spaced font for text editor
	static inline const QString& getMonospacedFontName() { return monospacedFontName; }
	/// Get access to the application's player manager
	inline PlayerManager* getPlayerManager() const { return playerManager; }
	/// Returns a pointer to the current active player using the application
	Player* getCurrentPlayer() const;

  protected:
	/// Adds the font from the given resource name to the font database
	/// @return The family name of the loaded font, empty on error
	QString loadFont(const QString& fileName);
};

#endif // BOTNEUMANNAPP_H
