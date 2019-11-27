#!/bin/bash

export PATH=/opt/Qt5.12.0/5.12.0/gcc_64/bin:$PATH

if [ "$(basename $(pwd))" = "tools" ]; then
	cd ..
fi

mkdir -p build && cd build

mkdir -p ag && cd ag && qmake ../../botNeumann.pro CONFIG+=release DEFINES='ABSTRACT=1 GAMIFICATION=1' && make -j4 && cd ..
mkdir -p as && cd as && qmake ../../botNeumann.pro CONFIG+=release DEFINES='ABSTRACT=1 GAMIFICATION=0' && make -j4 && cd ..
mkdir -p cg && cd cg && qmake ../../botNeumann.pro CONFIG+=release DEFINES='ABSTRACT=0 GAMIFICATION=1' && make -j4 && cd ..
mkdir -p cs && cd cs && qmake ../../botNeumann.pro CONFIG+=release DEFINES='ABSTRACT=0 GAMIFICATION=0' && make -j4 && cd ..
