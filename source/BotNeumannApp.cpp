#include "BotNeumannApp.h"
#include "Common.h"
#include "LogManager.h"
#include "PlayerManager.h"
#include "SvgRendererManager.h"

#include <ctime>
#include <QFontDatabase>
#include <QIcon>
#include <QSettings>

// Static member variables
QString BotNeumannApp::robotFontName;
QString BotNeumannApp::monospacedFontName;

// Default font point size
#ifdef Q_OS_MAC
const int monospacedFontSize = 11;
#else
const int monospacedFontSize = 10;
#endif

BotNeumannApp::BotNeumannApp(int& argc, char *argv[])
	: QApplication(argc, argv)
{
	// Properties for the whole application
	setApplicationName("botNeumann");
	setOrganizationName("Citic-Ucr");
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

	// If last time settings were asked to be reset, do it before starting
	QSettings settings;
	if ( settings.value(sk("Application/SettingsReset"), false).toBool() )
	{
		settings.clear();
		settings.sync();
		qWarning("Application settings were reset");
	}

	// Save in settings a flag indicating the application started
	// It will be cleared when the application finishes cleany
	lastSessionCrashed = settings.value(sk("Application/ExitClean"), "") == "running";
	if ( lastSessionCrashed )
		qWarning("Application crashed in last session");

	// A new session started, another try
	settings.setValue(sk("Application/ExitClean"), "running");
	settings.sync();

	// Managers use the settings, create them after resetting the settings
	// Enable logging of messages to a file
	this->logManager = new LogManager(this);

	// Reload the configuration of the last player who was playing with this game
	this->playerManager = new PlayerManager(this);
	this->playerManager->reloadLastPlayer();
}

BotNeumannApp::~BotNeumannApp()
{
	// An clean exit. If next time application starts, it is not clean, it crashed
	QSettings settings;
	settings.setValue(sk("Application/ExitClean"), "clean");
	settings.sync();

	// Not needed
	SvgRendererManager::deleteInstance();

	// Release all used fonts
	QFontDatabase::removeAllApplicationFonts();
}

QFont BotNeumannApp::getRobotFont()
{
	QFont font( getRobotFontName() );
	font.setPointSize(monospacedFontSize + 2);
	return font;
}

QFont BotNeumannApp::getMonospacedFont()
{
	// Set the default monospaced font of the operating system
//	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

	QFont font( getMonospacedFontName() );
	font.setPointSize(monospacedFontSize);
	font.setStyleHint(QFont::TypeWriter);
	font.setFixedPitch(true);
	return font;
}

Player* BotNeumannApp::getCurrentPlayer() const
{
	if ( playerManager )
		return playerManager->getCurrentPlayer();

	return nullptr;
}

QString BotNeumannApp::loadFont(const QString& fileName)
{
	// Add the font as an application exclusive font
	int id = QFontDatabase::addApplicationFont(fileName);
	Q_ASSERT( id >= 0 );

	// Retrieve its name. If the font has several names, retrieves only the first one
	const QStringList& list = QFontDatabase::applicationFontFamilies(id);
	Q_ASSERT(list.size() > 0);
	return list[0];
}
