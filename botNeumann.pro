QT += core gui svg widgets

TARGET = botNeumann
TEMPLATE = app

SOURCES += \
	source/main.cpp \
	source/MainWindow.cpp \
	source/BotNeumannApp.cpp

HEADERS += \
	source/MainWindow.h \
	source/BotNeumannApp.h

FORMS += \
	source/MainWindow.ui

RESOURCES += \
	assets/assets.qrc
