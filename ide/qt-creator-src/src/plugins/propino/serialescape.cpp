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

#include "serialescape.h"

SerialEscape::SerialEscape(QFtdi *ftdi, QSettings *settings) :
    QObject(ftdi)
{
    Q_CHECK_PTR(ftdi); Q_CHECK_PTR(settings);

    m_ftdi = ftdi; m_settings = settings; enabled = true; readState = TERMINAL;

    connect(m_ftdi, SIGNAL(aboutToClose()),
            this, SLOT(aboutToClose()));

    connect(m_ftdi, SIGNAL(lineStatusChanged(DWORD)),
            this, SLOT(lineStatusChanged(DWORD)));

    connect(m_ftdi, SIGNAL(readyRead()),
            this, SLOT(readBytes()));

    // New SerialOscilloscope (Default)

    SerialOscilloscope *oscilloscope = new SerialOscilloscope(
    tr("Propino Serial Oscilloscope"), m_settings);

    oscilloscope->setAttribute(Qt::WA_DeleteOnClose, false);

    oscilloscopes.insert(0, oscilloscope);

    connect(oscilloscope, SIGNAL(error(QString)),
            this, SIGNAL(error(QString)));

    connect(oscilloscope, SIGNAL(destroyed(QObject*)),
            this, SLOT(oscilloscopeDestroyed(QObject*)));

    // New SerialTerminal (Default)

    SerialTerminal *terminal = new SerialTerminal(
    tr("Propino Serial Terminal"), m_settings);

    terminal->setAttribute(Qt::WA_DeleteOnClose, false);

    terminals.insert(0, terminal);

    connect(terminal, SIGNAL(transmit(QByteArray)),
            this, SLOT(writeBytes(QByteArray)));

    connect(terminal, SIGNAL(destroyed(QObject*)),
            this, SLOT(terminalDestroyed(QObject*)));
}

SerialEscape::~SerialEscape()
{
    foreach(SerialOscilloscope *oscilloscope, oscilloscopes.values())
    {
        oscilloscope->setAttribute(Qt::WA_DeleteOnClose, true);

        oscilloscope->close();
    }

    foreach(SerialTerminal *terminal, terminals.values())
    {
        terminal->setAttribute(Qt::WA_DeleteOnClose, true);

        terminal->close();
    }
}

const QMap<unsigned int, SerialOscilloscope*> &SerialEscape::getOscilloscopes()
{
    return oscilloscopes;
}

const QMap<unsigned int, SerialTerminal*> &SerialEscape::getTerminals()
{
    return terminals;
}

void SerialEscape::deleteOscilloscope(SerialOscilloscope *oscilloscope)
{
    Q_CHECK_PTR(oscilloscope);

    oscilloscope->setAttribute(Qt::WA_DeleteOnClose, true);

    oscilloscope->close();
}

void SerialEscape::deleteTerminal(SerialTerminal *terminal)
{
    Q_CHECK_PTR(terminal);

    terminal->setAttribute(Qt::WA_DeleteOnClose, true);

    terminal->close();
}

bool SerialEscape::getEnabled()
{
    return enabled;
}

void SerialEscape::setEnabled(bool en)
{
    if(enabled && (!en))
    {
        readState = TERMINAL;
    }

    bool previous = enabled; enabled = en;

    if((!previous) && en)
    {
        readBytes();
    }
}

void SerialEscape::aboutToClose()
{
    foreach(SerialOscilloscope *oscilloscope, oscilloscopes.values())
    {
        oscilloscope->close();
    }

    foreach(SerialTerminal *terminal, terminals.values())
    {
        terminal->close();
    }

    if(enabled)
    {
        readState = TERMINAL;
    }
}

void SerialEscape::lineStatusChanged(DWORD lineStatus)
{
    if(enabled && (lineStatus & LINE_BI))
    {
        DWORD time = calcUSBRWTimeouts(FT_BAUD_115200);
        DWORD size = calcUSBParameters(FT_BAUD_115200);

        if((!m_ftdi->setBaudRate(FT_BAUD_115200))
        || (!m_ftdi->setTimeouts(time, time))
        || (!m_ftdi->setUSBParameters(size, size)))
        {
            emit error(tr("Serial Engine: ") + m_ftdi->errorString());
        }

        readState = TERMINAL;
    }
}

