@echo off
rem The executables of the following programs must be available in %PATH%
rem   Qt
rem   MinGW / Visual Studio 2013 (not supported yet)
rem   BitRock InstallBuilder
rem   Inkscape
rem   Doxygen

rem If exists a file setting the paths to the previous tools, run it
if exist buildpath.bat call buildpath.bat

:png
echo Exporting vector images to png...
inkscape -w 200 -h 270 -f assets/game_menu/robot.svg -C -e assets/game_menu/robot.png
inkscape -w 32 -h 32 -f assets/game_logo.svg -C -e assets/game_logo.png
rem inkscape -h 64 -f assets/game_title.svg -C -e assets/game_title.png

:makefile
echo Generating Makefile...
if not exist build mkdir build
qmake -makefile -o build/Makefile botNeumann.pro
if %ERRORLEVEL% NEQ 0 goto error

:compile
echo Compiling botNeumann...
cd build
mingw32-make release
if %ERRORLEVEL% NEQ 0 goto error
cd ..

:installer-contents
rem Copy runtime libraries. See http://doc.qt.io/qt-5/windows-deployment.html
if not exist installer\windows mkdir installer\windows
xcopy /q /y build\release\botNeumann.exe installer\windows
windeployqt -release installer\windows\botNeumann.exe
if %ERRORLEVEL% NEQ 0 goto error
del /q installer\windows\qt*.qm

:installer
builder-cli build installer/botNeumann.xml --license installer/license.xml --setvars project.outputDirectory=installer
if %ERRORLEVEL% NEQ 0 goto error

goto success

:doc
doxygen docs/api.doxygen
if %ERRORLEVEL% NEQ 0 goto error
goto success

:clean
del /q /s assets/*.png build/ docs/api installer/windows

:success
echo Build successful
goto end

:error
echo Build failed

:end
