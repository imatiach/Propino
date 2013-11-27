################################################################################
# @file
# Propino Plugin
#
# @version @n 1.0
# @date @n 9/15/2013
#
# @author @n Kwabena W. Agyeman
# @copyright @n (c) 2013 Kwabena W. Agyeman
# @n All rights reserved - Please see the end of the file for the terms of use
#
# @par Update History:
# @n v1.0 - Original release - 9/15/2013
################################################################################

PROPINO_VERSION = 0.1
PROPINO_COMPAT_VERSION = 0.1

include(../../qtcreatorplugin.pri)

QT += concurrent printsupport widgets

DEFINES += PROPINO_VERSION=$${PROPINO_VERSION}
DEFINES += PROPINO_COMPAT_VERSION=$${PROPINO_COMPAT_VERSION}

DEFINES -= QT_NO_CAST_FROM_ASCII
DEFINES -= QT_NO_CAST_TO_ASCII
DEFINES -= QT_NO_CAST_FROM_BYTEARRAY
DEFINES -= QT_USE_QSTRINGBUILDER

DEFINES -= QT_USE_FAST_OPERATOR_PLUS
DEFINES -= QT_USE_FAST_CONCATENATION

HEADERS += propinoplugin.h \
           boardsettingsdialog.h \
           programloaderdialog.h \
           binaryeditor.h \
           binaryfinder.h \
           serialescape.h \
           serialloader.h \
           serialoscilloscope.h \
           serialterminal.h \
           utilitempicker.h \
           utilpathpicker.h \
           propinoconstants.h \
           propino_global.h \
           propino_settings.h \
           propino_shared.h

SOURCES += propinoplugin.cpp \
           boardsettingsdialog.cpp \
           programloaderdialog.cpp \
           binaryeditor.cpp \
           binaryfinder.cpp \
           serialescape.cpp \
           serialloader.cpp \
           serialoscilloscope.cpp \
           serialterminal.cpp \
           utilitempicker.cpp \
           utilpathpicker.cpp

FORMS += boardsettingsdialog.ui \
         programloaderdialog.ui \
         binaryeditor.ui \
         binaryfinder.ui \
         serialoscilloscope.ui \
         serialterminal.ui \
         utilitempicker.ui \
         utilpathpicker.ui

RESOURCES += propino.qrc

# Include BINEditor

INCLUDEPATH += $$PWD/../bineditor
DEPENDPATH += $$PWD/../bineditor

HEADERS += $$PWD/../bineditor/bineditor.h
SOURCES += $$PWD/../bineditor/bineditor.cpp

# Include QFtdi

INCLUDEPATH += $$PWD/QFtdi
DEPENDPATH += $$PWD/QFtdi

HEADERS += $$PWD/QFtdi/qftdi.h
SOURCES += $$PWD/QFtdi/qftdi.cpp

# Include QCustomPlot

INCLUDEPATH += $$PWD/QCustomPlot
DEPENDPATH += $$PWD/QCustomPlot

HEADERS += $$PWD/QCustomPlot/qcustomplot.h
SOURCES += $$PWD/QCustomPlot/qcustomplot.cpp

# Include FTDI

win32: LIBS += -L$$PWD/ftdi/windows/i386/ -lftd2xx
win64: LIBS += -L$$PWD/ftdi/windows/amd64/ -lftd2xx64
win32: INCLUDEPATH += $$PWD/ftdi/windows
win64: INCLUDEPATH += $$PWD/ftdi/windows
win32: DEPENDPATH += $$PWD/ftdi/windows
win64: DEPENDPATH += $$PWD/ftdi/windows
win32: HEADERS += $$PWD/ftdi/windows/ftd2xx.h
win64: HEADERS += $$PWD/ftdi/windows/ftd2xx.h
win32: OTHER_FILES += $$PWD/ftdi/windows/i386/ftd2xx.dll
win64: OTHER_FILES += $$PWD/ftdi/windows/amd64/ftd2xx64.dll
win32: OTHER_FILES += $$PWD/ftdi/windows/i386/ftd2xx.lib
win64: OTHER_FILES += $$PWD/ftdi/windows/amd64/ftd2xx.lib

# Include FFTW

win32: LIBS += -L$$PWD/fftw/windows/32/ -llibfftw3-3
win64: LIBS += -L$$PWD/fftw/windows/64/ -llibfftw3-3
win32: INCLUDEPATH += $$PWD/fftw/windows/32
win64: INCLUDEPATH += $$PWD/fftw/windows/64
win32: DEPENDPATH += $$PWD/fftw/windows/32
win64: DEPENDPATH += $$PWD/fftw/windows/64
win32: HEADERS += $$PWD/fftw/windows/32/fftw3.h
win64: HEADERS += $$PWD/fftw/windows/64/fftw3.h
win32: OTHER_FILES += $$PWD/fftw/windows/32/libfftw3-3.dll
win64: OTHER_FILES += $$PWD/fftw/windows/64/libfftw3-3.dll
win32: OTHER_FILES += $$PWD/fftw/windows/32/libfftw3-3.def
win64: OTHER_FILES += $$PWD/fftw/windows/64/libfftw3-3.def

################################################################################
# @file
# @par MIT License - TERMS OF USE:
# @n Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# @n
# @n The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# @n
# @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################
