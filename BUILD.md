# botNeumann build instructions

botNeumann requires the following tools:

* All
	* [Qt](http://www.qt.io/) (>5.4) (mandatory)
	* [Universal ctags](https://ctags.io/) (mandatory)
	* [GNU GCC](https://gcc.gnu.org/) or [Clang](http://clang.llvm.org/) (mandatory)
	* [GNU GDB](https://www.gnu.org/software/gdb/) (mandatory)
	* [Inkscape](https://inkscape.org/) (optional)
	* [Doxygen](http://doxygen.org) (optional)
	* [Graphviz](http://www.graphviz.org/) (optional)
* Windows
	* [BitRock InstallBuilder](http://bitrock.com/)
* Mac OS X
	* [XCode](https://developer.apple.com/xcode/)



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

or you may create a `buildpath.bat` in the root directory of `botNeumann`. This file only requires to add the previous directories to the PATH enviroment variable, something like:

	@echo off
	PATH=%PATH%;C:\Qt\Qt5.4.0\5.4\mingw491_32\bin;C:\Qt\Qt5.4.0\Tools\mingw491_32\bin;C:\Program Files (x86)\BitRock InstallBuilder for Qt 9.5.5\bin;C:\Program Files\Inkscape

Finally run `build.bat`.


## Linux

A GCC or Clang compiler/debugger is required, to be installed and accesible in $PATH

Dependencies:

* GNU C/C++ compiler (GCC) for both 32 and 64bits
* GNU Debugger (GDB)
* Universal ctags
* Qt 5

On Debian you may write:

	sudo apt install build-essential gdb libgl1-mesa-dev libc6-dev-i386 gcc-multilib g++-multilib universal-ctags doxygen graphviz


## Mac OS X

Install XCode and its Command Line Tools. A compiler/debugger is also required in player's computer.


## Android

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.

## iOS

Probably not supported by LLVM/Clang compiler. A web server may be eventually implemented.
