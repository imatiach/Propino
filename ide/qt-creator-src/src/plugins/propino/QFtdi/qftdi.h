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

#ifndef QFTDI_H
#define QFTDI_H

#include <QtCore>
#include <QtWidgets>

#include "propino_settings.h"

#ifdef Q_OS_WIN
#include "dbt.h"
#endif

#include "ftd2xx.h"

#define MODEM_CTS 0x10
#define MODEM_DSR 0x20
#define MODEM_RI 0x40
#define MODEM_DCD 0x80

#define LINE_OE 0x02
#define LINE_PE 0x04
#define LINE_FE 0x08
#define LINE_BI 0x10

#define XON_CHAR 17
#define XOFF_CHAR 19

#define EEPROM_BASE_ADDR 0x80
#define EEPROM_SECTION_SIGNATURE 0xAA55

/* FTDI EEPROM Section 0 ******************************************************/

#define EEPROM_SECTION_0_NAME_LENGTH 255 // UTF-16
#define EEPROM_SECTION_0_MAKE_LENGTH 255 // UTF-16

#pragma pack(1) // pack tight

typedef struct QFtdiEepromSection0
{
    union
    {
        struct
        {
            ushort name[EEPROM_SECTION_0_NAME_LENGTH]; ushort nameTerminator;
            ushort make[EEPROM_SECTION_0_MAKE_LENGTH]; ushort makeTerminator;

            ushort signature; // 0xAA55h
        }
        s;

        char c[sizeof(s)];
    }
    u;

    ushort checksum; // CRC-16-CCITT
}
QFtdiEepromSection0;

#pragma pack(0) // pack loose

/******************************************************************************/

#pragma pack(1) // pack tight

typedef union QFtdiEeprom
{
    struct
    {
        QFtdiEepromSection0 s0;
    }
    s;

    WORD w[(sizeof(s) + sizeof(WORD) - 1) / sizeof(WORD)];
}
QFtdiEeprom;

#pragma pack(0) // pack loose

#define EEPROM_BYTE_SIZE sizeof(QFtdiEeprom)
#define EEPROM_WORD_SIZE (EEPROM_BYTE_SIZE / sizeof(WORD))

class QFtdiInfo
{

public:

    QFtdiInfo()
    {
        node.Flags = 0;
        node.Type = 0;
        node.ID = 0;
        node.LocId = 0;

        qMemSet(node.SerialNumber, 0, sizeof(node.SerialNumber));
        qMemSet(node.Description, 0, sizeof(node.Description));

        node.ftHandle = 0;

        name = "";
        make = "";
    }

    FT_DEVICE_LIST_INFO_NODE node;

    QString name;
    QString make;
};

Q_DECLARE_METATYPE(QFtdiInfo)

class QFtdi : public QIODevice, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:

    explicit QFtdi(QWidget *parent, QSettings *settings);
    ~QFtdi();

    const QFtdiInfo &getFtdiInfo() const;
    void setFtdiInfo(const QFtdiInfo &info);

    bool atEnd() const;
    qint64 bytesAvailable();
    qint64 bytesToWrite() const;
    bool canReadLine() const;
    void close();
    bool isSequential() const;
    bool open(OpenMode mode);
    qint64 pos() const;
    bool reset();
    bool seek(qint64 pos);
    qint64 size() const;
    bool waitForBytesWritten(int msecs) const;
    bool waitForReadyRead(int msecs) const;

    qint64 readBytes(QByteArray *bytes, int count);
    qint64 readBytes(char *data, qint64 size, int count);
    qint64 writeBytes(const QByteArray *bytes, int count);
    qint64 writeBytes(const char *data, qint64 size, int count);

    bool setBaudRate(DWORD baudRate);
    bool setDivisor(USHORT divisor);
    bool setDataCharacteristics(UCHAR wordLength,
                                UCHAR stopBits,
                                UCHAR parity);
    bool setTimeouts(DWORD readTimeout, DWORD writeTimeout);
    bool setFlowControl(USHORT flowControl, UCHAR XOn, UCHAR XOff);
    bool setDtr(bool enable);
    bool setRts(bool enable);
    bool getModemStatus(LPDWORD modemStatus, LPDWORD lineStatus);
    bool getDeviceInfo(FT_DEVICE *type,
                       LPDWORD ID,
                       QString &serialNumber,
                       QString &description);
    bool setChars(UCHAR eventCh, UCHAR eventChEn,
                  UCHAR errorCh, UCHAR errorChEn);
    bool setBreak(bool enable);
    bool purge(DWORD mask);
    bool resetDevice();
    bool setInTask(bool enable, int msec);
    bool setDeadManTimeout(DWORD deadmanTimeout);

    bool setLatencyTimer(UCHAR timer);
    bool getLatencyTimer(PUCHAR timer);
    bool setBitMode(UCHAR mask, UCHAR mode);
    bool getBitMode(PUCHAR mode);
    bool setUSBParameters(DWORD inTransferSize, DWORD outTransferSize);

    static bool getDevices(QList<QFtdiInfo> &devs,
                           QSettings *settings,
                           QString &errorString);
    bool setDevice(const QString &name, const QString &make);

private slots:

    void eventNotification();

signals:

    void deviceChanged();
    void deviceArrived();
    void deviceRemoved();

    void modemStatusChanged(DWORD modemStatus);
    void lineStatusChanged(DWORD lineStatus);

protected:

    qint64 readData(char *data, qint64 maxlen);
    qint64 readLineData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private:

    Q_DISABLE_COPY(QFtdi)

    bool nativeEventFilter(const QByteArray &eventType,
                           void *message,
                           long *result);

    static const QString &getErrorString(FT_STATUS status);

    QFtdiInfo m_info; QSettings *m_settings;

    #ifdef Q_OS_WIN
        HANDLE m_handle;
        QWinEventNotifier m_winEventNotifier;
        HDEVNOTIFY m_d2xxHandle;
        HDEVNOTIFY m_vcpHandle;
    #endif
};

#endif // QFTDI_H

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
