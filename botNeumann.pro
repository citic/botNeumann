QT += core gui svg widgets
TARGET = botNeumann
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += \
	source \
	source/Common \
	source/Dialogs \
	source/GameMenu \
	source/Players \
	source/UnitPlaying \
	source/UnitSelection

SOURCES += \
	source/main.cpp \
	source/MainWindow.cpp \
	source/BotNeumannApp.cpp \
	source/Common/BaseScene.cpp \
	source/GameMenu/GameMenuScene.cpp

HEADERS += \
	source/MainWindow.h \
	source/BotNeumannApp.h \
	source/Common/BaseScene.h \
	source/GameMenu/GameMenuScene.h

RESOURCES += \
	assets/assets.qrc
