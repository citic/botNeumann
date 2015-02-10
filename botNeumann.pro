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
	source/Common/GameScene.cpp \
	source/Gamee/Actor.cpp \
	source/Gamee/Director.cpp \
	source/Gamee/Layouts/Layout.cpp \
	source/Gamee/Layouts/LayoutItem.cpp \
	source/Gamee/Layouts/LinearLayout.cpp \
	source/Gamee/Layouts/Spacer.cpp \
	source/Gamee/Scene.cpp \
	source/Gamee/ScenicElement.cpp \
	source/Gamee/Stage.cpp \
	source/Gamee/SvgButton.cpp \
	source/Gamee/Transitions/Transition.cpp \
	source/Gamee/Transitions/TransitionSlide.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/main.cpp \
	source/MainWindow.cpp \
	source/UnitPlaying/Unit.cpp \
	source/UnitPlaying/UnitPlayingScene.cpp \
	source/UnitSelection/UnitManager.cpp \
	source/UnitSelection/UnitSelectionScene.cpp \

HEADERS += \
	source/BotNeumannApp.h \
	source/BotNeumannDirector.h \
	source/Common.h \
	source/Common/GameScene.h \
	source/Gamee/Actor.h \
	source/Gamee/Director.h \
	source/Gamee/Layouts/Layout.h \
	source/Gamee/Layouts/LayoutItem.h \
	source/Gamee/Layouts/LinearLayout.h \
	source/Gamee/Layouts/Spacer.h \
	source/Gamee/Scene.h \
	source/Gamee/ScenicElement.h \
	source/Gamee/Stage.h \
	source/Gamee/SvgButton.h \
	source/Gamee/Transitions/Transition.h \
	source/Gamee/Transitions/TransitionSlide.h \
	source/GameMenu/GameMenuScene.h \
	source/MainWindow.h \
	source/UnitPlaying/Unit.h \
	source/UnitPlaying/UnitPlayingScene.h \
	source/UnitSelection/UnitManager.h \
	source/UnitSelection/UnitSelectionScene.h \

RESOURCES += \
	assets/assets.qrc \
	units/training/training.qrc
