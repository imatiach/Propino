/***************************************************************************//**
* @file
* Serial Loader
*
* @version @n 1.0
* @date @n 10/8/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/8/2013
*******************************************************************************/

#ifndef SERIALLOADER_H
#define SERIALLOADER_H

#include <QtConcurrent>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "qftdi.h"
#include "serialescape.h"

// File Size Constants

#define MIN_S1_FILE_SIZE 0x1
#define MAX_S1_FILE_SIZE 0x1000000

#define MIN_P1_FILE_SIZE 0x1
#define MAX_P1_FILE_SIZE 0x10000

// S1 Loader Constants

#define MIN_S1_BAUD_RATE 115200
#define MAX_S1_BAUD_RATE 3000000

#define MIN_S1_BLOCK_SIZE 0x80
#define MAX_S1_BLOCK_SIZE 0x10000

class SerialLoader : public QObject
{
    Q_OBJECT

public:

    explicit SerialLoader(QFtdi *ftdi, QProgressDialog *dialog);

    bool loadBinary(const QString &protocolName,
                    const QString &binaryName = QString(),
                    const QString &stage1LoaderBinary = QString(),
                    const QString &stage2LoaderBinary = QString());

private:

    Q_DISABLE_COPY(SerialLoader)

    QFtdi *m_ftdi; QProgressDialog *m_dialog;

    QString loadBinarySwitch(const QString &protocolName,
                             const QString &binaryName = QString(),
                             const QString &stage1LoaderBinary = QString(),
                             const QString &stage2LoaderBinary = QString());

    QString stage1BinaryLoader(const QString &binaryName);
    QString stage2BinaryLoader(const QString &binaryName);

    // Propeller 1 Loader /////////////////////////////////////////////////////

    enum Propeller1Cmd { Shutdown,
                         LoadRun,
                         ProgramShutdown,
                         ProgramRun };

    QString propeller1Load(quint8 targetVersion,
                           Propeller1Cmd targetCommand,
                           const QString &binaryName = QString());

    quint8 m_propeller1LFSR;
    quint8 iteratePropeller1LFSR();

    QByteArray propeller1EncodeLong(qint32 data);
};

#endif // SERIALLOADER_H

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
