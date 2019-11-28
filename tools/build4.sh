#!/bin/bash

CPU=$(nproc --all)
QT=/opt/Qt5.13.1/5.13.1/gcc_64/bin
if [ -d $QT ]; then
    export PATH=$QT:$PATH
else
    QT=/opt/Qt5.12.0/5.12.0/gcc_64/bin
    export $QT:$PATH
fi

if [ "$(basename $(pwd))" = "tools" ]; then
	cd ..
fi

mkdir -p build && cd build

BASE='qmake ../../botNeumann.pro CONFIG+=release'
mkdir -p ag && cd ag && $BASE DEFINES='ABSTRACT=1 GAMIFICATION=1' && make -j$CPU && cd ..
mkdir -p as && cd as && $BASE DEFINES='ABSTRACT=1 GAMIFICATION=0' && make -j$CPU && cd ..
mkdir -p cg && cd cg && $BASE DEFINES='ABSTRACT=0 GAMIFICATION=1' && make -j$CPU && cd ..
mkdir -p cs && cd cs && $BASE DEFINES='ABSTRACT=0 GAMIFICATION=0' && make -j$CPU && cd ..

