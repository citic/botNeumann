#include "BotNeumannApp.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	BotNeumannApp app(argc, argv);
	MainWindow window;
	window.show();

	return app.exec();
}