void SerialEscape::oscilloscopeDestroyed(QObject *obj)
{
    oscilloscopes.remove(oscilloscopes.key(
    static_cast<SerialOscilloscope*>(obj)));
}

void SerialEscape::terminalDestroyed(QObject *obj)
{
    terminals.remove(terminals.key(
    static_cast<SerialTerminal*>(obj)));
}

void SerialEscape::writeBytes(const QByteArray &bytes)
{
    if(enabled)
    {
        QWidget *parent = qobject_cast<QWidget*>(sender());

        QProgressDialog dialog(parent, Qt::MSWindowsFixedSizeDialogHint |
                                       Qt::WindowTitleHint |
                                       Qt::WindowSystemMenuHint |
                                       Qt::WindowCloseButtonHint);

        dialog.setWindowTitle(tr("Serial Engine"));
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.setLabelText(tr("Sending Data..."));
        dialog.setRange(0, bytes.size());

        const char *data = bytes.constData(); qint64 size = bytes.size();

        while(size && (!dialog.wasCanceled()))
        {
            qint64 bytesWritten = m_ftdi->write(data, size);

            if(bytesWritten >= 0)
            {
                data += bytesWritten; size -= bytesWritten;
            }
            else
            {
                break;
            }

            dialog.setValue(bytes.size() - size);
        }
    }
}

void SerialEscape::readBytes()
{
    if(enabled)
    {
        QByteArray buffer; QByteArray data(m_ftdi->readAll());

        for(int i = 0; i < data.size(); i++)
        {
            unsigned char c = static_cast<unsigned char>(data.at(i));

            switch(readState)
            {
                case TERMINAL:
                {
                    if(data.at(i) != '\e')
                    {
                        buffer.append(data.at(i));
                    }
                    else
                    {
                        if(terminals.contains(0))
                        {
                            terminals.value(0)->receive(buffer);

                            buffer.clear();
                        }

                        readState = ESCAPE;
                    }

                    break;
                }

                case ESCAPE:
                {
                    readFunction = static_cast<SerialFunction>(c);

                    if(readFunction != EXTENDED_COMMAND)
                    {
                        readState = ESCAPE_S;
                    }
                    else
                    {
                        readState = ESCAPE_L;
                    }

                    break;
                }

                case ESCAPE_S:
                {
                    union { char c; struct { unsigned int a:4, b:4; } s; } u;

                    u.c = data.at(i);

                    readWHandle = 0;
                    readCHandle = u.s.a;
                    readDataType = static_cast<SerialDataType>(u.s.b);

                    readState = LENGTH;

                    break;
                }

                case ESCAPE_L:
                {
                    readDataType = static_cast<SerialDataType>(c);

                    readState = LENGTH;

                    break;
                }

                case LENGTH:
                {
                    argumentData.clear(); readLength = c + 1;

                    switch(readDataType)
                    {
                        case U_8_BIT: readLength *= sizeof(quint8); break;
                        case S_8_BIT: readLength *= sizeof(qint8); break;
                        case U_16_BIT: readLength *= sizeof(quint16); break;
                        case S_16_BIT: readLength *= sizeof(qint16); break;
                        case U_32_BIT: readLength *= sizeof(quint32); break;
                        case S_32_BIT: readLength *= sizeof(qint32); break;
                        case U_64_BIT: readLength *= sizeof(quint64); break;
                        case S_64_BIT: readLength *= sizeof(qint64); break;
                        case F_32_BIT: readLength *= sizeof(float); break;
                        case F_64_BIT: readLength *= sizeof(double); break;

                        default:

                        emit error(tr("Serial Engine: Invalid serial data type"
                        " %L1").arg(readDataType)); break;
                    }

                    if(readFunction == EXTENDED_COMMAND)
                    {
                        readLength += sizeof(qint32)
                                   +  sizeof(quint32)
                                   +  sizeof(quint32);
                    }

                    readState = PARAMETERS;

                    break;
                }

                case PARAMETERS:
                {
                    argumentData.append(data.at(i)); readLength--;

                    if(!readLength)
                    {
                        if(readFunction == EXTENDED_COMMAND)
                        {
                            SerialDataType temp = readDataType;

                            readDataType = S_32_BIT;
                            readFunction = static_cast<SerialFunction>
                                           (parseArguments<int>());

                            readDataType = U_32_BIT;
                            readWHandle = parseArguments<unsigned int>();
                            readCHandle = parseArguments<unsigned int>();

                            readDataType = temp;
                        }

                        executeCommand();

                        readState = TERMINAL;
                    }

                    break;
                }
            }
        }

        if(terminals.contains(0))
        {
            terminals.value(0)->receive(buffer);

            buffer.clear();
        }
    }
}

