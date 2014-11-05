#include "BotNeumannApp.h"
#include "QFontDatabase"

BotNeumannApp::BotNeumannApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	QFontDatabase::addApplicationFont(":/fonts/tenby_five.otf");
}
