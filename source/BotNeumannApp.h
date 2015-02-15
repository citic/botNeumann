#ifndef BOTNEUMANNAPP_H
#define BOTNEUMANNAPP_H

#include <QApplication>

/** Object that represents the bot Neumann application as a whole */
class BotNeumannApp : public QApplication
{
	Q_OBJECT

  public:
	/// Constructor
	explicit BotNeumannApp(int argc, char *argv[]);
	/// Family name of the font used for buttons
	static inline const QString& getRobotFontName() { return robotFontName; }
	/// Family name of the mono-spaced font for text editor
	static inline const QString& getMonospacedFontName() { return monospacedFontName; }

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