void SerialEscape::executeCommand()
{
    switch(readFunction)
    {
        case NULL_COMMAND: break;

        case SET_BAUDRATE:
        {
            unsigned int baudRate = parseArguments<unsigned int>();

            if((2400 <= baudRate) && (baudRate <= 3000000))
            {
                DWORD time = calcUSBRWTimeouts(baudRate);
                DWORD size = calcUSBParameters(baudRate);

                if((!m_ftdi->setBaudRate(baudRate))
                || (!m_ftdi->setTimeouts(time, time))
                || (!m_ftdi->setUSBParameters(size, size)))
                {
                    emit error(tr("Serial Engine: ") + m_ftdi->errorString());
                }

                readState = TERMINAL;
            }
            else
            {
                emit error(tr("Serial Engine: Invalid baudrate "
                "%L1").arg(baudRate));
            }

            break;
        }

        case NEW_BAR_CHART:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->newBarChart(
                readCHandle, argumentData);

                oscilloscopes.value(readWHandle)->show();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case NEW_CURVE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->newCurve(
                readCHandle, argumentData);

                oscilloscopes.value(readWHandle)->show();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case NEW_GRAPH:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->newGraph(
                readCHandle, argumentData);

                oscilloscopes.value(readWHandle)->show();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case NEW_BOX_CHART:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->newBoxChart(
                readCHandle, argumentData);

                oscilloscopes.value(readWHandle)->show();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_LINE_COLOR:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setLineColor(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FILL_COLOR:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFillColor(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_LINE_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setLineStyle(
                readCHandle, static_cast<Qt::PenStyle>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FILL_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFillStyle(
                readCHandle, static_cast<Qt::BrushStyle>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_DELTA_MODE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveDeltaMode(
                readCHandle, parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_DELTA_MODE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphDeltaMode(
                readCHandle, parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_SIZE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveSize(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_SIZE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphSize(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_SAMPLE_RATE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveSampleRate(
                readCHandle, parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_SAMPLE_RATE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphSampleRate(
                readCHandle, parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_SCALER:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveScaler(
                readCHandle, parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_SCALER:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphScaler(
                readCHandle, parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case ADD_BAR_DATA:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double key = parseArguments<double>();
                    double value = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->addBarData(
                    readCHandle, key,
                                 value);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_BOX_DATA:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double key = parseArguments<double>();
                    double minimum = parseArguments<double>();
                    double lowerQuartile = parseArguments<double>();
                    double median = parseArguments<double>();
                    double upperQuartile = parseArguments<double>();
                    double maximum = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->setBoxData(
                    readCHandle, key,
                                 minimum,
                                 lowerQuartile,
                                 median,
                                 upperQuartile,
                                 maximum);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case ADD_CURVE_DATA_0:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double key = parseArguments<double>();
                    double value = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->addCurveData(
                    readCHandle, key,
                                 value);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case ADD_CURVE_DATA_1:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double time = parseArguments<double>();
                    double key = parseArguments<double>();
                    double value = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->addCurveData(
                    readCHandle, time,
                                 key,
                                 value);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case ADD_GRAPH_DATA_0:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double value = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->addGraphData(
                    readCHandle, value);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case ADD_GRAPH_DATA_1:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                while(argumentData.size())
                {
                    double key = parseArguments<double>();
                    double value = parseArguments<double>();

                    oscilloscopes.value(readWHandle)->addGraphData(
                    readCHandle, key,
                                 value);
                }
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_BAR_ABOVE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setBarAbove(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_BAR_BELOW:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setBarBelow(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_DRAWING_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveDrawingStyle(
                readCHandle, static_cast<QCPCurve::LineStyle>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_SCATTER_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveScatterStyle(
                readCHandle, static_cast<QCPScatterStyle::ScatterShape>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_FILL_CURVE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveFillCurve(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_CURVE_FFT:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setCurveFFT(
                readCHandle, static_cast<MyQCPCurve::QCPFft>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_DRAWING_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphDrawingStyle(
                readCHandle, static_cast<QCPGraph::LineStyle>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_SCATTER_STYLE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphScatterStyle(
                readCHandle, static_cast<QCPScatterStyle::ScatterShape>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_FILL_GRAPH:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphFillGraph(
                readCHandle, parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_GRAPH_FFT:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setGraphFFT(
                readCHandle, static_cast<MyQCPGraph::QCPFft>(
                parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLEAR_DATA:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->clearData(readCHandle);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLEAR_PLOT:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->clearPlot(readCHandle);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLEAR_ALL_DATUM:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->clearAllDatum();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLEAR_ALL_PLOTS:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->clearAllPlots();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_X_AXIS_SCALE_TYPE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setXAxisScaleType(
                static_cast<QCPAxis::ScaleType>(parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_X_AXIS_SCALE_LOG_BASE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setXAxisScaleLogBase(
                parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_X_AXIS_RANGE_REVERSED:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setXAxisRangeReversed(
                parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_X_AXIS_LABEL:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setXAxisLabel(
                argumentData);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_Y_AXIS_SCALE_TYPE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setYAxisScaleType(
                static_cast<QCPAxis::ScaleType>(parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_Y_AXIS_SCALE_LOG_BASE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setYAxisScaleLogBase(
                parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_Y_AXIS_RANGE_REVERSED:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setYAxisRangeReversed(
                parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_Y_AXIS_LABEL:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setYAxisLabel(
                argumentData);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_X_AXIS_SCALE_TYPE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTXAxisScaleType(
                static_cast<QCPAxis::ScaleType>(parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_X_AXIS_SCALE_LOG_BASE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTXAxisScaleLogBase(
                parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_X_AXIS_RANGE_REVERSED:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTXAxisRangeReversed(
                parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_X_AXIS_LABEL:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTXAxisLabel(
                argumentData);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_Y_AXIS_SCALE_TYPE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTYAxisScaleType(
                static_cast<QCPAxis::ScaleType>(parseArguments<int>()));
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_Y_AXIS_SCALE_LOG_BASE:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTYAxisScaleLogBase(
                parseArguments<double>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_Y_AXIS_RANGE_REVERSED:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTYAxisRangeReversed(
                parseArguments<bool>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case SET_FFT_Y_AXIS_LABEL:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setFFTYAxisLabel(
                argumentData);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case EXTENDED_COMMAND: break;

        case SET_PLOT_BACKGROUND_COLOR:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->setPlotBackgroundColor(
                parseArguments<unsigned int>());
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case TERMINAL_PRINT:
        {
            if(terminals.contains(readWHandle))
            {
                terminals.value(readWHandle)->receive(argumentData);
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case NEW_OSCILLOSCOPE_WINDOW:
        {
            if(argumentData.isEmpty())
            {
                argumentData=tr("Oscilloscope %L1").arg(readWHandle).toUtf8();
            }

            QString check = argumentData; check.replace(QRegExp("[\\/]"), " ");

            foreach(SerialOscilloscope *o, oscilloscopes.values())
            {
                if((!(o->windowTitle().compare(check, Qt::CaseInsensitive)))
                && (oscilloscopes.key(o) != readWHandle))
                {
                    emit error(tr("Serial Engine: Window %L1's name is "
                    "already in use by window %L2").arg(readWHandle).
                    arg(oscilloscopes.key(o))); return;
                }
            }

            if(!oscilloscopes.contains(readWHandle))
            {
                SerialOscilloscope *oscilloscope = new SerialOscilloscope(
                argumentData, m_settings);

                oscilloscope->setAttribute(Qt::WA_DeleteOnClose, false);

                connect(oscilloscope, SIGNAL(error(QString)),
                        this, SIGNAL(error(QString)));

                connect(oscilloscope, SIGNAL(destroyed(QObject*)),
                        this, SLOT(oscilloscopeDestroyed(QObject*)));

                oscilloscopes.insert(readWHandle, oscilloscope);
                oscilloscope->show();
            }
            else
            {
                oscilloscopes.value(readWHandle)->setWindowTitle(
                argumentData); oscilloscopes.value(readWHandle)->show();
            }

            break;
        }

        case CLOSE_OSCILLOSCOPE_WINDOW:
        {
            if(oscilloscopes.contains(readWHandle))
            {
                oscilloscopes.value(readWHandle)->close();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLOSE_ALL_OSCILLOSCOPES:
        {
            foreach(SerialOscilloscope *oscilloscope, oscilloscopes.values())
            {
                oscilloscope->close();
            }

            break;
        }

        case NEW_TERMINAL_WINDOW:
        {
            if(argumentData.isEmpty())
            {
                argumentData=tr("Terminal %L1").arg(readWHandle).toUtf8();
            }

            QString check = argumentData; check.replace(QRegExp("[\\/]"), " ");

            foreach(SerialTerminal *t, terminals.values())
            {
                if((!(t->windowTitle().compare(check, Qt::CaseInsensitive)))
                && (terminals.key(t) != readWHandle))
                {
                    emit error(tr("Serial Engine: Window %L1's name is "
                    "already in use by window %L2").arg(readWHandle).
                    arg(terminals.key(t))); return;
                }
            }

            if(!terminals.contains(readWHandle))
            {
                SerialTerminal *terminal = new SerialTerminal(
                argumentData, m_settings);

                terminal->setAttribute(Qt::WA_DeleteOnClose, false);

                connect(terminal, SIGNAL(transmit(QByteArray)),
                        this, SLOT(writeBytes(QByteArray)));

                connect(terminal, SIGNAL(destroyed(QObject*)),
                        this, SLOT(terminalDestroyed(QObject*)));

                terminals.insert(readWHandle, terminal);
                terminal->show();
            }
            else
            {
                terminals.value(readWHandle)->setWindowTitle(
                argumentData); terminals.value(readWHandle)->show();
            }

            break;
        }

        case CLOSE_TERMINAL_WINDOW:
        {
            if(terminals.contains(readWHandle))
            {
                terminals.value(readWHandle)->close();
            }
            else
            {
                invalidWindowHandleNumber();
            }

            break;
        }

        case CLOSE_ALL_TERMINALS:
        {
            foreach(SerialTerminal *terminal, terminals.values())
            {
                terminal->close();
            }

            break;
        }

        default:
        {
            emit error(tr("Serial Engine: Invalid function enum number "
            "%L1").arg(readFunction)); break;
        }
    }
}

void SerialEscape::invalidWindowHandleNumber()
{
    emit error(tr("Serial Engine: Invalid window handle number "
    "%L1").arg(readWHandle));
}

template <class T> T SerialEscape::parseArguments()
{
    switch(readDataType)
    {
        case U_8_BIT:
        {
            union { quint8 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case S_8_BIT:
        {
            union { qint8 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case U_16_BIT:
        {
            union { quint16 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case S_16_BIT:
        {
            union { qint16 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case U_32_BIT:
        {
            union { quint32 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case S_32_BIT:
        {
            union { qint32 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case U_64_BIT:
        {
            union { quint64 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case S_64_BIT:
        {
            union { qint64 t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case F_32_BIT:
        {
            union { float t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        case F_64_BIT:
        {
            union { double t; char c[sizeof(t)]; } u;

            qMemSet(u.c, 0, sizeof(u.c));
            qMemCopy(u.c, argumentData.constData(),
            qMin(argumentData.size(), static_cast<int>(sizeof(u.c))));
            argumentData.remove(0, sizeof(u.c));

            return static_cast<T>(u.t);
        }

        default: return static_cast<T>(0);
    }
}

/***************************************************************************//**
* @file
* @par Documentation Available:
* @n http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-03_D2XXDataThroughput.pdf
* @n http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-04_DataLatencyFlow.pdf
*******************************************************************************/

DWORD calcUSBRWTimeouts(DWORD targetBaudRate)
{
    DWORD i = (calcUSBParameters(targetBaudRate) * 1000 * (1 + 8 + 1));

    return qMin((i + targetBaudRate - 1) / targetBaudRate, TARGET_MAX_TIMEOUT);
}

DWORD calcUSBParameters(DWORD targetBaudRate)
{
    DWORD i = ((1 + 8 + 1) * (64 - 2) * TARGET_UPDATE_RATE);

    return (((targetBaudRate + i - 1) / i) * 64);
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
