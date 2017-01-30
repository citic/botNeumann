all: png api release installer

png:
	inkscape -f assets/game_logo.svg -C -e assets/game_logo-32x32.png

debug: build
	cd build
	make debug
	
release: build
	cd build
	make release

build:
	mkdir -p build
	qmake -makefile -o build/Makefile botNeumann.pro

api:
	doxygen docs/api.doxygen

installer: installer/license.xml
	# Make sure BitRock InstallBuilder executable is reachable through your $PATH
	builder-cli build installer/botNeumann.xml --license installer/license.xml
	
clean:
	rm -rf assets/*.png docs/api build/
