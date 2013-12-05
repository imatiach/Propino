# Propino Repo

Hi! Kwabena here,

Welcome to the Offical Propino Repo!

See ide/qt-creator-src/src/plugins/propino for Propino specific code

## How to for Windows

### Install Qt5 MinGW

Download: http://qt-project.org/downloads
  
Make sure to install the compilier in the components menu

### Install BitRock InstallBuilder

Download: http://installbuilder.bitrock.com/download-installbuilder-for-qt-step-2.html
  
Get the Professional Version
    
### Other Things
  
* Install GIT: http://windows.github.com
* Install Perl: http://www.activestate.com/activeperl/downloads
* Install Python: http://www.activestate.com/activepython/downloads
* Install TCL: http://www.activestate.com/activetcl/downloads

### Building
  
1. Open ide/qt-creator-src/qtcreator.pro with QtCreator
2. Set the output directory for both debug and release to ide/qt-creator-build
3. Switch to release mode (lower left hand corner of QtCreator)
4. In the Mode Selector (the side bar) got to Projects->Build Steps and override Make with:
5. C:\Qt\Tools\QtCreator\bin\jom.exe - This will speed up buidling by the number of cores you have.

Other steps... (in the future these steps will not be needed)

1. Copy ide\qt-creator-src\src\plugins\propino\fftw\windows\32\libfftw3-3.dll to ide/qt-creator-build/bin
2. Copy ide\qt-creator-src\src\plugins\propino\ftdi\winodws\i386\ftd2xx.dll to ide/qt-creator-build/bin

## How to for Mac

Propino has not been ported to Mac yet.

## How to for Linux

Propino has not been ported to Linux yet.
