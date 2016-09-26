QT += core
QT -= gui

TARGET = GdbTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    UserProgram.cpp \
    DebuggerCall.cpp \
    ToolCall.cpp \
    GdbTest.cpp \
    GdbOutput.cpp \
    GdbToken.cpp \
    GdbItemTree.cpp

HEADERS += \
    UserProgram.h \
    DebuggerCall.h \
    ToolCall.h \
    GdbTest.h \
    GdbOutput.h \
    GdbToken.h \
    GdbItemTree.h

