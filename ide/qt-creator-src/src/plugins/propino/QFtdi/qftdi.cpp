/***************************************************************************//**
* @file
* FTDI D2XX Wrapper Library
*
* @version @n 1.0
* @date @n 9/15/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 9/15/2013
*******************************************************************************/

/***************************************************************************//**
* @file
* @par Documentation Available:
* @n http://qt-project.org/doc/qt-5.0/qtcore/qiodevice.html
* @n http://www.ftdichip.com/Support/Documents/ProgramGuides/D2XX_Programmer's_Guide(FT_000071).pdf
* @n http://www.ftdichip.com/Support/Documents/AppNotes/AN_152_Detecting_USB_%20Device_Insertion_and_Removal.pdf
*******************************************************************************/

#include "qftdi.h"

QFtdi::QFtdi(QWidget *parent, QSettings *settings) :
    QIODevice(parent),
    QAbstractNativeEventFilter()
{
    m_info = QFtdiInfo();

    m_settings = new QSettings(settings->fileName(), settings->format(), this);

    #ifdef Q_OS_WIN
    {
        m_handle = CreateEvent(0, false, false, 0); Q_CHECK_PTR(m_handle);

        m_winEventNotifier.setHandle(m_handle);

        connect(&m_winEventNotifier, SIGNAL(activated(HANDLE)),
                this, SLOT(eventNotification()));

        m_winEventNotifier.setEnabled(true);

        DEV_BROADCAST_DEVICEINTERFACE dbch_d2xx;

        dbch_d2xx.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        dbch_d2xx.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbch_d2xx.dbcc_classguid.Data1 = 0x219d0508; // D2XX GUID
        dbch_d2xx.dbcc_classguid.Data2 = 0x57a8;
        dbch_d2xx.dbcc_classguid.Data3 = 0x4ff5;
        dbch_d2xx.dbcc_classguid.Data4[0] = 0x97;
        dbch_d2xx.dbcc_classguid.Data4[1] = 0xa1;
        dbch_d2xx.dbcc_classguid.Data4[2] = 0xbd;
        dbch_d2xx.dbcc_classguid.Data4[3] = 0x86;
        dbch_d2xx.dbcc_classguid.Data4[4] = 0x58;
        dbch_d2xx.dbcc_classguid.Data4[5] = 0x7c;
        dbch_d2xx.dbcc_classguid.Data4[6] = 0x6c;
        dbch_d2xx.dbcc_classguid.Data4[7] = 0x7e;
        dbch_d2xx.dbcc_name[0] = 0;

        // parent->effectiveWinId() does not work
        m_d2xxHandle = RegisterDeviceNotification(
        reinterpret_cast<HWND>(parent->winId()),
        &dbch_d2xx, DEVICE_NOTIFY_WINDOW_HANDLE);
        Q_CHECK_PTR(m_d2xxHandle);

        DEV_BROADCAST_DEVICEINTERFACE dbch_vcp;

        dbch_vcp.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        dbch_vcp.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        dbch_vcp.dbcc_classguid.Data1 = 0x86e0d1e0L; // VCP GUID
        dbch_vcp.dbcc_classguid.Data2 = 0x8089;
        dbch_vcp.dbcc_classguid.Data3 = 0x11d0;
        dbch_vcp.dbcc_classguid.Data4[0] = 0x9c;
        dbch_vcp.dbcc_classguid.Data4[1] = 0xe4;
        dbch_vcp.dbcc_classguid.Data4[2] = 0x08;
        dbch_vcp.dbcc_classguid.Data4[3] = 0x00;
        dbch_vcp.dbcc_classguid.Data4[4] = 0x3e;
        dbch_vcp.dbcc_classguid.Data4[5] = 0x30;
        dbch_vcp.dbcc_classguid.Data4[6] = 0x1f;
        dbch_vcp.dbcc_classguid.Data4[7] = 0x73;
        dbch_vcp.dbcc_name[0] = 0;

        // parent->effectiveWinId() does not work
        m_vcpHandle = RegisterDeviceNotification(
        reinterpret_cast<HWND>(parent->winId()),
        &dbch_vcp, DEVICE_NOTIFY_WINDOW_HANDLE);
        Q_CHECK_PTR(m_vcpHandle);
    }
    #endif

    connect(this, SIGNAL(aboutToClose()),
            this, SIGNAL(readChannelFinished()));

    QCoreApplication::instance()->installNativeEventFilter(this);
}

