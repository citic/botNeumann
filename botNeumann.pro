QT += core gui svg widgets
TARGET = botNeumann
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += \
	source \
	source/Common \
	source/Dialogs \
	source/Players \
	source/Playing \
	source/Selection

SOURCES += \
	source/main.cpp \
	source/MainWindow.cpp \
	source/BotNeumannApp.cpp \
	source/Common/BaseScene.cpp

HEADERS += \
	source/MainWindow.h \
	source/BotNeumannApp.h \
	source/Common/BaseScene.h

RESOURCES += \
	assets/assets.qrc
