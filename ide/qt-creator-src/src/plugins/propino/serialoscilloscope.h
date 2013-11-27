/***************************************************************************//**
* @file
* Serial Oscilloscope
*
* @version @n 1.0
* @date @n 11/18/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 11/18/2013
*******************************************************************************/

#ifndef SERIALOSCILLOSCOPE_H
#define SERIALOSCILLOSCOPE_H

#include <QtConcurrent>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "propino_settings.h"

#include "fftw3.h"
#include "qcustomplot.h"

#define MAX_DATA_SIZE 65536
#define DEF_DATA_SIZE 256

#define MIN_X_DEF_RANGE -1
#define MIN_Y_DEF_RANGE -1

#define MAX_X_DEF_RANGE +1
#define MAX_Y_DEF_RANGE +1

#define UPDATE_RATE (1000 / 50)

class MyQCPCurve : public QCPCurve
{
    Q_OBJECT

public:

    MyQCPCurve(QCPAxis *key, QCPAxis *value) : QCPCurve(key, value)
    {
        deltaMode = false;
        size = DEF_DATA_SIZE;
        sampleRate = 1;
        scaler = 1;

        FFTWOption = none;
        FFTWPlot = NULL;
    }

    bool deltaMode;
    unsigned int size;
    double sampleRate;
    double scaler;

    enum QCPFft
    {
        none,
        forward,
        backward
    }
    FFTWOption;

    QCPCurve *FFTWPlot;

private:

    Q_DISABLE_COPY(MyQCPCurve)
};

class MyQCPGraph : public QCPGraph
{
    Q_OBJECT

public:

    MyQCPGraph(QCPAxis *key, QCPAxis *value) : QCPGraph(key, value)
    {
        deltaMode = false;
        size = DEF_DATA_SIZE;
        sampleRate = 1;
        scaler = 1;

        FFTWOption = none;
        FFTWPlot = NULL;
    }

    bool deltaMode;
    unsigned int size;
    double sampleRate;
    double scaler;

    enum QCPFft
    {
        none,
        magnitude,
        magnitude2,
        magnitude10,
        magnitude20,
        phaseRadians,
        phaseDegrees
    }
    FFTWOption;

    QCPGraph *FFTWPlot;

private:

    Q_DISABLE_COPY(MyQCPGraph)
};

namespace Ui
{
    class SerialOscilloscope;
}

class SerialOscilloscope : public QMainWindow
{
    Q_OBJECT

public:

    explicit SerialOscilloscope(const QString &title,
                                QSettings *settings,
                                QWidget *parent = NULL);
    ~SerialOscilloscope();

    static void initFttw(QSettings *settings, QWidget *parent = NULL);
    static void finiFttw();

    void newBarChart(unsigned int handle, const QString &name);
    void newCurve(unsigned int handle, const QString &name);
    void newGraph(unsigned int handle, const QString &name);
    void newBoxChart(unsigned int handle, const QString &name);

    void setLineColor(unsigned int handle, QRgb rgb);
    void setFillColor(unsigned int handle, QRgb rgb);
    void setLineStyle(unsigned int handle, Qt::PenStyle ps);
    void setFillStyle(unsigned int handle, Qt::BrushStyle bs);

    void setCurveDeltaMode(unsigned int handle, bool enable);
    void setGraphDeltaMode(unsigned int handle, bool enable);
    void setCurveSize(unsigned int handle, unsigned int size);
    void setGraphSize(unsigned int handle, unsigned int size);

    void setCurveSampleRate(unsigned int handle, double sampleRate);
    void setGraphSampleRate(unsigned int handle, double sampleRate);
    void setCurveScaler(unsigned int handle, double scaler);
    void setGraphScaler(unsigned int handle, double scaler);

    void addBarData(unsigned int handle,
                    double key,
                    double value);

    void setBoxData(unsigned int handle,
                    double key,
                    double minimum,
                    double lowerQuartile,
                    double median,
                    double upperQuartile,
                    double maximum);

    void addCurveData(unsigned int handle,
                      double key,
                      double value);

