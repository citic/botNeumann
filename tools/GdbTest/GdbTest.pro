QT += core
QT -= gui

TARGET = GdbTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    UserProgram.cpp \
    ToolCall.cpp \
    GdbTest.cpp \
    GdbToken.cpp \
    GdbItemTree.cpp \
    GdbResponse.cpp \
    GdbCall.cpp

HEADERS += \
    UserProgram.h \
    ToolCall.h \
    GdbTest.h \
    GdbToken.h \
    GdbItemTree.h \
    GdbResponse.h \
    GdbCall.h

