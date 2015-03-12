#include "BotNeumannApp.h"
#include "PlayerManager.h"
#include <QFontDatabase>
#include <QIcon>

// Static member variables
QString BotNeumannApp::robotFontName;
QString BotNeumannApp::monospacedFontName;

BotNeumannApp::BotNeumannApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	// Properties for the whole application
	setApplicationName("botNeumann");
	setOrganizationName("citic-ucr");
	setOrganizationDomain("citic.ucr.ac.cr");
	setApplicationDisplayName("bot Neumann++");
	setApplicationVersion("1.0.0");
  #ifndef Q_OS_LINUX
	setWindowIcon(QIcon(":/game_logo.svg"));
  #endif

	// Set the seed for generating random numbers. ToDo: change it for <random>
	qsrand(time(nullptr));

	// Default font for the game
	robotFontName = loadFont(":/fonts/fonts/tenby_five.otf");
	monospacedFontName = loadFont(":/fonts/fonts/liberation_mono_regular.ttf");

	// Reload the configuration of the last player who was playing with this game
	this->playerManager = new PlayerManager(this);
	this->playerManager->reloadLastPlayer();
}

Player* BotNeumannApp::getCurrentPlayer() const
{
	Q_ASSERT(playerManager);
	return playerManager->getCurrentPlayer();
}

QString BotNeumannApp::loadFont(const QString& fileName)
{
	// Add the font as an application exclusive font
	int id = QFontDatabase::addApplicationFont(fileName);
	Q_ASSERT( id != -1 );

	// Retrieve its name. If the font has several names, retrieves only the first one
	const QStringList& list = QFontDatabase::applicationFontFamilies(id);
	Q_ASSERT(list.size() > 0);
	return list[0];
}