QFtdi::~QFtdi()
{
    close();

    #ifdef Q_OS_WIN
    {
        BOOL result; Q_UNUSED(result);

        result = CloseHandle(m_handle); Q_ASSERT(result);

        result = UnregisterDeviceNotification(m_d2xxHandle); Q_ASSERT(result);
        result = UnregisterDeviceNotification(m_vcpHandle); Q_ASSERT(result);
    }
    #endif

    QCoreApplication::instance()->removeNativeEventFilter(this);
}

bool QFtdi::nativeEventFilter(const QByteArray &eventType,
                              void *message,
                              long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    #ifdef Q_OS_WIN
    {
        PMSG msg = static_cast<PMSG>(message);

        if(msg->message == WM_DEVICECHANGE)
        {
            if(msg->wParam == DBT_DEVNODES_CHANGED)
            {
                emit deviceChanged();
            }

            if(msg->wParam == DBT_DEVICEARRIVAL)
            {
                emit deviceArrived();
            }

            if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)
            {
                emit deviceRemoved();
            }
        }
    }
    #endif

    return false;
}

void QFtdi::eventNotification()
{
    DWORD rx;
    DWORD tx;
    DWORD event;

    FT_STATUS status = FT_GetStatus(m_info.node.ftHandle, &rx, &tx, &event);

    setErrorString(getErrorString(status));

    if(FT_SUCCESS(status))
    {
        if(event & FT_EVENT_RXCHAR)
        {
            emit readyRead();
        }

        if((event & FT_EVENT_MODEM_STATUS) || (event & FT_EVENT_LINE_STATUS))
        {
            DWORD modemStatus;
            DWORD lineStatus;

            if(getModemStatus(&modemStatus, &lineStatus))
            {
                if(event & FT_EVENT_MODEM_STATUS)
                {
                    emit modemStatusChanged(modemStatus);
                }

                if(event & FT_EVENT_LINE_STATUS)
                {
                    emit lineStatusChanged(lineStatus);
                }
            }
        }
    }
}

const QFtdiInfo &QFtdi::getFtdiInfo() const
{
    return m_info;
}

void QFtdi::setFtdiInfo(const QFtdiInfo &info)
{
    close();

    m_info = info;
}

bool QFtdi::atEnd() const
{
    return QIODevice::atEnd();
}

qint64 QFtdi::bytesAvailable()
{
    DWORD result;

    FT_STATUS status = FT_GetQueueStatus(m_info.node.ftHandle, &result);

    setErrorString(getErrorString(status));

    if(FT_SUCCESS(status))
    {
        return result + QIODevice::bytesAvailable();
    }

    return -1;
}

qint64 QFtdi::bytesToWrite() const
{
    return QIODevice::bytesToWrite();
}

bool QFtdi::canReadLine() const
{
    return QIODevice::canReadLine();
}

void QFtdi::close()
{
    if(isOpen())
    {
        QIODevice::close();

        setErrorString(getErrorString(FT_Close(m_info.node.ftHandle)));
    }
}

bool QFtdi::isSequential() const
{
    return true;
}

bool QFtdi::open(OpenMode mode)
{
    FT_STATUS status = FT_OpenEx(m_info.node.SerialNumber,
                                 FT_OPEN_BY_SERIAL_NUMBER,
                                 &m_info.node.ftHandle);

    setErrorString(getErrorString(status));

    if(FT_SUCCESS(status))
    {
        #ifdef Q_OS_WIN
        {
            status = FT_SetEventNotification(m_info.node.ftHandle,
                                             FT_EVENT_RXCHAR |
                                             FT_EVENT_MODEM_STATUS |
                                             FT_EVENT_LINE_STATUS, m_handle);
        }
        #endif

        setErrorString(getErrorString(status));

        if(FT_SUCCESS(status))
        {
            return QIODevice::open(mode);
        }
        else
        {
            FT_Close(m_info.node.ftHandle);
        }
    }

    return false;
}

qint64 QFtdi::pos() const
{
    return QIODevice::pos();
}

