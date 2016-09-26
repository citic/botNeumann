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
    GdbToken.cpp \
    GdbItemTree.cpp \
    GdbResponse.cpp

HEADERS += \
    UserProgram.h \
    DebuggerCall.h \
    ToolCall.h \
    GdbTest.h \
    GdbToken.h \
    GdbItemTree.h \
    GdbResponse.h

