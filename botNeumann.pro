QT += core gui svg widgets
TARGET = botNeumann
TEMPLATE = app
CONFIG += c++11
ICON = assets/game_logo.icns
RC_ICONS = assets/game_logo.ico

INCLUDEPATH += \
	source \
	source/Common \
	source/Dialogs \
	source/Gamee \
	source/GameMenu \
	source/Players \
	source/UnitPlaying \
	source/UnitSelection \

SOURCES += \
	source/BotNeumannApp.cpp \
	source/BotNeumannDirector.cpp \
	source/Gamee/Actor.cpp \
	source/Gamee/Director.cpp \
	source/Gamee/Scene.cpp \
	source/Gamee/ScenicElement.cpp \
	source/Gamee/Stage.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/main.cpp \
	source/MainWindow.cpp \

HEADERS += \
	source/BotNeumannApp.h \
	source/BotNeumannDirector.h \
	source/Gamee/Actor.h \
	source/Gamee/Director.h \
	source/Gamee/Scene.h \
	source/Gamee/ScenicElement.h \
	source/Gamee/Stage.h \
	source/GameMenu/GameMenuScene.h \
	source/Global.h\
	source/MainWindow.h \

RESOURCES += \
	assets/assets.qrc
