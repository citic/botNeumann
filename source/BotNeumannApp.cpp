#include "BotNeumannApp.h"
#include <QFontDatabase>
#include <QIcon>

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

	// Default font for the game
	QFontDatabase::addApplicationFont(":/fonts/tenby_five.otf");
}
