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

#include "serialloader.h"

SerialLoader::SerialLoader(QFtdi *ftdi, QProgressDialog *dialog) :
    QObject(ftdi)
{
    Q_CHECK_PTR(ftdi); Q_CHECK_PTR(dialog); m_ftdi = ftdi; m_dialog = dialog;
}

bool SerialLoader::loadBinary(const QString &protocolName,
                              const QString &binaryName,
                              const QString &stage1LoaderBinary,
                              const QString &stage2LoaderBinary)
{
    m_dialog->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint |
    Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::Dialog);

    m_dialog->setCancelButton(NULL);
    m_dialog->setWindowModality(Qt::ApplicationModal);
    m_dialog->setLabelText(tr("Resetting Device..."));
    m_dialog->setWindowTitle(tr("Binary Loader"));
    m_dialog->setRange(0, 0);

    QEventLoop loop; QFuture<QString> future; QFutureWatcher<QString> watcher;

    connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));

    m_ftdi->blockSignals(true); m_dialog->show();

    future = QtConcurrent::run(this, &SerialLoader::loadBinarySwitch,
    protocolName, binaryName, stage1LoaderBinary, stage2LoaderBinary);

    watcher.setFuture(future); loop.exec();

    m_ftdi->blockSignals(false); bool ok = m_ftdi->isOpen();

    QString result = future.result();

    if(!result.isNull())
    {
        m_dialog->hide();

        QMessageBox::critical(m_dialog->parentWidget(),
        tr("Binary Loader"), result);
    }

    if(!ok)
    {
        emit m_ftdi->aboutToClose();
    }

    return result.isNull();
}

