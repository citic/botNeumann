QT += core gui svg widgets
TARGET = botNeumann
TEMPLATE = app
CONFIG += c++11

# Application icon. Affects only Mac OS X
ICON = assets/game_logo.icns

# Executable icon on Microsoft Windows
RC_ICONS = assets/game_logo.ico

INCLUDEPATH += \
	source \
	source/Common \
	source/Compiler \
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
	source/Common/InfoDialog.cpp \
	source/Common/PlayerStatus.cpp \
	source/Compiler/ClangData.cpp \
	source/Compiler/Compiler.cpp \
	source/Compiler/Diagnostic.cpp \
	source/Gamee/Actor.cpp \
	source/Gamee/Director.cpp \
	source/Gamee/LabelButton.cpp \
	source/Gamee/Layouts/Layout.cpp \
	source/Gamee/Layouts/LayoutItem.cpp \
	source/Gamee/Layouts/LinearLayout.cpp \
	source/Gamee/Layouts/Spacer.cpp \
	source/Gamee/Player.cpp \
	source/Gamee/PlayerManager.cpp \
	source/Gamee/Prop.cpp \
	source/Gamee/Scene.cpp \
	source/Gamee/ScenicElement.cpp \
	source/Gamee/Stage.cpp \
	source/Gamee/SvgButton.cpp \
	source/Gamee/Transitions/Transition.cpp \
	source/Gamee/Transitions/TransitionSlide.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/GameMenu/PlayerManagerDialog.cpp \
	source/main.cpp \
	source/MainWindow.cpp \
	source/UnitPlaying/CodeEditor.cpp \
	source/UnitPlaying/CodeSegment.cpp \
	source/UnitPlaying/CpuCore.cpp \
	source/UnitPlaying/CpuCores.cpp \
	source/UnitPlaying/DataSegment.cpp \
	source/UnitPlaying/HeapSegment.cpp \
	source/UnitPlaying/MemoryRow.cpp \
	source/UnitPlaying/MemorySegment.cpp \
	source/UnitPlaying/PlayerSolution.cpp \
	source/UnitPlaying/SyntaxHighlighter.cpp \
	source/UnitPlaying/Unit.cpp \
	source/UnitPlaying/UnitPlayingScene.cpp \
	source/UnitSelection/UnitManager.cpp \
	source/UnitSelection/UnitSelectionScene.cpp \

HEADERS += \
	source/BotNeumannApp.h \
	source/BotNeumannDirector.h \
	source/Common/Common.h \
	source/Common/GameScene.h \
	source/Common/InfoDialog.h \
	source/Common/PlayerStatus.h \
	source/Compiler/ClangData.h \
	source/Compiler/Compiler.h \
	source/Compiler/Diagnostic.h \
	source/Gamee/Actor.h \
	source/Gamee/Director.h \
	source/Gamee/LabelButton.h \
	source/Gamee/Layouts/Layout.h \
	source/Gamee/Layouts/LayoutItem.h \
	source/Gamee/Layouts/LinearLayout.h \
	source/Gamee/Layouts/Spacer.h \
	source/Gamee/Player.h \
	source/Gamee/PlayerManager.h \
	source/Gamee/Prop.h \
	source/Gamee/Scene.h \
	source/Gamee/ScenicElement.h \
	source/Gamee/Stage.h \
	source/Gamee/SvgButton.h \
	source/Gamee/Transitions/Transition.h \
	source/Gamee/Transitions/TransitionSlide.h \
	source/GameMenu/GameMenuScene.h \
	source/GameMenu/PlayerManagerDialog.h \
	source/MainWindow.h \
	source/UnitPlaying/CodeEditor.h \
	source/UnitPlaying/CodeSegment.h \
	source/UnitPlaying/CpuCore.h \
	source/UnitPlaying/CpuCores.h \
	source/UnitPlaying/DataSegment.h \
	source/UnitPlaying/HeapSegment.h \
	source/UnitPlaying/MemoryRow.h \
	source/UnitPlaying/MemorySegment.h \
	source/UnitPlaying/PlayerSolution.h \
	source/UnitPlaying/SyntaxHighlighter.h \
	source/UnitPlaying/Unit.h \
	source/UnitPlaying/UnitPlayingScene.h \
	source/UnitSelection/UnitManager.h \
	source/UnitSelection/UnitSelectionScene.h \

RESOURCES += \
	assets/assets.qrc \
	units/training/training.qrc

FORMS += \
	source/GameMenu/PlayerManagerDialog.ui \
	source/Common/InfoDialog.ui

OTHER_FILES += \
	units/botnu-1.0.dtd \

# Third party library: LLVM/Clang
macx {
	# Defines if Clang is integrated in botNeumann executable, or this is a TCP client or TCP server
	DEFINES += CLANG_INTEGRATED
	#DEFINES += CLANG_CLIENT
	#DEFINES += CLANG_SERVER

	LLVM_PATH = $${PWD}/3rdparty/llvm
	INCLUDEPATH += $${LLVM_PATH}/tools/clang/include
	LIBS += -L$${LLVM_PATH}/build/Release+Asserts/lib -lclang
}