bool QFtdi::reset()
{
    return QIODevice::reset();
}

bool QFtdi::seek(qint64 pos)
{
    return QIODevice::seek(pos);
}

qint64 QFtdi::size() const
{
    return QIODevice::size();
}

bool QFtdi::waitForBytesWritten(int msecs) const
{
    QEventLoop loop;

    connect(this, SIGNAL(bytesWritten()), &loop, SLOT(quit()));

    if(msecs >= 0)
    {
        QTimer timer;
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

        timer.setSingleShot(true);
        timer.start(msecs);

        loop.exec();
        return timer.isActive();
    }

    loop.exec();
    return true;
}

bool QFtdi::waitForReadyRead(int msecs) const
{
    QEventLoop loop;

    connect(this, SIGNAL(readyRead()), &loop, SLOT(quit()));

    if(msecs >= 0)
    {
        QTimer timer;
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

        timer.setSingleShot(true);
        timer.start(msecs);

        loop.exec();
        return timer.isActive();
    }

    loop.exec();
    return true;
}

qint64 QFtdi::readBytes(QByteArray *bytes, int count)
{
    return readBytes(bytes->data(), bytes->size(), count);
}

qint64 QFtdi::readBytes(char *data, qint64 size, int count)
{
    qint64 saveSize = size;

    for(int i = 0; size && (i < count); i++)
    {
        qint64 bytesRead = read(data, size);

        if(bytesRead >= 0)
        {
            data += bytesRead; size -= bytesRead;
        }
        else
        {
            return bytesRead;
        }
    }

    return saveSize - size;
}

qint64 QFtdi::writeBytes(const QByteArray *bytes, int count)
{
    return writeBytes(bytes->constData(), bytes->size(), count);
}

qint64 QFtdi::writeBytes(const char *data, qint64 size, int count)
{
    qint64 saveSize = size;

    for(int i = 0; size && (i < count); i++)
    {
        qint64 bytesWritten = write(data, size);

        if(bytesWritten >= 0)
        {
            data += bytesWritten; size -= bytesWritten;
        }
        else
        {
            return bytesWritten;
        }
    }

    return saveSize - size;
}