QString SerialLoader::loadBinarySwitch(const QString &protocolName,
                                       const QString &binaryName,
                                       const QString &stage1LoaderBinary,
                                       const QString &stage2LoaderBinary)
{
    QString result;

    if(protocolName == "propeller1-shutdown")
    {
        result = propeller1Load(1, Shutdown);
    }
    else if(protocolName == "propeller1-load-run")
    {
        result = propeller1Load(1, LoadRun, binaryName);
    }
    else if(protocolName == "propeller1-program-shutdown")
    {
        result = propeller1Load(1, ProgramShutdown, binaryName);
    }
    else if(protocolName == "propeller1-program-run")
    {
        result = propeller1Load(1, ProgramRun, binaryName);
    }
    else if(protocolName == "propeller1-s1")
    {
        if((result = propeller1Load(1, LoadRun, stage1LoaderBinary)).isNull())
        {
            result = stage1BinaryLoader(binaryName);
        }
    }
    else if(protocolName == "propeller1-s1-s2")
    {
        if((result = propeller1Load(1, LoadRun, stage1LoaderBinary)).isNull())
        {
            if((result = stage1BinaryLoader(stage2LoaderBinary)).isNull())
            {
                result = stage2BinaryLoader(binaryName);
            }
        }
    }
    else
    {
        return tr("Unknown binary loader protocol");
    }

    if(m_ftdi->isOpen())
    {
        if(result.isNull()) // success
        {
            if(!m_ftdi->setInTask(false, 10))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(10);

            if((!m_ftdi->purge(FT_PURGE_RX))
            || (!m_ftdi->setInTask(true, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }
        }
        else // failure
        {
            if((!m_ftdi->setDtr(true))
            || (!m_ftdi->setRts(true))
            || (!m_ftdi->setInTask(false, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(100);

            if((!m_ftdi->setRts(false)) || (!m_ftdi->setDtr(false)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(100);

            if((!m_ftdi->purge(FT_PURGE_RX | FT_PURGE_TX))
            || (!m_ftdi->setInTask(true, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }
        }

        DWORD time = calcUSBRWTimeouts(FT_BAUD_115200);
        DWORD size = calcUSBParameters(FT_BAUD_115200);

        if((!m_ftdi->setBaudRate(FT_BAUD_115200))
        || (!m_ftdi->setTimeouts(time, time))
        || (!m_ftdi->setUSBParameters(size, size))
        || (!m_ftdi->setLatencyTimer(16)))
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }
    }

    return result;
}

QString SerialLoader::stage1BinaryLoader(const QString &binaryName)
{
    QFile binaryFile(binaryName);

    if(!binaryFile.open(QIODevice::ReadOnly))
    {
        return tr("Unable to open application binary image \"%1\"").
               arg(binaryName);
    }

    qint64 fileSize = QFileInfo(binaryName).size();

    if(fileSize < MIN_S1_FILE_SIZE)
    {
        return tr("File size less than the min size of %Ln byte(s)",
               "", MIN_S1_FILE_SIZE);
    }

    if(MAX_S1_FILE_SIZE < fileSize)
    {
        return tr("File size greater than the max size of %Ln byte(s)",
               "", MAX_S1_FILE_SIZE);
    }

    if((!m_ftdi->setBaudRate(FT_BAUD_115200))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // receive sync phrase ////////////////////////////////////////////////////

    bool ok = false; QByteArray shiftreg;

    for(int i = 0; i < 300; i++)
    {
        char c;

        if(m_ftdi->getChar(&c) && shiftreg.append(c).endsWith("Hello World!"))
        {
            ok = true; break;
        }
    }

    if(!ok)
    {
        return tr("Syncing error");
    }

    // receive settings size //////////////////////////////////////////////////

    union { struct { quint16 a, b; } s; char c[sizeof(s)]; } setSize;

    if((m_ftdi->readBytes(setSize.c, sizeof(setSize.c), 100) != sizeof(
       setSize.c)) || ((setSize.s.a & 0xFFFF) != ((~setSize.s.b) & 0xFFFF)))
    {
        return tr("Syncing error");
    }

    // receive settings data //////////////////////////////////////////////////

    QByteArray setData(setSize.s.a, 0);

    if(m_ftdi->readBytes(&setData, 500) != setSize.s.a)
    {
        return tr("Syncing error");
    }

    #pragma pack(1) // pack tight

    union settingsBlocks
    {
        struct
        {
            struct settingsBlock0
            {
                union
                {
                    struct
                    {
                        quint32 maxBaudRate;
                        quint32 maxBlockSize;

                        quint16 signature;
                    }
                    s;

                    char c[sizeof(s)];
                }
                u;

                quint16 checksum;
            }
            s0;
        }
        s;

        char c[sizeof(s)];
    }
    settings;

    #pragma pack(0) // pack loose

    qMemSet(settings.c, 0x00, sizeof(settings.c));
    qMemCopy(settings.c, setData.constData(),
    qMin(setData.size(), static_cast<int>(sizeof(settings.c))));

    // check settings data ////////////////////////////////////////////////////

    if(settings.s.s0.u.s.signature != 0xAA55)
    {
        return tr("Signature error");
    }

    quint16 checksum = qChecksum(settings.s.s0.u.c, sizeof(settings.s.s0.u.c));

    if(checksum != settings.s.s0.checksum)
    {
        return tr("Checksum error - expected ") +
        "0x" + QString("%1").arg(checksum, 0x04L, 0x10L,
        QLatin1Char('0')).toUpper() + tr(" got ") +
        "0x" + QString("%1").arg(settings.s.s0.checksum, 0x04L, 0x10L,
        QLatin1Char('0')).toUpper() + tr(" instead");
    }

    if(settings.s.s0.u.s.maxBaudRate < MIN_S1_BAUD_RATE)
    {
        return tr("Baud rate less than the min of %Ln bit(s) per second",
               "", MIN_S1_BAUD_RATE);
    }

    if(MAX_S1_BAUD_RATE < settings.s.s0.u.s.maxBaudRate)
    {
        return tr("Baud rate greater than the max of %Ln bit(s) per second",
               "", MAX_S1_BAUD_RATE);
    }

    if(settings.s.s0.u.s.maxBlockSize < MIN_S1_BLOCK_SIZE)
    {
        return tr("Block size less than the min of %Ln byte(s)",
               "", MIN_S1_BLOCK_SIZE);
    }

    if(MAX_S1_BLOCK_SIZE < settings.s.s0.u.s.maxBlockSize)
    {
        return tr("Block size greater than the max of %Ln byte(s)",
               "", MAX_S1_BLOCK_SIZE);
    }

    if(!m_ftdi->setBaudRate(settings.s.s0.u.s.maxBaudRate))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // send binary data blocks ////////////////////////////////////////////////

    m_dialog->setLabelText(tr("Sending Blocks..."));

    QByteArray binaryFileData = binaryFile.readAll(); quint32 address = 0;

    forever
    {
        QByteArray data = binaryFileData.left(settings.s.s0.u.s.maxBlockSize);

        int zeroPadding = (((data.size() + sizeof(quint32) - 1) /
        sizeof(quint32)) * sizeof(quint32)) - data.size();

        while(zeroPadding--)
        {
            data.append(char(0));
        }

        QByteArray block;

        // add block address //////////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;
            u.s = address; address += data.size();
            block.append(u.c, sizeof(u.c));
        }

        // add block data /////////////////////////////////////////////////////
        {
            block.append(data);
        }

        // add block signature ////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = 0xAA55;
            block.append(u.c, sizeof(u.c));
        }

        // add block checksum /////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = qChecksum(block.constData(), block.size());
            block.append(u.c, sizeof(u.c));
        }

        // add block size /////////////////////////////////////////////////////
        {
            union { struct { quint16 a, b; } s; char c[sizeof(s)]; } u;
            u.s.a = (data.size() / sizeof(quint32)); u.s.b = ~u.s.a;
            block.prepend(u.c, sizeof(u.c));
        }

        // write block ////////////////////////////////////////////////////////

        if(m_ftdi->write(block) != block.size())
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }

        // wait for acknowledge ///////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;

            if(m_ftdi->readBytes(u.c, sizeof(u.c), 500) != sizeof(u.c))
            {
                return tr("Acknowledge error");
            }

            if(u.s != 0x41413535)
            {
                return tr("Acknowledge error");
            }
        }

        if(!binaryFileData.size())
        {
            break;
        }

        binaryFileData.remove(0, settings.s.s0.u.s.maxBlockSize);
    }

    return QString();
}

QString SerialLoader::stage2BinaryLoader(const QString &binaryName)
{
    return stage1BinaryLoader(binaryName);
}

QString SerialLoader::propeller1Load(quint8 targetVersion,
                                     Propeller1Cmd targetCommand,
                                     const QString &binaryName)
{
    QFile binaryFile; qint64 fileSize = QFileInfo().size();

    if(targetCommand != Shutdown)
    {
        binaryFile.setFileName(binaryName);

        if(!binaryFile.open(QIODevice::ReadOnly))
        {
            return tr("Unable to open application binary image \"%1\"").
                   arg(binaryName);
        }

        fileSize = QFileInfo(binaryName).size();

        if(fileSize < MIN_P1_FILE_SIZE)
        {
            return tr("File size less than the min size of %Ln byte(s)",
                      "", MIN_P1_FILE_SIZE);
        }

        if(MAX_P1_FILE_SIZE < fileSize)
        {
            return tr("File size greater than the max size of %Ln byte(s)",
                   "", MAX_P1_FILE_SIZE);
        }
    }

    // hardware reset /////////////////////////////////////////////////////////

    if((!m_ftdi->setBaudRate(FT_BAUD_115200))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1))
    || (!m_ftdi->setDtr(true))
    || (!m_ftdi->setRts(true))
    || (!m_ftdi->setInTask(false, 10))
    || (!m_ftdi->purge(FT_PURGE_TX)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    QThread::msleep(100);

    if((!m_ftdi->purge(FT_PURGE_RX))
    || (!m_ftdi->setInTask(true, 10))
    || (!m_ftdi->setRts(false))
    || (!m_ftdi->setDtr(false)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    QThread::msleep(100);

    // send calibration pulse and lfsr bits ///////////////////////////////////

    QByteArray bytes; bytes.append(0xF9); m_propeller1LFSR = 'P';

    for(int i = 0; i < 250; i++)
    {
        bytes.append(0xFE | iteratePropeller1LFSR());
    }

    if(m_ftdi->write(bytes) != bytes.size())
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // purge bytes ////////////////////////////////////////////////////////////

    if(!m_ftdi->setInTask(false, 10))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    QThread::msleep(50);

    if((!m_ftdi->purge(FT_PURGE_RX))
    || (!m_ftdi->setInTask(true, 10)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // clock in the lfsr bits and version id //////////////////////////////////

    bytes.fill(249, 258);

    if(m_ftdi->write(bytes) != bytes.size())
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    QThread::msleep(50);

    if(m_ftdi->readBytes(&bytes, 100) != 258)
    {
        return tr("Error connecting to the Propeller chip");
    }

    for(int i = 0; i < 250; i++)
    {
        if((bytes.at(i) & 0x01) != iteratePropeller1LFSR())
        {
            return tr("Error connecting to the Propeller chip");
        }
    }

    quint8 version = 0;

    for(int i = 250; i < 258; i++)
    {
        version = (version >> 1) + ((bytes.at(i) & 0x01) << 7);
    }

    if(targetVersion != version)
    {
        return tr("Version %L1 Propeller chip found, not version %L2").
        arg(version).arg(targetVersion);
    }

    // send command ///////////////////////////////////////////////////////////

    bytes = propeller1EncodeLong(targetCommand);

    if(m_ftdi->write(bytes) != bytes.size())
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    if(targetCommand == Shutdown)
    {
        return QString();
    }

    // send count /////////////////////////////////////////////////////////////

    union { qint32 l; char c[sizeof(l)]; } u;

    qint64 size = ((fileSize + sizeof(u.c) - 1) / sizeof(u.c));

    bytes = propeller1EncodeLong(size);

    if(m_ftdi->write(bytes) != bytes.size())
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    bytes.clear();

    for(qint64 i = 0; i < size; i++)
    {
        qMemSet(u.c, 0, sizeof(u.c));

        if(binaryFile.read(u.c, sizeof(u.c)) != (sizeof(u.c)
        - ((i == (size - 1)) ? ((size * sizeof(u.c)) - fileSize) : 0)))
        {
            return binaryFile.errorString();
        }

        bytes.append(propeller1EncodeLong(u.l));
    }

    // program ram ////////////////////////////////////////////////////////////
    {
        m_dialog->setLabelText(tr("Programming RAM..."));

        if(m_ftdi->write(bytes) != bytes.size())
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }
    }

    if(!m_ftdi->setTimeouts(50, 50))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // verify ram /////////////////////////////////////////////////////////////
    {
        m_dialog->setLabelText(tr("Verifying RAM..."));

        bool ok = false;

        for(int i = 0; i < 10; i++)
        {
            if(!m_ftdi->putChar(0xF9))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            char c;

            if(m_ftdi->getChar(&c))
            {
                ok = !(c & 0x01); break;
            }
        }

        if(!ok)
        {
            return tr("RAM verify board error");
        }
    }

    if(targetCommand == LoadRun)
    {
        return QString();
    }

    // program eeprom /////////////////////////////////////////////////////////
    {
        m_dialog->setLabelText(tr("Programming EEPROM..."));

        bool ok = false;

        for(int i = 0; i <= 50; i++) // allow a bit over 5 seconds...
        {
            if(!m_ftdi->putChar(0xF9))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            char c;

            if(m_ftdi->getChar(&c))
            {
                ok = !(c & 0x01); break;
            }
        }

        if(!ok)
        {
            return tr("EEPROM program board error");
        }
    }

    // verify eeprom //////////////////////////////////////////////////////////
    {
        m_dialog->setLabelText(tr("Verifying EEPROM..."));

        bool ok = false;

        for(int i = 0; i <= 50; i++) // allow a bit over 5 seconds...
        {
            if(!m_ftdi->putChar(0xF9))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            char c;

            if(m_ftdi->getChar(&c))
            {
                ok = !(c & 0x01); break;
            }
        }

        if(!ok)
        {
            return tr("EEPROM verify board error");
        }
    }

    return QString();
}

quint8 SerialLoader::iteratePropeller1LFSR()
{
    quint8 bit = m_propeller1LFSR & 1;

    m_propeller1LFSR = (m_propeller1LFSR << 1) |
                     (((m_propeller1LFSR >> 7) ^
                       (m_propeller1LFSR >> 5) ^
                       (m_propeller1LFSR >> 4) ^
                       (m_propeller1LFSR >> 1) )
                                          & 1) ;

    return bit;
}

QByteArray SerialLoader::propeller1EncodeLong(qint32 data)
{
    QByteArray bytes;

    for(int i = 0; i < 10; i++)
    {
        bytes.append(0x92 | (data & 1) |
                     ((data & 2) << 2) |
                     ((data & 4) << 4));

        data >>= 3;
    }

    return bytes.append(0xF2 | (data & 1) | ((data & 2) << 2));
}

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
