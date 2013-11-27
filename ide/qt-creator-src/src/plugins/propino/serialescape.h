/***************************************************************************//**
* @file
* Serial Escape
*
* @version @n 1.0
* @date @n 10/22/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/22/2013
*******************************************************************************/

#ifndef SERIALESCAPE_H
#define SERIALESCAPE_H

#include <QtCore>
#include <QtWidgets>

#include "serialoscilloscope.h"
#include "serialterminal.h"

#include "propino_shared.h"
#include "qftdi.h"

class SerialEscape : public QObject
{
    Q_OBJECT

public:

    explicit SerialEscape(QFtdi *ftdi, QSettings *settings);
    ~SerialEscape();

    const QMap<unsigned int, SerialOscilloscope*> &getOscilloscopes();
    const QMap<unsigned int, SerialTerminal*> &getTerminals();

    void deleteOscilloscope(SerialOscilloscope *oscilloscope);
    void deleteTerminal(SerialTerminal *terminal);

    bool getEnabled();
    void setEnabled(bool en);

private slots:

    void aboutToClose();
    void lineStatusChanged(DWORD lineStatus);

    void oscilloscopeDestroyed(QObject *obj);
    void terminalDestroyed(QObject *obj);

    void writeBytes(const QByteArray &bytes);
    void readBytes();

signals:

    void error(const QString &message);

private:

    bool enabled;

    enum
    {
        TERMINAL,
        ESCAPE,
        ESCAPE_S,
        ESCAPE_L,
        LENGTH,
        PARAMETERS
    }
    readState;

    SerialFunction readFunction;
    unsigned int readWHandle;
    unsigned int readCHandle;
    SerialDataType readDataType;

    unsigned int readLength;
    QByteArray argumentData;

    void executeCommand();
    void invalidWindowHandleNumber();
    template <class T> T parseArguments();

    QMap<unsigned int, SerialOscilloscope*> oscilloscopes;
    QMap<unsigned int, SerialTerminal*> terminals;

    QSettings *m_settings; QFtdi *m_ftdi;
};

/***************************************************************************//**
* @file
* @par Documentation Available:
* @n http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-03_D2XXDataThroughput.pdf
* @n http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-04_DataLatencyFlow.pdf
*******************************************************************************/

#define TARGET_MAX_TIMEOUT 10UL // maximum timeout in ms
#define TARGET_UPDATE_RATE 50UL // readyRead rate per second

DWORD calcUSBRWTimeouts(DWORD targetBaudRate);
DWORD calcUSBParameters(DWORD targetBuadRate);

#endif // SERIALESCAPE_H

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
