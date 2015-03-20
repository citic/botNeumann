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
for /f "delims=" %%i in ('where Qt5Core.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where Qt5Gui.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where Qt5Svg.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where Qt5Widgets.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where libgcc_s_dw2-1.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where libwinpthread-1.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where libstdc++-6.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where icuin53.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where icuuc53.dll') do xcopy /d /y %%i release
for /f "delims=" %%i in ('where icudt53.dll') do xcopy /d /y %%i release

rem mingw32-make debug
rem for /f "delims=" %%i in ('where Qt5Cored.dll') do xcopy /d /y %%i release
rem for /f "delims=" %%i in ('where Qt5Guid.dll') do xcopy /d /y %%i release
rem for /f "delims=" %%i in ('where Qt5Svgd.dll') do xcopy /d /y %%i release
rem for /f "delims=" %%i in ('where Qt5Widgetsd.dll') do xcopy /d /y %%i release
cd ..

:doc
doxygen doc/api.doxygen

:installer
builder-cli build installer/botNeumann.xml --license installer/license.xml

goto end


:clean
rem rm -rf assets/*.png doc/api build/

:end
