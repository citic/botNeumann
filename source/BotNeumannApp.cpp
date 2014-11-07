#include "BotNeumannApp.h"
#include "QFontDatabase"

BotNeumannApp::BotNeumannApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	// Properties for the whole application
	setApplicationName("botNeumann");
	setOrganizationName("citic-ucr");
	setOrganizationDomain("citic.ucr.ac.cr");
	setApplicationDisplayName("bot Neumann++");
	setApplicationVersion("1.0.0");

	// Default font for the game
	QFontDatabase::addApplicationFont(":/fonts/tenby_five.otf");
}
