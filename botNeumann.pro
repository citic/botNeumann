QT += core gui svg widgets
TARGET = botNeumann
TEMPLATE = app
CONFIG += c++11

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
	source/Gamee/Actor.cpp \
	source/Gamee/Scene.cpp \
	source/Gamee/Stage.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/main.cpp \
	source/MainWindow.cpp \
	source/Gamee/ScenicElement.cpp

HEADERS += \
	source/BotNeumannApp.h \
	source/Gamee/Actor.h \
	source/Gamee/Scene.h \
	source/Gamee/Stage.h \
	source/GameMenu/GameMenuScene.h \
	source/MainWindow.h \
	source/Gamee/ScenicElement.h

RESOURCES += \
	assets/assets.qrc
