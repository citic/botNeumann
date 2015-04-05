# botNeumann build instructions

botNeumann requires the following tools:

* All
	* [Qt](http://www.qt.io/) (>5.4)
	* [LLVM/Clang](http://llvm.org/releases/) (=3.4.2)
	* [Inkscape](https://inkscape.org/)
	* [Doxygen](http://doxygen.org) (optional)
* Windows
	* [BitRock InstallBuilder](http://bitrock.com/)
* Mac OS X
	* [XCode](https://developer.apple.com/xcode/)
* Linux
	* []()



## Microsoft Windows

Download Qt with MinGW and install it where you want in your system.

LLVM/Clang still not tested on MsWin.

To work with the source code, open botNeumann.pro Qt project in Qt-Creator.

[ToDo: instructions to set up the BitRock ]

To compile and generate an installer run a command interpreter. Add the following directories to your %PATH% environment variable:

1. Where `inkscape.exe` is
2. Your Qt bin (where `qmake.exe`)
3. Your MinGW bin (where `mingw32-make` is)
4. Your BitRock Install Builder bin (where `builder-cli` is)

Finally run `build.bat`.


## Unix-like

botNeumann requires a LLVM/Clang compiler 3.4.2. To download and compile it run the Makefile in the `3rdparty` folder. It may last several minutes according to your computer speed.

	$ cd 3rdparty
	$ make


### Mac OS X

In order to edit the source code and run the executable from Qt Creator, it should be configured. The `botNeumann` executable is generated inside a bundle (i.e. `botNeuman.app`) folder. When it is run from Qt Creator, the Clang dynamic library is not found. Steps to configure:

1. Open `botNeuman.pro` in Qt Creator.
2. Projects | Build environment | Details
3. Add a environment variable

	DYLD_LIBRARY_PATH = /full/path/to/botNeumann/3rdparty/llvm/build/Release+Asserts/lib



### Linux

[Not tested yet]


## Android

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.

## iOS

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.
