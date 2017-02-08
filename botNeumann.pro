QT += core gui svg widgets network
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
	source/Dialogs \
	source/Gamee \
	source/Gamee/Layouts \
	source/Gamee/Transitions \
	source/GameMenu \
	source/Players \
	source/ToolCall \
	source/ToolCall/Compiler \
	source/ToolCall/Debugger \
	source/UnitPlaying \
	source/UnitPlaying/Common \
	source/UnitPlaying/CodeSegment \
	source/UnitPlaying/DataSegment \
	source/UnitPlaying/HeapSegment \
	source/UnitPlaying/Messages \
	source/UnitPlaying/Threads \
	source/UnitSelection \

HEADERS += \
	source/BotNeumannApp.h \
	source/BotNeumannDirector.h \
	source/Common/Common.h \
	source/Common/GameScene.h \
	source/Common/InfoDialog.h \
	source/Common/LogManager.h \
	source/Common/PlayerStatus.h \
	source/Common/Util.h \
	source/Gamee/Actor.h \
	source/Gamee/AlignedItem.h \
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
	source/Gamee/SvgRendererManager.h \
	source/Gamee/Transitions/Transition.h \
	source/Gamee/Transitions/TransitionSlide.h \
	source/GameMenu/GameMenuScene.h \
	source/GameMenu/PlayerManagerDialog.h \
	source/MainWindow.h \
	source/ToolCall/Compiler/Compiler.h \
	source/ToolCall/Compiler/CompilerCall.h \
	source/ToolCall/Compiler/CompilerDiagnostic.h \
	source/ToolCall/Compiler/Diagnostic.h \
	source/ToolCall/Compiler/LinkerCall.h \
	source/ToolCall/Compiler/LinkerDiagnostic.h \
	source/ToolCall/Debugger/DebuggerCall.h \
	source/ToolCall/Debugger/GdbCall.h \
	source/ToolCall/Debugger/GdbCommon.h \
	source/ToolCall/Debugger/GdbItemTree.h \
	source/ToolCall/Debugger/GdbResponse.h \
	source/ToolCall/Debugger/GdbToken.h \
	source/ToolCall/ToolCall.h \
	source/UnitPlaying/CodeSegment/CodeEditor.h \
	source/UnitPlaying/CodeSegment/CodeSegment.h \
	source/UnitPlaying/CodeSegment/CompiledProgram.h \
	source/UnitPlaying/CodeSegment/DebuggerBreakpoint.h \
	source/UnitPlaying/CodeSegment/GuiBreakpoint.h \
	source/UnitPlaying/CodeSegment/LineNumberArea.h \
	source/UnitPlaying/CodeSegment/PlayerSolution.h \
	source/UnitPlaying/CodeSegment/SyntaxHighlighter.h \
	source/UnitPlaying/CodeSegment/TestCaseGenerator.h \
	source/UnitPlaying/Common/GdbResponseListener.h \
	source/UnitPlaying/Common/MemoryFrame.h \
	source/UnitPlaying/Common/MemoryRow.h \
	source/UnitPlaying/Common/MemorySegment.h \
	source/UnitPlaying/Common/MemoryTop.h \
	source/UnitPlaying/Common/VisualizationSpeed.h \
	source/UnitPlaying/DataSegment/DataSegment.h \
	source/UnitPlaying/DataSegment/StandardInputOutput.h \
	source/UnitPlaying/HeapSegment/HeapSegment.h \
	source/UnitPlaying/Messages/MessagesArea.h \
	source/UnitPlaying/Threads/CallStack.h \
	source/UnitPlaying/Threads/CpuCore.h \
	source/UnitPlaying/Threads/CpuCores.h \
	source/UnitPlaying/Threads/ExecutionThread.h \
	source/UnitPlaying/Threads/ExecutionThreadActor.h \
	source/UnitPlaying/Unit.h \
	source/UnitPlaying/UnitPlayingScene.h \
	source/UnitPlaying/Visualizator.h \
	source/UnitSelection/UnitManager.h \
	source/UnitSelection/UnitSelectionScene.h \

