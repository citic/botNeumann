all: png api

png:
	inkscape -f assets/game_logo.svg -C -e assets/game_logo-32x32.png

api:
	doxygen doc/api.doxygen

clean:
	rm -rf assets/*.png doc/api