bool QFtdi::setBaudRate(DWORD baudRate)
{
    FT_STATUS status = FT_SetBaudRate(m_info.node.ftHandle, baudRate);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setDivisor(USHORT divisor)
{
    FT_STATUS status = FT_SetDivisor(m_info.node.ftHandle, divisor);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setDataCharacteristics(UCHAR wordLength,
                                   UCHAR stopBits,
                                   UCHAR parity)
{
    FT_STATUS status = FT_SetDataCharacteristics(m_info.node.ftHandle,
                                                 wordLength, stopBits, parity);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setTimeouts(DWORD readTimeout, DWORD writeTimeout)
{
    FT_STATUS status = FT_SetTimeouts(m_info.node.ftHandle, readTimeout,
                                      writeTimeout);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setFlowControl(USHORT flowControl, UCHAR XOn, UCHAR XOff)
{
    FT_STATUS status = FT_SetFlowControl(m_info.node.ftHandle, flowControl,
                                         XOn, XOff);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setDtr(bool enable)
{
    if(enable)
    {
        FT_STATUS status = FT_SetDtr(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
    else
    {
        FT_STATUS status = FT_ClrDtr(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
}

bool QFtdi::setRts(bool enable)
{
    if(enable)
    {
        FT_STATUS status = FT_SetRts(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
    else
    {
        FT_STATUS status = FT_ClrRts(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
}

bool QFtdi::getModemStatus(LPDWORD modemStatus, LPDWORD lineStatus)
{
    union { DWORD dw; struct { UCHAR a, b, c, d; } s; } u;

    FT_STATUS status = FT_GetModemStatus(m_info.node.ftHandle, &u.dw);

    if(FT_SUCCESS(status))
    {
        if(modemStatus)
        {
            *modemStatus = u.s.a;
        }

        if(lineStatus)
        {
            *lineStatus = u.s.b;
        }
    }

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::getDeviceInfo(FT_DEVICE *type,
                          LPDWORD ID,
                          QString &serialNumber,
                          QString &description)
{
    char pcSerialNumber[16];
    char pcDescription[64];

    FT_STATUS status = FT_GetDeviceInfo(m_info.node.ftHandle, type, ID,
                                        pcSerialNumber, pcDescription, NULL);

    if(FT_SUCCESS(status))
    {
        serialNumber = pcSerialNumber;
        description = pcDescription;
    }

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setChars(UCHAR eventCh, UCHAR eventChEn,
                     UCHAR errorCh, UCHAR errorChEn)
{
    FT_STATUS status = FT_SetChars(m_info.node.ftHandle, eventCh, eventChEn,
                                   errorCh, errorChEn);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setBreak(bool enable)
{
    if(enable)
    {
        FT_STATUS status = FT_SetBreakOn(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
    else
    {
        FT_STATUS status = FT_SetBreakOff(m_info.node.ftHandle);
        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
}

bool QFtdi::purge(DWORD mask)
{
    FT_STATUS status = FT_Purge(m_info.node.ftHandle, mask);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::resetDevice()
{
    FT_STATUS status = FT_ResetDevice(m_info.node.ftHandle);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setInTask(bool enable, int msec)
{
    if(enable)
    {
        QTimer timer;

        timer.setSingleShot(true);
        timer.start(msec);

        FT_STATUS status;

        do
        {
            status = FT_RestartInTask(m_info.node.ftHandle);
        }
        while((!FT_SUCCESS(status)) && timer.isActive());

        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
    else
    {
        QTimer timer;

        timer.setSingleShot(true);
        timer.start(msec);

        FT_STATUS status;

        do
        {
            status = FT_StopInTask(m_info.node.ftHandle);
        }
        while((!FT_SUCCESS(status)) && timer.isActive());

        setErrorString(getErrorString(status));
        return FT_SUCCESS(status);
    }
}

bool QFtdi::setDeadManTimeout(DWORD deadmanTimeout)
{
    FT_STATUS status = FT_SetDeadmanTimeout(m_info.node.ftHandle,
                                            deadmanTimeout);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setLatencyTimer(UCHAR timer)
{
    FT_STATUS status = FT_SetLatencyTimer(m_info.node.ftHandle, timer);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::getLatencyTimer(PUCHAR timer)
{
    FT_STATUS status = FT_GetLatencyTimer(m_info.node.ftHandle, timer);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setBitMode(UCHAR mask, UCHAR mode)
{
    FT_STATUS status = FT_SetBitMode(m_info.node.ftHandle, mask, mode);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::getBitMode(PUCHAR mode)
{
    FT_STATUS status = FT_GetBitMode(m_info.node.ftHandle, mode);
    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::setUSBParameters(DWORD inTransferSize, DWORD outTransferSize)
{
    FT_STATUS status = FT_SetUSBParameters(m_info.node.ftHandle,
                                           inTransferSize, outTransferSize);

    setErrorString(getErrorString(status));
    return FT_SUCCESS(status);
}

bool QFtdi::getDevices(QList<QFtdiInfo> &devs,
                       QSettings *settings,
                       QString &errorString)
{
    devs.clear();

    DWORD numDevs;

    FT_STATUS status = FT_CreateDeviceInfoList(&numDevs);

    errorString = getErrorString(status);

    if(!FT_SUCCESS(status))
    {
        return false;
    }

    if(!numDevs)
    {
        return true;
    }

    FT_DEVICE_LIST_INFO_NODE devInfo[numDevs];

    status = FT_GetDeviceInfoList(devInfo, &numDevs);

    errorString = getErrorString(status);

    if(!FT_SUCCESS(status))
    {
        return false;
    }

    QSettings p_settings(settings->fileName(), settings->format());

    QMap<QString, QFtdiInfo> map;

    for(DWORD i = 0; i < numDevs; i++)
    {
        QFtdiInfo info; info.node = devInfo[i];

        if(!(info.node.Flags & FT_FLAGS_OPENED))
        {
            bool eeprom = false;

            switch(info.node.Type)
            {
                case FT_DEVICE_BM: break;
                case FT_DEVICE_AM: break;
                case FT_DEVICE_100AX: break;
                case FT_DEVICE_UNKNOWN: break;
                case FT_DEVICE_2232C: break;
                case FT_DEVICE_232R: break;
                case FT_DEVICE_2232H: break;
                case FT_DEVICE_4232H: break;
                case FT_DEVICE_232H: break;

                case FT_DEVICE_X_SERIES:
                {
                    status = FT_OpenEx(info.node.SerialNumber,
                                       FT_OPEN_BY_SERIAL_NUMBER,
                                       &info.node.ftHandle);

                    errorString = getErrorString(status);

                    if(!FT_SUCCESS(status))
                    {
                        FT_Close(info.node.ftHandle);

                        return false;
                    }

                    QFtdiEeprom e;

                    for(DWORD j = 0; j < EEPROM_WORD_SIZE; j++)
                    {
                        status = FT_ReadEE(info.node.ftHandle,
                                           EEPROM_BASE_ADDR + j,
                                           e.w + j);

                        errorString = getErrorString(status);

                        if(!FT_SUCCESS(status))
                        {
                            FT_Close(info.node.ftHandle);

                            return false;
                        }
                    }

                    if((e.s.s0.u.s.signature ==
                        EEPROM_SECTION_SIGNATURE)
                    && (e.s.s0.checksum ==
                        qChecksum(e.s.s0.u.c, sizeof(e.s.s0.u.c))))
                    {
                        p_settings.beginGroup(PROPINO_FTDI_KEY_GROUP);
                        p_settings.beginGroup(info.node.SerialNumber);

                        p_settings.setValue(PROPINO_FTDI_KEY_NAME,
                        info.name = QString::fromUtf16(e.s.s0.u.s.name));

                        p_settings.setValue(PROPINO_FTDI_KEY_MAKE,
                        info.make = QString::fromUtf16(e.s.s0.u.s.make));

                        p_settings.endGroup();
                        p_settings.endGroup();

                        eeprom = true;
                    }

                    status = FT_Close(info.node.ftHandle);

                    errorString = getErrorString(status);

                    if(!FT_SUCCESS(status))
                    {
                        FT_Close(info.node.ftHandle);

                        return false;
                    }

                    break;
                }

                default: break;
            }

            if(!eeprom)
            {
                p_settings.beginGroup(PROPINO_FTDI_KEY_GROUP);
                p_settings.beginGroup(info.node.SerialNumber);

                info.name = p_settings.value(PROPINO_FTDI_KEY_NAME,
                tr("Noname Board ") + info.node.SerialNumber).toString();

                info.make = p_settings.value(PROPINO_FTDI_KEY_MAKE,
                PROPINO_FTDI_VAL_MAKE).toString();

                p_settings.endGroup();
                p_settings.endGroup();
            }

            map.insertMulti(info.name, info);
        }
    }

    devs = map.values();

    return true;
}

bool QFtdi::setDevice(const QString &name, const QString &make)
{
    switch(m_info.node.Type)
    {
        case FT_DEVICE_BM: break;
        case FT_DEVICE_AM: break;
        case FT_DEVICE_100AX: break;
        case FT_DEVICE_UNKNOWN: break;
        case FT_DEVICE_2232C: break;
        case FT_DEVICE_232R: break;
        case FT_DEVICE_2232H: break;
        case FT_DEVICE_4232H: break;
        case FT_DEVICE_232H: break;

        case FT_DEVICE_X_SERIES:
        {
            QFtdiEeprom e;

            qMemSet(e.s.s0.u.s.name, 0, sizeof(e.s.s0.u.s.name));
            qMemCopy(e.s.s0.u.s.name, name.utf16(),
            sizeof(ushort) * qMin(name.size(),
            EEPROM_SECTION_0_NAME_LENGTH));
            e.s.s0.u.s.nameTerminator = 0;

            qMemSet(e.s.s0.u.s.make, 0, sizeof(e.s.s0.u.s.make));
            qMemCopy(e.s.s0.u.s.make, make.utf16(),
            sizeof(ushort) * qMin(make.size(),
            EEPROM_SECTION_0_MAKE_LENGTH));
            e.s.s0.u.s.makeTerminator = 0;

            e.s.s0.u.s.signature = EEPROM_SECTION_SIGNATURE;
            e.s.s0.checksum = qChecksum(e.s.s0.u.c, sizeof(e.s.s0.u.c));

            for(DWORD i = 0; i < EEPROM_WORD_SIZE; i++)
            {
                FT_STATUS status = FT_WriteEE(m_info.node.ftHandle,
                                              EEPROM_BASE_ADDR + i,
                                              e.w[i]);

                setErrorString(getErrorString(status));

                if(!FT_SUCCESS(status))
                {
                    return false;
                }
            }

            break;
        }

        default: break;
    }

    m_settings->beginGroup(PROPINO_FTDI_KEY_GROUP);
    m_settings->beginGroup(m_info.node.SerialNumber);

    m_settings->setValue(PROPINO_FTDI_KEY_NAME, m_info.name = name);
    m_settings->setValue(PROPINO_FTDI_KEY_MAKE, m_info.make = make);

    m_settings->endGroup();
    m_settings->endGroup();

    return true;
}

qint64 QFtdi::readData(char *data, qint64 maxlen)
{
    DWORD result;

    FT_STATUS status = FT_Read(m_info.node.ftHandle, data, maxlen, &result);

    setErrorString(getErrorString(status));

    if(FT_SUCCESS(status))
    {
        return result;
    }

    return -1;
}

qint64 QFtdi::readLineData(char *data, qint64 maxlen)
{
    return QIODevice::readLineData(data, maxlen);
}

qint64 QFtdi::writeData(const char *data, qint64 len)
{
    DWORD result;

    FT_STATUS status = FT_Write(m_info.node.ftHandle, const_cast<char*>(data),
                                len, &result);

    setErrorString(getErrorString(status));

    if(FT_SUCCESS(status))
    {
        emit bytesWritten(result);

        return result;
    }

    return -1;
}

const QString &QFtdi::getErrorString(FT_STATUS status)
{
    switch(status)
    {
        case FT_OK:
        {
            static QString s = ""; return s;
        }

        case FT_INVALID_HANDLE:
        {
            static QString s = tr("Invalid Handle"); return s;
        }

        case FT_DEVICE_NOT_FOUND:
        {
            static QString s = tr("Device Not Found"); return s;
        }

        case FT_DEVICE_NOT_OPENED:
        {
            static QString s = tr("Device Not Opened"); return s;
        }

        case FT_IO_ERROR:
        {
            static QString s = tr("IO Error"); return s;
        }

        case FT_INSUFFICIENT_RESOURCES:
        {
            static QString s = tr("Insufficient Resources"); return s;
        }

        case FT_INVALID_PARAMETER:
        {
            static QString s = tr("Invalid Parameter"); return s;
        }

        case FT_INVALID_BAUD_RATE:
        {
            static QString s = tr("Invalid Baud Rate"); return s;
        }

        case FT_DEVICE_NOT_OPENED_FOR_ERASE:
        {
            static QString s = tr("Device Not Opened For Erase"); return s;
        }

        case FT_DEVICE_NOT_OPENED_FOR_WRITE:
        {
            static QString s = tr("Device Not Opened For Write"); return s;
        }

        case FT_FAILED_TO_WRITE_DEVICE:
        {
            static QString s = tr("Failed To Write Device"); return s;
        }

        case FT_EEPROM_READ_FAILED:
        {
            static QString s = tr("EEPROM Read Failed"); return s;
        }

        case FT_EEPROM_WRITE_FAILED:
        {
            static QString s = tr("EEPROM Write Failed"); return s;
        }

        case FT_EEPROM_ERASE_FAILED:
        {
            static QString s = tr("EEPROM Erase Failed"); return s;
        }

        case FT_EEPROM_NOT_PRESENT:
        {
            static QString s = tr("EEPROM Not Present"); return s;
        }

        case FT_EEPROM_NOT_PROGRAMMED:
        {
            static QString s = tr("EEPROM Not Programmed"); return s;
        }

        case FT_INVALID_ARGS:
        {
            static QString s = tr("Invalid Args"); return s;
        }

        case FT_NOT_SUPPORTED:
        {
            static QString s = tr("Not Supported"); return s;
        }

        case FT_OTHER_ERROR:
        {
            static QString s = tr("Other Error"); return s;
        }

        case FT_DEVICE_LIST_NOT_READY:
        {
            static QString s = tr("Device List Not Ready"); return s;
        }

        default:
        {
            static QString s = ""; return s;
        }
    }
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