SOURCES += \
	source/BotNeumannApp.cpp \
	source/BotNeumannDirector.cpp \
	source/Common/Common.cpp \
	source/Common/GameScene.cpp \
	source/Common/InfoDialog.cpp \
	source/Common/LogManager.cpp \
	source/Common/PlayerStatus.cpp \
	source/Common/Util.cpp \
	source/Gamee/Actor.cpp \
	source/Gamee/AlignedItem.cpp \
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
	source/Gamee/SvgRendererManager.cpp \
	source/Gamee/Transitions/Transition.cpp \
	source/Gamee/Transitions/TransitionSlide.cpp \
	source/GameMenu/GameMenuScene.cpp \
	source/GameMenu/PlayerManagerDialog.cpp \
	source/main.cpp \
	source/MainWindow.cpp \
	source/ToolCall/Compiler/Compiler.cpp \
	source/ToolCall/Compiler/CompilerCall.cpp \
	source/ToolCall/Compiler/CompilerDiagnostic.cpp \
	source/ToolCall/Compiler/Diagnostic.cpp \
	source/ToolCall/Compiler/LinkerCall.cpp \
	source/ToolCall/Compiler/LinkerDiagnostic.cpp \
	source/ToolCall/Debugger/DebuggerCall.cpp \
	source/ToolCall/Debugger/GdbCall.cpp \
	source/ToolCall/Debugger/GdbItemTree.cpp \
	source/ToolCall/Debugger/GdbResponse.cpp \
	source/ToolCall/Debugger/GdbToken.cpp \
	source/ToolCall/ToolCall.cpp \
	source/UnitPlaying/CodeSegment/CodeEditor.cpp \
	source/UnitPlaying/CodeSegment/CodeSegment.cpp \
	source/UnitPlaying/CodeSegment/CompiledProgram.cpp \
	source/UnitPlaying/CodeSegment/DebuggerBreakpoint.cpp \
	source/UnitPlaying/CodeSegment/GuiBreakpoint.cpp \
	source/UnitPlaying/CodeSegment/LineNumberArea.cpp \
	source/UnitPlaying/CodeSegment/PlayerSolution.cpp \
	source/UnitPlaying/CodeSegment/SyntaxHighlighter.cpp \
	source/UnitPlaying/CodeSegment/TestCaseGenerator.cpp \
	source/UnitPlaying/Common/GdbResponseListener.cpp \
	source/UnitPlaying/Common/MemoryFrame.cpp \
	source/UnitPlaying/Common/MemoryRow.cpp \
	source/UnitPlaying/Common/MemorySegment.cpp \
	source/UnitPlaying/Common/MemoryTop.cpp \
	source/UnitPlaying/Common/VisualizationSpeed.cpp \
	source/UnitPlaying/DataSegment/DataSegment.cpp \
	source/UnitPlaying/DataSegment/StandardInputOutput.cpp \
	source/UnitPlaying/HeapSegment/HeapSegment.cpp \
	source/UnitPlaying/Messages/MessagesArea.cpp \
	source/UnitPlaying/Threads/CallStack.cpp \
	source/UnitPlaying/Threads/CpuCore.cpp \
	source/UnitPlaying/Threads/CpuCores.cpp \
	source/UnitPlaying/Threads/ExecutionThread.cpp \
	source/UnitPlaying/Threads/ExecutionThreadActor.cpp \
	source/UnitPlaying/Unit.cpp \
	source/UnitPlaying/UnitPlayingScene.cpp \
	source/UnitPlaying/Visualizator.cpp \
	source/UnitSelection/UnitManager.cpp \
	source/UnitSelection/UnitSelectionScene.cpp \

RESOURCES += \
	assets/assets.qrc \
	units/training/training.qrc

FORMS += \
	source/GameMenu/PlayerManagerDialog.ui \
	source/Common/InfoDialog.ui

OTHER_FILES += \
	units/botnu-1.0.dtd \
