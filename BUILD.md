# botNeumann build instructions

botNeumann requires the following tools:

* All
	* [Qt](http://www.qt.io/) (>5.4)
	* [Inkscape](https://inkscape.org/)
	* [Doxygen](http://doxygen.org) (optional)
* Windows
	* [BitRock InstallBuilder](http://bitrock.com/)
* Mac OS X
	* [XCode](https://developer.apple.com/xcode/)
* Linux
	* [GCC](https://gcc.gnu.org/) or [Clang](http://clang.llvm.org/) command line tools



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


## Linux

A GCC or Clang compiler/debugger is required, to be installed and accesible in $PATH


## Mac OS X

Install XCode and its Command Line Tools. A compiler/debugger is also required in player's computer.


## Android

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.

## iOS

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.
