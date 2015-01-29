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
	source/Gamee/Layouts \
	source/Gamee/Transitions \
	source/GameMenu \
	source/Players \
	source/UnitPlaying \
	source/UnitSelection \

SOURCES += \
	source/BotNeumannApp.cpp \
	source/BotNeumannDirector.cpp \
	source/Gamee/Actor.cpp \
	source/Gamee/Director.cpp \
	source/Gamee/Layouts/Layout.cpp \
	source/Gamee/Layouts/LayoutItem.cpp \
	source/Gamee/Layouts/LinearLayout.cpp \
	source/Gamee/Layouts/Spacer.cpp \
	source/Gamee/Scene.cpp \
	source/Gamee/ScenicElement.cpp \
	source/Gamee/Stage.cpp \
	source/Gamee/Transitions/Transition.cpp \
	source/Gamee/Transitions/TransitionSlide.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/main.cpp \
	source/MainWindow.cpp \

HEADERS += \
	source/BotNeumannApp.h \
	source/BotNeumannDirector.h \
	source/Gamee/Actor.h \
	source/Gamee/Director.h \
	source/Gamee/Layouts/Layout.h \
	source/Gamee/Layouts/LayoutItem.h \
	source/Gamee/Layouts/LinearLayout.h \
	source/Gamee/Layouts/Spacer.h \
	source/Gamee/Scene.h \
	source/Gamee/ScenicElement.h \
	source/Gamee/Stage.h \
	source/Gamee/Transitions/Transition.h \
	source/Gamee/Transitions/TransitionSlide.h \
	source/GameMenu/GameMenuScene.h \
	source/Global.h\
	source/MainWindow.h \

RESOURCES += \
	assets/assets.qrc
