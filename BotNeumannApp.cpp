#include "BotNeumannApp.h"
#include "QFontDatabase"

BotNeumannApp::BotNeumannApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	QFontDatabase::addApplicationFont(":/Fonts/Tenby-Five.otf");
}
