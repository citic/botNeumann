#!/bin/bash

CPU=$(nproc --all)
QT=/opt/Qt5.13.1/5.13.1/gcc_64/bin
if [ -d $QT ]; then
    export PATH=$QT:$PATH
else
    QT=/opt/Qt5.12.0/5.12.0/gcc_64/bin
    export PATH=$QT:$PATH
fi

if [ "$(basename $(pwd))" = "tools" ]; then
	cd ..
fi

mkdir -p build && cd build

BASE='qmake ../../botNeumann.pro CONFIG+=release'
mkdir -p ag3 && cd ag3 && $BASE DEFINES='ABSTRACT=1 GAMIFICATION=1' && make -j$CPU && cd ..
mkdir -p as2 && cd as2 && $BASE DEFINES='ABSTRACT=1 GAMIFICATION=0' && make -j$CPU && cd ..
mkdir -p cg1 && cd cg1 && $BASE DEFINES='ABSTRACT=0 GAMIFICATION=1' && make -j$CPU && cd ..
mkdir -p cs4 && cd cs4 && $BASE DEFINES='ABSTRACT=0 GAMIFICATION=0' && make -j$CPU && cd ..
