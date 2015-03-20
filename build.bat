@echo off
rem The executables of the following programs must be available in %PATH%
rem   Qt
rem   MinGW / Visual Studio 2013 (not supported yet)
rem   BitRock InstallBuilder
rem   Inkscape
rem   Doxygen

:png
rem inkscape -f assets/game_logo.svg -C -e assets/game_logo-32x32.png

:build
mkdir build
qmake -makefile -o build/Makefile botNeumann.pro
cd build
mingw32-make release
cd ..

rem Copy runtime libraries. See http://doc.qt.io/qt-5/windows-deployment.html
mkdir installer\windows
copy /y build\release\botNeumann.exe installer\windows
windeployqt -release installer\windows\botNeumann.exe

:doc
doxygen doc/api.doxygen

:installer
builder-cli build installer/botNeumann.xml --license installer/license.xml --setvars project.outputDirectory=installer

goto end


:clean
rem rm -rf assets/*.png doc/api build/

:end