    void addCurveData(unsigned int handle,
                      double time,
                      double key,
                      double value);

    void addGraphData(unsigned int handle,
                      double value);

    void addGraphData(unsigned int handle,
                      double key,
                      double value);

    void setBarAbove(unsigned int handle, unsigned int target);
    void setBarBelow(unsigned int handle, unsigned int target);

    void setCurveDrawingStyle(unsigned int handle, QCPCurve::LineStyle ls);
    void setCurveScatterStyle(unsigned int handle,
                              QCPScatterStyle::ScatterShape ss);
    void setCurveFillCurve(unsigned int handle, unsigned int target);
    void setCurveFFT(unsigned int handle, MyQCPCurve::QCPFft option);

    void setGraphDrawingStyle(unsigned int handle, QCPGraph::LineStyle ls);
    void setGraphScatterStyle(unsigned int handle,
                              QCPScatterStyle::ScatterShape ss);
    void setGraphFillGraph(unsigned int handle, unsigned int target);
    void setGraphFFT(unsigned int handle, MyQCPGraph::QCPFft option);

    void clearData(unsigned int handle);
    void clearPlot(unsigned int handle);

    void clearAllDatum();
    void clearAllPlots();

    void setXAxisScaleType(QCPAxis::ScaleType st);
    void setXAxisScaleLogBase(double base);
    void setXAxisRangeReversed(bool reversed);
    void setXAxisLabel(const QString &name);

    void setYAxisScaleType(QCPAxis::ScaleType st);
    void setYAxisScaleLogBase(double base);
    void setYAxisRangeReversed(bool reversed);
    void setYAxisLabel(const QString &name);

    void setFFTXAxisScaleType(QCPAxis::ScaleType st);
    void setFFTXAxisScaleLogBase(double base);
    void setFFTXAxisRangeReversed(bool reversed);
    void setFFTXAxisLabel(const QString &name);

    void setFFTYAxisScaleType(QCPAxis::ScaleType st);
    void setFFTYAxisScaleLogBase(double base);
    void setFFTYAxisRangeReversed(bool reversed);
    void setFFTYAxisLabel(const QString &name);

    void setPlotBackgroundColor(QRgb bc);

private slots:

    void zoomPlotDefault(QMouseEvent *event = NULL);
    void zoomMathDefault(QMouseEvent *event = NULL);

    void saveBmp();
    void saveJpg();
    void savePng();
    void savePdf();

    void exportState();
    void importState();

    void generalHelp();
    void oscilloscopeHelp();

    void plotMousePress(QMouseEvent *event);
    void plotMouseWheel(QWheelEvent *event);

    void mathMousePress(QMouseEvent *event);
    void mathMouseWheel(QWheelEvent *event);

    void replot();

signals:

    void error(const QString &message);

protected:

    void closeEvent(QCloseEvent *event);

private:

    Q_DISABLE_COPY(SerialOscilloscope)

    static const QPen m_pens[];
    static const QPen m_spens[];
    static const QBrush m_brushes[];
    static const QBrush m_sbrushes[];

    static const int m_fftw_sizes[];
    static QMap<int, fftw_plan> m_fftw_plans, m_fftw_f_plans, m_fftw_b_plans;
    static bool m_fftw_multi_threaded;
    static bool m_fftw_enabled;

    bool m_rescalePlotAxes;
    bool m_rescaleMathAxes;

    QColor m_background;

    QMap<unsigned int, QCPAbstractPlottable*> m_plottables;

    QMutex m_replotLock; void scheduleReplot();

    static void calculateCurveFft(MyQCPCurve *curve);
    static void calculateGraphFft(MyQCPGraph *graph);

    template <class QCP>
    void newPlottable(unsigned int handle, const QString &name);

    template <class QCP>
    QCP *verifyHandle(unsigned int handle, const QString &name);

    void error(unsigned int handle, const QString &message);

    QSettings *m_settings; Ui::SerialOscilloscope *m_ui;
};

#endif // SERIALOSCILLOSCOPE_H

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
