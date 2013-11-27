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

#include "serialoscilloscope.h"
#include "ui_serialoscilloscope.h"

const QPen SerialOscilloscope::m_pens[] =
{
    QPen(QBrush(QColor(000, 000, 255)), 0, Qt::SolidLine),
    QPen(QBrush(QColor(255, 000, 000)), 0, Qt::SolidLine),
    QPen(QBrush(QColor(000, 255, 000)), 0, Qt::SolidLine),
    QPen(QBrush(QColor(255, 000, 255)), 0, Qt::SolidLine),
    QPen(QBrush(QColor(000, 255, 255)), 0, Qt::SolidLine),
    QPen(QBrush(QColor(255, 255, 000)), 0, Qt::SolidLine),

    QPen(QBrush(QColor(000, 000, 255)), 0, Qt::DashLine),
    QPen(QBrush(QColor(255, 000, 000)), 0, Qt::DashLine),
    QPen(QBrush(QColor(000, 255, 000)), 0, Qt::DashLine),
    QPen(QBrush(QColor(255, 000, 255)), 0, Qt::DashLine),
    QPen(QBrush(QColor(000, 255, 255)), 0, Qt::DashLine),
    QPen(QBrush(QColor(255, 255, 000)), 0, Qt::DashLine),

    QPen(QBrush(QColor(000, 000, 255)), 0, Qt::DotLine),
    QPen(QBrush(QColor(255, 000, 000)), 0, Qt::DotLine),
    QPen(QBrush(QColor(000, 255, 000)), 0, Qt::DotLine),
    QPen(QBrush(QColor(255, 000, 255)), 0, Qt::DotLine),
    QPen(QBrush(QColor(000, 255, 255)), 0, Qt::DotLine),
    QPen(QBrush(QColor(255, 255, 000)), 0, Qt::DotLine),

    QPen(QBrush(QColor(000, 000, 255)), 0, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 000, 000)), 0, Qt::DashDotLine),
    QPen(QBrush(QColor(000, 255, 000)), 0, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 000, 255)), 0, Qt::DashDotLine),
    QPen(QBrush(QColor(000, 255, 255)), 0, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 255, 000)), 0, Qt::DashDotLine),

    QPen(QBrush(QColor(000, 000, 255)), 0, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 000, 000)), 0, Qt::DashDotDotLine),
    QPen(QBrush(QColor(000, 255, 000)), 0, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 000, 255)), 0, Qt::DashDotDotLine),
    QPen(QBrush(QColor(000, 255, 255)), 0, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 255, 000)), 0, Qt::DashDotDotLine)
};

const QPen SerialOscilloscope::m_spens[] =
{
    QPen(QBrush(QColor(000, 000, 255)), 3, Qt::SolidLine),
    QPen(QBrush(QColor(255, 000, 000)), 3, Qt::SolidLine),
    QPen(QBrush(QColor(000, 255, 000)), 3, Qt::SolidLine),
    QPen(QBrush(QColor(255, 000, 255)), 3, Qt::SolidLine),
    QPen(QBrush(QColor(000, 255, 255)), 3, Qt::SolidLine),
    QPen(QBrush(QColor(255, 255, 000)), 3, Qt::SolidLine),

    QPen(QBrush(QColor(000, 000, 255)), 3, Qt::DashLine),
    QPen(QBrush(QColor(255, 000, 000)), 3, Qt::DashLine),
    QPen(QBrush(QColor(000, 255, 000)), 3, Qt::DashLine),
    QPen(QBrush(QColor(255, 000, 255)), 3, Qt::DashLine),
    QPen(QBrush(QColor(000, 255, 255)), 3, Qt::DashLine),
    QPen(QBrush(QColor(255, 255, 000)), 3, Qt::DashLine),

    QPen(QBrush(QColor(000, 000, 255)), 3, Qt::DotLine),
    QPen(QBrush(QColor(255, 000, 000)), 3, Qt::DotLine),
    QPen(QBrush(QColor(000, 255, 000)), 3, Qt::DotLine),
    QPen(QBrush(QColor(255, 000, 255)), 3, Qt::DotLine),
    QPen(QBrush(QColor(000, 255, 255)), 3, Qt::DotLine),
    QPen(QBrush(QColor(255, 255, 000)), 3, Qt::DotLine),

    QPen(QBrush(QColor(000, 000, 255)), 3, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 000, 000)), 3, Qt::DashDotLine),
    QPen(QBrush(QColor(000, 255, 000)), 3, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 000, 255)), 3, Qt::DashDotLine),
    QPen(QBrush(QColor(000, 255, 255)), 3, Qt::DashDotLine),
    QPen(QBrush(QColor(255, 255, 000)), 3, Qt::DashDotLine),

    QPen(QBrush(QColor(000, 000, 255)), 3, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 000, 000)), 3, Qt::DashDotDotLine),
    QPen(QBrush(QColor(000, 255, 000)), 3, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 000, 255)), 3, Qt::DashDotDotLine),
    QPen(QBrush(QColor(000, 255, 255)), 3, Qt::DashDotDotLine),
    QPen(QBrush(QColor(255, 255, 000)), 3, Qt::DashDotDotLine)
};

const QBrush SerialOscilloscope::m_brushes[] =
{
    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush)
};

const QBrush SerialOscilloscope::m_sbrushes[] =
{
    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush),

    QBrush(QColor(200, 200, 255), Qt::NoBrush),
    QBrush(QColor(255, 200, 200), Qt::NoBrush),
    QBrush(QColor(200, 255, 200), Qt::NoBrush),
    QBrush(QColor(255, 200, 255), Qt::NoBrush),
    QBrush(QColor(200, 255, 255), Qt::NoBrush),
    QBrush(QColor(255, 255, 200), Qt::NoBrush)
};

const int SerialOscilloscope::m_fftw_sizes[] =
{
    2,      4,      8,      16,     32,     64,     128,    256,
    512,    1024,   2048,   4096,   8192,   16384,  32768,  65536
};

QMap<int, fftw_plan> SerialOscilloscope::m_fftw_plans=QMap<int, fftw_plan>();
QMap<int, fftw_plan> SerialOscilloscope::m_fftw_f_plans=QMap<int, fftw_plan>();
QMap<int, fftw_plan> SerialOscilloscope::m_fftw_b_plans=QMap<int, fftw_plan>();
bool SerialOscilloscope::m_fftw_multi_threaded=false;
bool SerialOscilloscope::m_fftw_enabled=false;

SerialOscilloscope::SerialOscilloscope(const QString &title,
                                       QSettings *settings,
                                       QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::SerialOscilloscope)
{
    Q_CHECK_PTR(settings);

    m_ui->setupUi(this); setWindowTitle(title); m_settings = settings;
    m_ui->statusBar->hide(); // Default style doesn't look quite right...

    m_ui->math->hide(); m_rescalePlotAxes = true; m_rescaleMathAxes = true;

    m_background = QColor(Qt::white);

    m_plottables = QMap<unsigned int, QCPAbstractPlottable*>();

    m_ui->plot->xAxis->setRange(MIN_X_DEF_RANGE, MAX_X_DEF_RANGE);
    m_ui->plot->xAxis->setNumberFormat("gb");
    m_ui->plot->xAxis->setPadding(2);

    m_ui->plot->yAxis->setRange(MIN_Y_DEF_RANGE, MAX_Y_DEF_RANGE);
    m_ui->plot->yAxis->setNumberFormat("gb");
    m_ui->plot->yAxis->setPadding(2);

    m_ui->math->xAxis->setRange(MIN_X_DEF_RANGE, MAX_X_DEF_RANGE);
    m_ui->math->xAxis->setNumberFormat("gb");
    m_ui->math->xAxis->setPadding(2);

    m_ui->math->yAxis->setRange(MIN_Y_DEF_RANGE, MAX_Y_DEF_RANGE);
    m_ui->math->yAxis->setNumberFormat("gb");
    m_ui->math->yAxis->setPadding(2);

    m_ui->plot->setNoAntialiasingOnDrag(true);
    m_ui->plot->axisRect()->setupFullAxesBox(true);

    m_ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /* |
                                QCP::iMultiSelect | QCP::iSelectPlottables */);

    m_ui->math->setNoAntialiasingOnDrag(true);
    m_ui->math->axisRect()->setupFullAxesBox(true);

    m_ui->math->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom /* |
                                QCP::iMultiSelect | QCP::iSelectPlottables */);

    m_ui->plot->xAxis2->setTicks(false);
    m_ui->plot->xAxis2->setPadding(2);
    m_ui->plot->yAxis2->setTicks(false);
    m_ui->plot->yAxis2->setPadding(2);

    m_ui->math->xAxis2->setTicks(false);
    m_ui->math->xAxis2->setPadding(2);
    m_ui->math->yAxis2->setTicks(false);
    m_ui->math->yAxis2->setPadding(2);

    connect(m_ui->actionZoom_Default, SIGNAL(triggered()),
            this, SLOT(zoomPlotDefault()));

    connect(m_ui->actionZoom_Default, SIGNAL(triggered()),
            this, SLOT(zoomMathDefault()));

    connect(m_ui->actionSave_Bmp, SIGNAL(triggered()),
            this, SLOT(saveBmp()));

    connect(m_ui->actionSave_Jpg, SIGNAL(triggered()),
            this, SLOT(saveJpg()));

    connect(m_ui->actionSave_Png, SIGNAL(triggered()),
            this, SLOT(savePng()));

    connect(m_ui->actionSave_Pdf, SIGNAL(triggered()),
            this, SLOT(savePdf()));

    connect(m_ui->actionClose_Window, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_ui->actionGeneral_Help, SIGNAL(triggered()),
            this, SLOT(generalHelp()));

    connect(m_ui->actionOscilloscope_Help, SIGNAL(triggered()),
            this, SLOT(oscilloscopeHelp()));

    connect(m_ui->plot, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(plotMousePress(QMouseEvent*)));

    connect(m_ui->plot, SIGNAL(mouseWheel(QWheelEvent*)),
            this, SLOT(plotMouseWheel(QWheelEvent*)));

    connect(m_ui->math, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(mathMousePress(QMouseEvent*)));

    connect(m_ui->math, SIGNAL(mouseWheel(QWheelEvent*)),
            this, SLOT(mathMouseWheel(QWheelEvent*)));

    connect(m_ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this, SLOT(zoomPlotDefault(QMouseEvent*)));

    connect(m_ui->math, SIGNAL(mouseDoubleClick(QMouseEvent*)),
            this, SLOT(zoomMathDefault(QMouseEvent*)));

    QSettings i_settings(m_settings->fileName(), m_settings->format());

    i_settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
    i_settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    if(i_settings.contains(SERIAL_OSCILLOSCOPE_KEY_STATE))
    {
        restoreState(i_settings.value(SERIAL_OSCILLOSCOPE_KEY_STATE).
                     toByteArray());
    }

    if(i_settings.contains(SERIAL_OSCILLOSCOPE_KEY_GEOMETRY))
    {
        restoreGeometry(i_settings.value(SERIAL_OSCILLOSCOPE_KEY_GEOMETRY).
                        toByteArray());
    }

    scheduleReplot();
}

SerialOscilloscope::~SerialOscilloscope()
{
    m_replotLock.tryLock(); m_replotLock.unlock(); delete m_ui;
}

void SerialOscilloscope::closeEvent(QCloseEvent *event)
{
    foreach(QObject *child, children())
    {
        QDialog *dialog = qobject_cast<QDialog*>(child);

        if(dialog)
        {
            dialog->reject();
        }
    }

    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
    settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    if(isVisible())
    {
        settings.setValue(SERIAL_OSCILLOSCOPE_KEY_STATE,
                          saveState());
    }

    if(isVisible())
    {
        settings.setValue(SERIAL_OSCILLOSCOPE_KEY_GEOMETRY,
                          saveGeometry());
    }

    QWidget::closeEvent(event);
}

void SerialOscilloscope::newBarChart(unsigned int handle, const QString &name)
{
    if(!m_plottables.contains(handle))
    {
        newPlottable<QCPBars>(handle, name);

        QBrush brush = m_plottables.value(handle)->brush();
        QBrush sbrush = m_plottables.value(handle)->selectedBrush();

        brush.setStyle(Qt::SolidPattern);
        sbrush.setStyle(Qt::SolidPattern);

        m_plottables.value(handle)->setBrush(brush);
        m_plottables.value(handle)->setSelectedBrush(sbrush);

        scheduleReplot(); return;
    }

    QCPBars *bar = verifyHandle<QCPBars>(handle,
    tr("New Bar Chart"));

    if(bar != NULL)
    {
        newPlottable<QCPBars>(handle, name); // Rename plottable

        scheduleReplot();
    }
}

void SerialOscilloscope::newCurve(unsigned int handle, const QString &name)
{
    if(!m_plottables.contains(handle))
    {
        newPlottable<MyQCPCurve>(handle, name);

        MyQCPCurve*curve=qobject_cast<MyQCPCurve*>(m_plottables.value(handle));

        curve->FFTWPlot = new QCPCurve(m_ui->math->xAxis,
                                       m_ui->math->yAxis);

        m_ui->math->addPlottable(curve->FFTWPlot);

        curve->FFTWPlot->setPen(curve->pen());
        curve->FFTWPlot->setSelectedPen(curve->selectedPen());
        curve->FFTWPlot->setBrush(curve->brush());
        curve->FFTWPlot->setSelectedBrush(curve->selectedBrush());

        curve->FFTWPlot->setName(curve->name());
        curve->FFTWPlot->removeFromLegend();

        scheduleReplot(); return;
    }

    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("New Curve Chart"));

    if(curve != NULL)
    {
        newPlottable<MyQCPCurve>(handle, name); // Rename plottable

        curve->FFTWPlot->setName(curve->name());

        if((!curve->FFTWPlot->name().isEmpty())
        && (curve->FFTWOption != MyQCPCurve::none))
        {
            curve->FFTWPlot->addToLegend();
        }
        else
        {
            curve->FFTWPlot->removeFromLegend();
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::newGraph(unsigned int handle, const QString &name)
{
    if(!m_plottables.contains(handle))
    {
        newPlottable<MyQCPGraph>(handle, name);

        MyQCPGraph*graph=qobject_cast<MyQCPGraph*>(m_plottables.value(handle));

        graph->FFTWPlot = new QCPGraph(m_ui->math->xAxis,
                                       m_ui->math->yAxis);

        m_ui->math->addPlottable(graph->FFTWPlot);

        graph->FFTWPlot->setPen(graph->pen());
        graph->FFTWPlot->setSelectedPen(graph->selectedPen());
        graph->FFTWPlot->setBrush(graph->brush());
        graph->FFTWPlot->setSelectedBrush(graph->selectedBrush());

        graph->FFTWPlot->setName(graph->name());
        graph->FFTWPlot->removeFromLegend();

        scheduleReplot(); return;
    }

    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("New Graph Chart"));

    if(graph != NULL)
    {
        newPlottable<MyQCPGraph>(handle, name); // Rename plottable

        graph->FFTWPlot->setName(graph->name());

        if((!graph->FFTWPlot->name().isEmpty())
        && (graph->FFTWOption != MyQCPGraph::none))
        {
            graph->FFTWPlot->addToLegend();
        }
        else
        {
            graph->FFTWPlot->removeFromLegend();
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::newBoxChart(unsigned int handle, const QString &name)
{
    if(!m_plottables.contains(handle))
    {
        newPlottable<QCPStatisticalBox>(handle, name);

        QBrush brush = m_plottables.value(handle)->brush();
        QBrush sbrush = m_plottables.value(handle)->selectedBrush();

        brush.setStyle(Qt::SolidPattern);
        sbrush.setStyle(Qt::SolidPattern);

        m_plottables.value(handle)->setBrush(brush);
        m_plottables.value(handle)->setSelectedBrush(sbrush);

        scheduleReplot(); return;
    }

    QCPStatisticalBox *box = verifyHandle<QCPStatisticalBox>(handle,
    tr("New Box Chart"));

    if(box != NULL)
    {
        newPlottable<QCPStatisticalBox>(handle, name); // Rename plottable

        scheduleReplot();
    }
}

void SerialOscilloscope::setLineColor(unsigned int handle, QRgb rgb)
{
    QCPAbstractPlottable *ap = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Set Line Color"));

    if(ap != NULL)
    {
        QPen pen = ap->pen();
        QPen spen = ap->selectedPen();

        pen.setColor(QColor::fromRgb(rgb));
        spen.setColor(QColor::fromRgb(rgb));

        ap->setPen(pen);
        ap->setSelectedPen(spen);

        MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(ap);

        if(curve)
        {
            curve->FFTWPlot->setPen(pen);
            curve->FFTWPlot->setSelectedPen(spen);
        }

        MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(ap);

        if(graph)
        {
            graph->FFTWPlot->setPen(pen);
            graph->FFTWPlot->setSelectedPen(spen);
        }
    }
}

void SerialOscilloscope::setFillColor(unsigned int handle, QRgb rgb)
{
    QCPAbstractPlottable *ap = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Set Fill Color"));

    if(ap != NULL)
    {
        QBrush brush = ap->brush();
        QBrush sbrush = ap->selectedBrush();

        brush.setColor(QColor::fromRgb(rgb));
        sbrush.setColor(QColor::fromRgb(rgb));

        ap->setBrush(brush);
        ap->setSelectedBrush(sbrush);

        MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(ap);

        if(curve)
        {
            curve->FFTWPlot->setBrush(brush);
            curve->FFTWPlot->setSelectedBrush(sbrush);
        }

        MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(ap);

        if(graph)
        {
            graph->FFTWPlot->setBrush(brush);
            graph->FFTWPlot->setSelectedBrush(sbrush);
        }
    }
}

void SerialOscilloscope::setLineStyle(unsigned int handle, Qt::PenStyle ps)
{
    QCPAbstractPlottable *ap = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Set Line Style"));

    if(ap != NULL)
    {
        switch(ps)
        {
            case Qt::NoPen:
            case Qt::SolidLine:
            case Qt::DashLine:
            case Qt::DotLine:
            case Qt::DashDotLine:
            case Qt::DashDotDotLine:

            {
                QPen pen = ap->pen();
                QPen spen = ap->selectedPen();

                pen.setStyle(ps);
                spen.setStyle(ps);

                ap->setPen(pen);
                ap->setSelectedPen(spen);

                MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(ap);

                if(curve)
                {
                    curve->FFTWPlot->setPen(pen);
                    curve->FFTWPlot->setSelectedPen(spen);
                }

                MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(ap);

                if(graph)
                {
                    graph->FFTWPlot->setPen(pen);
                    graph->FFTWPlot->setSelectedPen(spen);
                }

                scheduleReplot();
                break;
            }

            case Qt::CustomDashLine:

            default:
            error(handle, tr("Set Line Style: Invalid line style"));
        }
    }
}

void SerialOscilloscope::setFillStyle(unsigned int handle, Qt::BrushStyle bs)
{
    QCPAbstractPlottable *ap = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Set Fill Style"));

    if(ap != NULL)
    {
        switch(bs)
        {
            case Qt::NoBrush:
            case Qt::SolidPattern:
            case Qt::HorPattern:
            case Qt::VerPattern:
            case Qt::CrossPattern:
            case Qt::BDiagPattern:
            case Qt::FDiagPattern:
            case Qt::DiagCrossPattern:

            {
                QBrush brush = ap->brush();
                QBrush sbrush = ap->selectedBrush();

                brush.setStyle(bs);
                sbrush.setStyle(bs);

                ap->setBrush(brush);
                ap->setSelectedBrush(sbrush);

                MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(ap);

                if(curve)
                {
                    curve->FFTWPlot->setBrush(brush);
                    curve->FFTWPlot->setSelectedBrush(sbrush);
                }

                MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(ap);

                if(graph)
                {
                    graph->FFTWPlot->setBrush(brush);
                    graph->FFTWPlot->setSelectedBrush(sbrush);
                }

                scheduleReplot();
                break;
            }

            case Qt::Dense1Pattern:
            case Qt::Dense2Pattern:
            case Qt::Dense3Pattern:
            case Qt::Dense4Pattern:
            case Qt::Dense5Pattern:
            case Qt::Dense6Pattern:
            case Qt::Dense7Pattern:
            case Qt::LinearGradientPattern:
            case Qt::RadialGradientPattern:
            case Qt::ConicalGradientPattern:
            case Qt::TexturePattern:

            default:
            error(handle, tr("Set Fill Style: Invalid fill style"));
        }
    }
}

void SerialOscilloscope::setCurveDeltaMode(unsigned int handle, bool enable)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Delta Mode"));

    if(curve != NULL)
    {
        curve->deltaMode = enable;
    }
}

void SerialOscilloscope::setGraphDeltaMode(unsigned int handle, bool enable)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Delta Mode"));

    if(graph != NULL)
    {
        graph->deltaMode = enable;
    }
}

void SerialOscilloscope::setCurveSize(unsigned int handle, unsigned int size)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Size"));

    if(curve != NULL)
    {
        if(MAX_DATA_SIZE < size)
        {
            error(handle, tr("Set Curve Size: The maximum supported "
            "curve size is %Ln data point(s)", "", MAX_DATA_SIZE));
        }
        else
        {
            curve->size = size;

            while(static_cast<int>(curve->size) < curve->data()->size())
            {
                curve->removeData(curve->data()->constBegin().key());
            }

            scheduleReplot();
        }
    }
}

void SerialOscilloscope::setGraphSize(unsigned int handle, unsigned int size)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Size"));

    if(graph != NULL)
    {
        if(MAX_DATA_SIZE < size)
        {
            error(handle, tr("Set Graph Size: The maximum supported "
            "graph size is %Ln data point(s)", "", MAX_DATA_SIZE));
        }
        else
        {
            graph->size = size;

            while(static_cast<int>(graph->size) < graph->data()->size())
            {
                graph->removeData(graph->data()->constBegin().key());
            }

            scheduleReplot();
        }
    }
}

void SerialOscilloscope::setCurveSampleRate(unsigned int handle,
                                            double sampleRate)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Sample Rate"));

    if(curve != NULL)
    {
        if(qIsFinite(sampleRate) && (!qFuzzyCompare(1 + sampleRate, 1 + 0.0)))
        {
            curve->sampleRate = (1 / sampleRate);
        }
        else
        {
            error(handle, tr("Set Curve Sample Rate: Invalid sample rate"));
        }
    }
}

void SerialOscilloscope::setGraphSampleRate(unsigned int handle,
                                            double sampleRate)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Sample Rate"));

    if(graph != NULL)
    {
        if(qIsFinite(sampleRate) && (!qFuzzyCompare(1 + sampleRate, 1 + 0.0)))
        {
            graph->sampleRate = (1 / sampleRate);
        }
        else
        {
            error(handle, tr("Set Graph Sample Rate: Invalid sample rate"));
        }
    }
}

void SerialOscilloscope::setCurveScaler(unsigned int handle, double scaler)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Scaler"));

    if(curve != NULL)
    {
        if(qIsFinite(scaler) && (!qFuzzyCompare(1 + scaler, 1 + 0.0)))
        {
            curve->scaler = (1 / scaler);
        }
        else
        {
            error(handle, tr("Set Curve Scaler: Invalid scaler"));
        }
    }
}

void SerialOscilloscope::setGraphScaler(unsigned int handle, double scaler)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Scaler"));

    if(graph != NULL)
    {
        if(qIsFinite(scaler) && (!qFuzzyCompare(1 + scaler, 1 + 0.0)))
        {
            graph->scaler = (1 / scaler);
        }
        else
        {
            error(handle, tr("Set Graph Scaler: Invalid scaler"));
        }
    }
}

void SerialOscilloscope::addBarData(unsigned int handle,
                                    double key,
                                    double value)
{
    QCPBars *bar = verifyHandle<QCPBars>(handle,
    tr("Add Bar Data"));

    if(bar != NULL)
    {
        if(qIsFinite(key) && qIsFinite(value))
        {
            bar->removeData(key);
            bar->addData(key, value);

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Add Bar Data: Invalid input(s) - "
                             "key=%1, "
                             "value=%2").
                             arg(key).
                             arg(value));
        }
    }
}

void SerialOscilloscope::setBoxData(unsigned int handle,
                                    double key,
                                    double minimum,
                                    double lowerQuartile,
                                    double median,
                                    double upperQuartile,
                                    double maximum)
{
    QCPStatisticalBox *box = verifyHandle<QCPStatisticalBox>(handle,
    tr("Set Box Data"));

    if(box != NULL)
    {
        if(qIsFinite(key) && qIsFinite(minimum)
        && qIsFinite(lowerQuartile) && qIsFinite(median)
        && qIsFinite(upperQuartile) && qIsFinite(maximum))
        {
            box->setKey(key);
            box->setMinimum(minimum);
            box->setLowerQuartile(lowerQuartile);
            box->setMedian(median);
            box->setUpperQuartile(upperQuartile);
            box->setMaximum(maximum);

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Set Box Data: Invalid input(s) - "
                             "key=%1, "
                             "minimum=%2, "
                             "lowerQuartile=%3, "
                             "median=%4, "
                             "upperQuartile=%5, "
                             "maximum=%6").
                             arg(key).
                             arg(minimum).
                             arg(lowerQuartile).
                             arg(median).
                             arg(upperQuartile).
                             arg(maximum));
        }
    }
}

void SerialOscilloscope::addCurveData(unsigned int handle,
                                      double key,
                                      double value)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Add Curve Data"));

    if(curve != NULL)
    {
        if(qIsFinite(key) && qIsFinite(value))
        {
            if(curve->data()->isEmpty())
            {
                curve->addData(0, key * curve->scaler, value * curve->scaler);
            }
            else
            {
                if((curve->deltaMode)
                && (static_cast<int>(curve->size) <= curve->data()->size()))
                {
                    QCPCurveDataMap::iterator i =
                    const_cast<QCPCurveDataMap*>(curve->data())->begin();

                    QCPCurveDataMap::iterator j =
                    const_cast<QCPCurveDataMap*>(curve->data())->end();

                    for(; i != (j - 1); i++) // shift all values left
                    {
                        QCPCurveDataMap::iterator k = i + 1;

                        i.value().key = k.value().key;
                        i.value().value = k.value().value;
                    }

                    i.value().key = key * curve->scaler;
                    i.value().value = value * curve->scaler;
                }
                else
                {
                    double time = (curve->data()->constEnd() - 1).key() +
                    curve->sampleRate;

                    curve->removeData(time);
                    curve->addData(time,
                    key * curve->scaler, value * curve->scaler);
                }
            }

            while(static_cast<int>(curve->size) < curve->data()->size())
            {
                curve->removeData(curve->data()->constBegin().key());
            }

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Add Curve Data: Invalid input(s) - "
                             "key=%1, "
                             "value=%2").
                             arg(key).
                             arg(value));
        }
    }
}

void SerialOscilloscope::addCurveData(unsigned int handle,
                                      double time,
                                      double key,
                                      double value)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Add Curve Data"));

    if(curve != NULL)
    {
        if(qIsFinite(time) && qIsFinite(key) && qIsFinite(value))
        {
            curve->removeData(time);
            curve->addData(time, key * curve->scaler, value * curve->scaler);

            while(static_cast<int>(curve->size) < curve->data()->size())
            {
                curve->removeData(curve->data()->constBegin().key());
            }

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Add Curve Data: Invalid input(s) - "
                             "time=%1, "
                             "key=%2, "
                             "value=%3").
                             arg(time).
                             arg(key).
                             arg(value));
        }
    }
}

void SerialOscilloscope::addGraphData(unsigned int handle,
                                      double value)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Add Graph Data"));

    if(graph != NULL)
    {
        if(qIsFinite(value))
        {
            if(graph->data()->isEmpty())
            {
                graph->addData(0, value * graph->scaler);
            }
            else
            {
                if((graph->deltaMode)
                && (static_cast<int>(graph->size) <= graph->data()->size()))
                {
                    QCPDataMap::iterator i =
                    const_cast<QCPDataMap*>(graph->data())->begin();

                    QCPDataMap::iterator j =
                    const_cast<QCPDataMap*>(graph->data())->end();

                    for(; i != (j - 1); i++) // shift all values left
                    {
                        QCPDataMap::iterator k = i + 1;

                        i.value().value = k.value().value;
                    }

                    i.value().value = value * graph->scaler;
                }
                else
                {
                    double key = (graph->data()->constEnd() - 1).key() +
                    graph->sampleRate;

                    graph->removeData(key);
                    graph->addData(key,
                    value * graph->scaler);
                }
            }

            while(static_cast<int>(graph->size) < graph->data()->size())
            {
                graph->removeData(graph->data()->constBegin().key());
            }

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Add Graph Data: Invalid input(s) - "
                             "value=%1").
                             arg(value));
        }
    }
}

void SerialOscilloscope::addGraphData(unsigned int handle,
                                      double key,
                                      double value)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Add Graph Data"));

    if(graph != NULL)
    {
        if(qIsFinite(key) && qIsFinite(value))
        {
            graph->removeData(key);
            graph->addData(key, value * graph->scaler);

            while(static_cast<int>(graph->size) < graph->data()->size())
            {
                graph->removeData(graph->data()->constBegin().key());
            }

            scheduleReplot();
        }
        else
        {
            error(handle, tr("Add Graph Data: Invalid input(s) - "
                             "key=%1, "
                             "value=%2").
                             arg(key).
                             arg(value));
        }
    }
}

void SerialOscilloscope::setBarAbove(unsigned int handle, unsigned int target)
{
    QCPBars *bar = verifyHandle<QCPBars>(handle,
    tr("Set Bar Above (handle)"));

    if(bar != NULL)
    {
        QCPBars *barTarget = verifyHandle<QCPBars>(target,
        tr("Set Bar Above (target)"));

        if(bar == barTarget)
        {
            bar->moveAbove(NULL);
        }
        else if(barTarget != NULL)
        {
            bar->moveAbove(barTarget);
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::setBarBelow(unsigned int handle, unsigned int target)
{
    QCPBars *bar = verifyHandle<QCPBars>(handle,
    tr("Set Bar Below (handle)"));

    if(bar != NULL)
    {
        QCPBars *barTarget = verifyHandle<QCPBars>(target,
        tr("Set Bar Below (target)"));

        if(bar == barTarget)
        {
            bar->moveBelow(NULL);
        }
        else if(barTarget != NULL)
        {
            bar->moveBelow(barTarget);
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::setCurveDrawingStyle(unsigned int handle,
                                              QCPCurve::LineStyle ls)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Line Style"));

    if(curve != NULL)
    {
        switch(ls)
        {
            case QCPCurve::lsNone:
            case QCPCurve::lsLine:

            curve->setLineStyle(ls);
            curve->FFTWPlot->setLineStyle(ls);
            scheduleReplot();
            break;

            default:
            error(handle, tr("Set Curve Line Style: Invalid line style"));
        }
    }
}

void SerialOscilloscope::setCurveScatterStyle(unsigned int handle,
                                              QCPScatterStyle::ScatterShape ss)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Scatter Style"));

    if(curve != NULL)
    {
        switch(ss)
        {
            case QCPScatterStyle::ssNone:
            case QCPScatterStyle::ssDot:
            case QCPScatterStyle::ssCross:
            case QCPScatterStyle::ssPlus:
            case QCPScatterStyle::ssCircle:
            case QCPScatterStyle::ssDisc:
            case QCPScatterStyle::ssSquare:
            case QCPScatterStyle::ssDiamond:
            case QCPScatterStyle::ssStar:
            case QCPScatterStyle::ssTriangle:
            case QCPScatterStyle::ssTriangleInverted:
            case QCPScatterStyle::ssCrossSquare:
            case QCPScatterStyle::ssPlusSquare:
            case QCPScatterStyle::ssCrossCircle:
            case QCPScatterStyle::ssPlusCircle:
            case QCPScatterStyle::ssPeace:

            curve->setScatterStyle(ss);
            curve->FFTWPlot->setScatterStyle(ss);
            scheduleReplot();
            break;

            case QCPScatterStyle::ssPixmap:
            case QCPScatterStyle::ssCustom:

            default:
            error(handle,
                  tr("Set Curve Scatter Style: Invalid scatter style"));
        }
    }
}

void SerialOscilloscope::setCurveFillCurve(unsigned int handle,
                                           unsigned int target)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve Fill Curve (handle)"));

    if(curve != NULL)
    {
        MyQCPCurve *curveTarget = verifyHandle<MyQCPCurve>(target,
        tr("Set Curve Fill Curve (target)"));

        if(curve == curveTarget)
        {
            /* Doesn't exist yet */

            // curve->setChannelFillCurve(NULL);
            // curve->FFTWPlot->setChannelFillCurve(NULL);
        }
        else if(curveTarget != NULL)
        {
            /* Doesn't exist yet */

            // curve->setChannelFillCurve(curveTarget);
            // cruve->FFTWPlot->setChannelFillCurve(curveTarget->FFTWPlot);
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::setCurveFFT(unsigned int handle,
                                     MyQCPCurve::QCPFft option)
{
    MyQCPCurve *curve = verifyHandle<MyQCPCurve>(handle,
    tr("Set Curve FFT"));

    if(curve != NULL)
    {
        if(m_fftw_enabled)
        {
            switch(option)
            {
                case MyQCPCurve::none:
                case MyQCPCurve::forward:
                case MyQCPCurve::backward:

                curve->FFTWOption = option;

                if((!curve->FFTWPlot->name().isEmpty())
                && (curve->FFTWOption != MyQCPCurve::none))
                {
                    curve->FFTWPlot->addToLegend();
                }
                else
                {
                    curve->FFTWPlot->removeFromLegend();
                }

                if(curve->FFTWOption == MyQCPCurve::none)
                {
                    curve->FFTWPlot->clearData();
                }

                scheduleReplot();
                break;

                default:
                error(handle, tr("Set Curve FFT: Invalid FFT option"));
            }
        }
        else
        {
            error(handle, tr("Set Curve FFT: FFT support disabled"));
        }
    }
}

void SerialOscilloscope::setGraphDrawingStyle(unsigned int handle,
                                              QCPGraph::LineStyle ls)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Line Style"));

    if(graph != NULL)
    {
        switch(ls)
        {
            case QCPGraph::lsNone:
            case QCPGraph::lsLine:
            case QCPGraph::lsStepLeft:
            case QCPGraph::lsStepRight:
            case QCPGraph::lsStepCenter:
            case QCPGraph::lsImpulse:

            graph->setLineStyle(ls);
            graph->FFTWPlot->setLineStyle(ls);
            scheduleReplot();
            break;

            default:
            error(handle, tr("Set Graph Line Style: Invalid line style"));
        }
    }
}

void SerialOscilloscope::setGraphScatterStyle(unsigned int handle,
                                              QCPScatterStyle::ScatterShape ss)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Scatter Style"));

    if(graph != NULL)
    {
        switch(ss)
        {
            case QCPScatterStyle::ssNone:
            case QCPScatterStyle::ssDot:
            case QCPScatterStyle::ssCross:
            case QCPScatterStyle::ssPlus:
            case QCPScatterStyle::ssCircle:
            case QCPScatterStyle::ssDisc:
            case QCPScatterStyle::ssSquare:
            case QCPScatterStyle::ssDiamond:
            case QCPScatterStyle::ssStar:
            case QCPScatterStyle::ssTriangle:
            case QCPScatterStyle::ssTriangleInverted:
            case QCPScatterStyle::ssCrossSquare:
            case QCPScatterStyle::ssPlusSquare:
            case QCPScatterStyle::ssCrossCircle:
            case QCPScatterStyle::ssPlusCircle:
            case QCPScatterStyle::ssPeace:

            graph->setScatterStyle(ss);
            graph->FFTWPlot->setScatterStyle(ss);
            scheduleReplot();
            break;

            case QCPScatterStyle::ssPixmap:
            case QCPScatterStyle::ssCustom:

            default:
            error(handle,
                  tr("Set Graph Scatter Style: Invalid scatter style"));
        }
    }
}

void SerialOscilloscope::setGraphFillGraph(unsigned int handle,
                                           unsigned int target)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph Fill Graph (handle)"));

    if(graph != NULL)
    {
        MyQCPGraph *graphTarget = verifyHandle<MyQCPGraph>(target,
        tr("Set Graph Fill Graph (target)"));

        if(graph == graphTarget)
        {
            graph->setChannelFillGraph(NULL);
            graph->FFTWPlot->setChannelFillGraph(NULL);
        }
        else if(graphTarget != NULL)
        {
            graph->setChannelFillGraph(graphTarget);
            graph->FFTWPlot->setChannelFillGraph(graphTarget->FFTWPlot);
        }

        scheduleReplot();
    }
}

void SerialOscilloscope::setGraphFFT(unsigned int handle,
                                     MyQCPGraph::QCPFft option)
{
    MyQCPGraph *graph = verifyHandle<MyQCPGraph>(handle,
    tr("Set Graph FFT"));

    if(graph != NULL)
    {
        if(m_fftw_enabled)
        {
            switch(option)
            {
                case MyQCPGraph::none:
                case MyQCPGraph::magnitude:
                case MyQCPGraph::magnitude2:
                case MyQCPGraph::magnitude10:
                case MyQCPGraph::magnitude20:
                case MyQCPGraph::phaseRadians:
                case MyQCPGraph::phaseDegrees:

                graph->FFTWOption = option;

                if((!graph->FFTWPlot->name().isEmpty())
                && (graph->FFTWOption != MyQCPGraph::none))
                {
                    graph->FFTWPlot->addToLegend();
                }
                else
                {
                    graph->FFTWPlot->removeFromLegend();
                }

                if(graph->FFTWOption == MyQCPGraph::none)
                {
                    graph->FFTWPlot->clearData();
                }

                scheduleReplot();
                break;

                default:
                error(handle, tr("Set Graph FFT: Invalid FFT option"));
            }
        }
        else
        {
            error(handle, tr("Set Graph FFT: FFT support disabled"));
        }
    }
}

void SerialOscilloscope::clearData(unsigned int handle)
{
    QCPAbstractPlottable *plot = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Clear Data"));

    if(plot != NULL)
    {
        plot->clearData();

        scheduleReplot();
    }
}

void SerialOscilloscope::clearPlot(unsigned int handle)
{
    QCPAbstractPlottable *plot = verifyHandle<QCPAbstractPlottable>(handle,
    tr("Clear Plot"));

    if(plot != NULL)
    {
        QCPBars *bars = qobject_cast<QCPBars*>(plot);

        if(bars)
        {
            foreach(QCPAbstractPlottable *plottable, m_plottables.values())
            {
                QCPBars *p = qobject_cast<QCPBars*>(plottable);

                if(p && (p->barAbove() == bars))
                {
                    p->moveAbove(NULL);
                }

                if(p && (p->barBelow() == bars))
                {
                    p->moveBelow(NULL);
                }
            }
        }

        MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(plot);

        if(curve)
        {
            /*

            foreach(QCPAbstractPlottable *plottable, plottables.values())
            {
                MyQCPCurve *p = qobject_cast<MyQCPCurve*>(plottable);

                if(p && (p->channelFillCurve() == curve))
                {
                    p->setChannelFillCurve(NULL);
                }

                if(p && (p->FFTWPlot->channelFillCurve() == curve->FFTWPlot))
                {
                    p->FFTWPlot->setChannelFillCurve(NULL);
                }
            }

            */

            m_ui->math->removePlottable(curve->FFTWPlot);
        }

        MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(plot);

        if(graph)
        {
            foreach(QCPAbstractPlottable *plottable, m_plottables.values())
            {
                MyQCPGraph *p = qobject_cast<MyQCPGraph*>(plottable);

                if(p && (p->channelFillGraph() == graph))
                {
                    p->setChannelFillGraph(NULL);
                }

                if(p && (p->FFTWPlot->channelFillGraph() == graph->FFTWPlot))
                {
                    p->FFTWPlot->setChannelFillGraph(NULL);
                }
            }

            m_ui->math->removePlottable(graph->FFTWPlot);
        }

        m_ui->plot->removePlottable(plot); m_plottables.remove(handle);

        scheduleReplot();
    }
}

void SerialOscilloscope::clearAllDatum()
{
    foreach(unsigned int key, m_plottables.keys())
    {
        clearData(key);
    }
}

void SerialOscilloscope::clearAllPlots()
{
    foreach(unsigned int key, m_plottables.keys())
    {
        clearPlot(key);
    }
}

void SerialOscilloscope::setXAxisScaleType(QCPAxis::ScaleType st)
{
    switch(st)
    {
        case QCPAxis::stLinear:
        case QCPAxis::stLogarithmic:

        m_ui->plot->xAxis->setScaleType(st);
        m_ui->plot->xAxis2->setScaleType(st);

        if(st == QCPAxis::stLinear)
        {
            m_ui->plot->xAxis->setNumberPrecision(6);
            m_ui->plot->xAxis2->setNumberPrecision(6);
        }

        if(st == QCPAxis::stLogarithmic)
        {
            m_ui->plot->xAxis->setNumberPrecision(0);
            m_ui->plot->xAxis2->setNumberPrecision(0);
        }

        scheduleReplot();
        break;

        default:
        emit error("[" + windowTitle() + "]::" +
                   tr("Set X Axis Scale Type: Invalid scale type"));
    }
}

void SerialOscilloscope::setXAxisScaleLogBase(double base)
{
    if(qIsFinite(base) && (base > 1))
    {
        m_ui->plot->xAxis->setScaleLogBase(base);
        m_ui->plot->xAxis2->setScaleLogBase(base);

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set X Axis Scale Log Base: Invalid base"));
    }
}

void SerialOscilloscope::setXAxisRangeReversed(bool reversed)
{
    m_ui->plot->xAxis->setRangeReversed(reversed);
    m_ui->plot->xAxis2->setRangeReversed(reversed);

    scheduleReplot();
}

void SerialOscilloscope::setXAxisLabel(const QString &name)
{
    if(name.isNull())
    {
        m_ui->plot->xAxis->setLabel("X");
    }
    else
    {
        m_ui->plot->xAxis->setLabel(name);
    }

    scheduleReplot();
}

void SerialOscilloscope::setYAxisScaleType(QCPAxis::ScaleType st)
{
    switch(st)
    {
        case QCPAxis::stLinear:
        case QCPAxis::stLogarithmic:

        m_ui->plot->yAxis->setScaleType(st);
        m_ui->plot->yAxis2->setScaleType(st);

        if(st == QCPAxis::stLinear)
        {
            m_ui->plot->yAxis->setNumberPrecision(6);
            m_ui->plot->yAxis2->setNumberPrecision(6);
        }

        if(st == QCPAxis::stLogarithmic)
        {
            m_ui->plot->yAxis->setNumberPrecision(0);
            m_ui->plot->yAxis2->setNumberPrecision(0);
        }

        scheduleReplot();
        break;

        default:
        emit error("[" + windowTitle() + "]::" +
                   tr("Set Y Axis Scale Type: Invalid scale type"));
    }
}

void SerialOscilloscope::setYAxisScaleLogBase(double base)
{
    if(qIsFinite(base) && (base > 1))
    {
        m_ui->plot->yAxis->setScaleLogBase(base);
        m_ui->plot->yAxis2->setScaleLogBase(base);

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set Y Axis Scale Log Base: Invalid base"));
    }
}

void SerialOscilloscope::setYAxisRangeReversed(bool reversed)
{
    m_ui->plot->yAxis->setRangeReversed(reversed);
    m_ui->plot->yAxis2->setRangeReversed(reversed);

    scheduleReplot();
}

void SerialOscilloscope::setYAxisLabel(const QString &name)
{
    if(name.isNull())
    {
        m_ui->plot->yAxis->setLabel("Y");
    }
    else
    {
        m_ui->plot->yAxis->setLabel(name);
    }

    scheduleReplot();
}

void SerialOscilloscope::setFFTXAxisScaleType(QCPAxis::ScaleType st)
{
    if(m_fftw_enabled)
    {
        switch(st)
        {
            case QCPAxis::stLinear:
            case QCPAxis::stLogarithmic:

            m_ui->math->xAxis->setScaleType(st);
            m_ui->math->xAxis2->setScaleType(st);

            if(st == QCPAxis::stLinear)
            {
                m_ui->math->xAxis->setNumberPrecision(6);
                m_ui->math->xAxis2->setNumberPrecision(6);
            }

            if(st == QCPAxis::stLogarithmic)
            {
                m_ui->math->xAxis->setNumberPrecision(0);
                m_ui->math->xAxis2->setNumberPrecision(0);
            }

            scheduleReplot();
            break;

            default:
            emit error("[" + windowTitle() + "]::" +
                       tr("Set FFT X Axis Scale Type: Invalid scale type"));
        }
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT X Axis Scale Type: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTXAxisScaleLogBase(double base)
{
    if(m_fftw_enabled)
    {
        if(qIsFinite(base) && (base > 1))
        {
            m_ui->math->xAxis->setScaleLogBase(base);
            m_ui->math->xAxis2->setScaleLogBase(base);

            scheduleReplot();
        }
        else
        {
            emit error("[" + windowTitle() + "]::" +
                       tr("Set FFT X Axis Scale Log Base: Invalid base"));
        }
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT X Axis Scale Log Base: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTXAxisRangeReversed(bool reversed)
{
    if(m_fftw_enabled)
    {
        m_ui->math->xAxis->setRangeReversed(reversed);
        m_ui->math->xAxis2->setRangeReversed(reversed);

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT X Axis Range Reversed: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTXAxisLabel(const QString &name)
{
    if(m_fftw_enabled)
    {
        if(name.isNull())
        {
            m_ui->math->xAxis->setLabel("X");
        }
        else
        {
            m_ui->math->xAxis->setLabel(name);
        }

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT X Axis Label: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTYAxisScaleType(QCPAxis::ScaleType st)
{
    if(m_fftw_enabled)
    {
        switch(st)
        {
            case QCPAxis::stLinear:
            case QCPAxis::stLogarithmic:

            m_ui->math->yAxis->setScaleType(st);
            m_ui->math->yAxis2->setScaleType(st);

            if(st == QCPAxis::stLinear)
            {
                m_ui->math->yAxis->setNumberPrecision(6);
                m_ui->math->yAxis2->setNumberPrecision(6);
            }

            if(st == QCPAxis::stLogarithmic)
            {
                m_ui->math->yAxis->setNumberPrecision(0);
                m_ui->math->yAxis2->setNumberPrecision(0);
            }

            scheduleReplot();
            break;

            default:
            emit error("[" + windowTitle() + "]::" +
                       tr("Set FFT Y Axis Scale Type: Invalid scale type"));
        }
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT Y Axis Scale Type: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTYAxisScaleLogBase(double base)
{
    if(m_fftw_enabled)
    {
        if(qIsFinite(base) && (base > 1))
        {
            m_ui->math->yAxis->setScaleLogBase(base);
            m_ui->math->yAxis2->setScaleLogBase(base);

            scheduleReplot();
        }
        else
        {
            emit error("[" + windowTitle() + "]::" +
                       tr("Set FFT Y Axis Scale Log Base: Invalid base"));
        }
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT Y Axis Scale Log Base: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTYAxisRangeReversed(bool reversed)
{
    if(m_fftw_enabled)
    {
        m_ui->math->yAxis->setRangeReversed(reversed);
        m_ui->math->yAxis2->setRangeReversed(reversed);

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT Y Axis Range Reversed: FFT support disabled"));
    }
}

void SerialOscilloscope::setFFTYAxisLabel(const QString &name)
{
    if(m_fftw_enabled)
    {
        if(name.isNull())
        {
            m_ui->math->yAxis->setLabel("Y");
        }
        else
        {
            m_ui->math->yAxis->setLabel(name);
        }

        scheduleReplot();
    }
    else
    {
        emit error("[" + windowTitle() + "]::" +
                   tr("Set FFT Y Axis Label: FFT support disabled"));
    }
}

void SerialOscilloscope::setPlotBackgroundColor(QRgb bc)
{
    // Plot Colors
    {
        m_ui->plot->setBackground(QBrush(m_background = QColor::fromRgb(bc)));

        // Axis Base Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->basePen();
            QPen y1Pen = m_ui->plot->yAxis->basePen();
            QPen x2Pen = m_ui->plot->xAxis2->basePen();
            QPen y2Pen = m_ui->plot->yAxis2->basePen();

            QPen sx1Pen = m_ui->plot->xAxis->selectedBasePen();
            QPen sy1Pen = m_ui->plot->yAxis->selectedBasePen();
            QPen sx2Pen = m_ui->plot->xAxis2->selectedBasePen();
            QPen sy2Pen = m_ui->plot->yAxis2->selectedBasePen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->plot->xAxis->setBasePen(x1Pen);
            m_ui->plot->yAxis->setBasePen(y1Pen);
            m_ui->plot->xAxis2->setBasePen(x2Pen);
            m_ui->plot->yAxis2->setBasePen(y2Pen);

            m_ui->plot->xAxis->setSelectedBasePen(sx1Pen);
            m_ui->plot->yAxis->setSelectedBasePen(sy1Pen);
            m_ui->plot->xAxis2->setSelectedBasePen(sx2Pen);
            m_ui->plot->yAxis2->setSelectedBasePen(sy2Pen);
        }

        // Axis Tick Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->tickPen();
            QPen y1Pen = m_ui->plot->yAxis->tickPen();
            QPen x2Pen = m_ui->plot->xAxis2->tickPen();
            QPen y2Pen = m_ui->plot->yAxis2->tickPen();

            QPen sx1Pen = m_ui->plot->xAxis->selectedTickPen();
            QPen sy1Pen = m_ui->plot->yAxis->selectedTickPen();
            QPen sx2Pen = m_ui->plot->xAxis2->selectedTickPen();
            QPen sy2Pen = m_ui->plot->yAxis2->selectedTickPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->plot->xAxis->setTickPen(x1Pen);
            m_ui->plot->yAxis->setTickPen(y1Pen);
            m_ui->plot->xAxis2->setTickPen(x2Pen);
            m_ui->plot->yAxis2->setTickPen(y2Pen);

            m_ui->plot->xAxis->setSelectedTickPen(sx1Pen);
            m_ui->plot->yAxis->setSelectedTickPen(sy1Pen);
            m_ui->plot->xAxis2->setSelectedTickPen(sx2Pen);
            m_ui->plot->yAxis2->setSelectedTickPen(sy2Pen);
        }

        // Axis Sub Tick Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->subTickPen();
            QPen y1Pen = m_ui->plot->yAxis->subTickPen();
            QPen x2Pen = m_ui->plot->xAxis2->subTickPen();
            QPen y2Pen = m_ui->plot->yAxis2->subTickPen();

            QPen sx1Pen = m_ui->plot->xAxis->selectedSubTickPen();
            QPen sy1Pen = m_ui->plot->yAxis->selectedSubTickPen();
            QPen sx2Pen = m_ui->plot->xAxis2->selectedSubTickPen();
            QPen sy2Pen = m_ui->plot->yAxis2->selectedSubTickPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->plot->xAxis->setSubTickPen(x1Pen);
            m_ui->plot->yAxis->setSubTickPen(y1Pen);
            m_ui->plot->xAxis2->setSubTickPen(x2Pen);
            m_ui->plot->yAxis2->setSubTickPen(y2Pen);

            m_ui->plot->xAxis->setSelectedSubTickPen(sx1Pen);
            m_ui->plot->yAxis->setSelectedSubTickPen(sy1Pen);
            m_ui->plot->xAxis2->setSelectedSubTickPen(sx2Pen);
            m_ui->plot->yAxis2->setSelectedSubTickPen(sy2Pen);
        }

        // Axis Grid Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->grid()->pen();
            QPen y1Pen = m_ui->plot->yAxis->grid()->pen();
            QPen x2Pen = m_ui->plot->xAxis2->grid()->pen();
            QPen y2Pen = m_ui->plot->yAxis2->grid()->pen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->plot->xAxis->grid()->setPen(x1Pen);
            m_ui->plot->yAxis->grid()->setPen(y1Pen);
            m_ui->plot->xAxis2->grid()->setPen(x2Pen);
            m_ui->plot->yAxis2->grid()->setPen(y2Pen);
        }

        // Axis Sub Grid Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->grid()->subGridPen();
            QPen y1Pen = m_ui->plot->yAxis->grid()->subGridPen();
            QPen x2Pen = m_ui->plot->xAxis2->grid()->subGridPen();
            QPen y2Pen = m_ui->plot->yAxis2->grid()->subGridPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->plot->xAxis->grid()->setSubGridPen(x1Pen);
            m_ui->plot->yAxis->grid()->setSubGridPen(y1Pen);
            m_ui->plot->xAxis2->grid()->setSubGridPen(x2Pen);
            m_ui->plot->yAxis2->grid()->setSubGridPen(y2Pen);
        }

        // Axis Zero Line Pen
        {
            QPen x1Pen = m_ui->plot->xAxis->grid()->zeroLinePen();
            QPen y1Pen = m_ui->plot->yAxis->grid()->zeroLinePen();
            QPen x2Pen = m_ui->plot->xAxis2->grid()->zeroLinePen();
            QPen y2Pen = m_ui->plot->yAxis2->grid()->zeroLinePen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->plot->xAxis->grid()->setZeroLinePen(x1Pen);
            m_ui->plot->yAxis->grid()->setZeroLinePen(y1Pen);
            m_ui->plot->xAxis2->grid()->setZeroLinePen(x2Pen);
            m_ui->plot->yAxis2->grid()->setZeroLinePen(y2Pen);
        }

        // Axis Label Color

        if(QColor::fromRgb(bc).lightness() > 150)
        {
            m_ui->plot->xAxis->setLabelColor(Qt::black);
            m_ui->plot->yAxis->setLabelColor(Qt::black);
            m_ui->plot->xAxis2->setLabelColor(Qt::black);
            m_ui->plot->yAxis2->setLabelColor(Qt::black);

            m_ui->plot->xAxis->setTickLabelColor(Qt::black);
            m_ui->plot->yAxis->setTickLabelColor(Qt::black);
            m_ui->plot->xAxis2->setTickLabelColor(Qt::black);
            m_ui->plot->yAxis2->setTickLabelColor(Qt::black);

            m_ui->plot->xAxis->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->plot->yAxis->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->plot->xAxis2->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->plot->yAxis2->setSelectedLabelColor(QColor(000, 000, 255));

            m_ui->plot->xAxis->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->plot->yAxis->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->plot->xAxis2->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->plot->yAxis2->setSelectedTickLabelColor(QColor(000,000,255));
        }
        else
        {
            m_ui->plot->xAxis->setLabelColor(Qt::white);
            m_ui->plot->yAxis->setLabelColor(Qt::white);
            m_ui->plot->xAxis2->setLabelColor(Qt::white);
            m_ui->plot->yAxis2->setLabelColor(Qt::white);

            m_ui->plot->xAxis->setTickLabelColor(Qt::white);
            m_ui->plot->yAxis->setTickLabelColor(Qt::white);
            m_ui->plot->xAxis2->setTickLabelColor(Qt::white);
            m_ui->plot->yAxis2->setTickLabelColor(Qt::white);

            m_ui->plot->xAxis->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->plot->yAxis->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->plot->xAxis2->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->plot->yAxis2->setSelectedLabelColor(QColor(200, 200, 255));

            m_ui->plot->xAxis->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->plot->yAxis->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->plot->xAxis2->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->plot->yAxis2->setSelectedTickLabelColor(QColor(200,200,255));
        }

        // Legend Brush
        {
            QBrush brush = m_ui->plot->legend->brush();
            QBrush sbrush = m_ui->plot->legend->selectedBrush();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                brush.setColor(QColor::fromRgb(bc)); // Qt::black);
                sbrush.setColor(QColor::fromRgb(bc)); // Qt::black);
            }
            else
            {
                brush.setColor(QColor::fromRgb(bc)); // Qt::white);
                sbrush.setColor(QColor::fromRgb(bc)); // Qt::white);
            }

            m_ui->plot->legend->setBrush(brush);
            m_ui->plot->legend->setSelectedBrush(sbrush);
        }

        // Legend Border Pen
        {
            QPen pen = m_ui->plot->legend->borderPen();
            QPen spen = m_ui->plot->legend->selectedBorderPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                pen.setColor(Qt::black);
                spen.setColor(QColor(000, 000, 255));
            }
            else
            {
                pen.setColor(Qt::white);
                spen.setColor(QColor(200, 200, 255));
            }

            m_ui->plot->legend->setBorderPen(pen);
            m_ui->plot->legend->setSelectedBorderPen(spen);
        }

        // Legend Icon Border Pen
        {
            QPen pen = m_ui->plot->legend->iconBorderPen();
            QPen spen = m_ui->plot->legend->selectedIconBorderPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                pen.setColor(QColor::fromRgb(bc)); // Qt::black);
                spen.setColor(QColor(000, 000, 255));
            }
            else
            {
                pen.setColor(QColor::fromRgb(bc)); // Qt::white);
                spen.setColor(QColor(200, 200, 255));
            }

            m_ui->plot->legend->setIconBorderPen(pen);
            m_ui->plot->legend->setSelectedIconBorderPen(spen);
        }

        // Legend Text Color

        if(QColor::fromRgb(bc).lightness() > 150)
        {
            m_ui->plot->legend->setTextColor(Qt::black);
            m_ui->plot->legend->setSelectedTextColor(QColor(000, 000, 255));
        }
        else
        {
            m_ui->plot->legend->setTextColor(Qt::white);
            m_ui->plot->legend->setSelectedTextColor(QColor(200, 200, 255));
        }
    }

    // Math Colors
    {
        m_ui->math->setBackground(QBrush(m_background = QColor::fromRgb(bc)));

        // Axis Base Pen
        {
            QPen x1Pen = m_ui->math->xAxis->basePen();
            QPen y1Pen = m_ui->math->yAxis->basePen();
            QPen x2Pen = m_ui->math->xAxis2->basePen();
            QPen y2Pen = m_ui->math->yAxis2->basePen();

            QPen sx1Pen = m_ui->math->xAxis->selectedBasePen();
            QPen sy1Pen = m_ui->math->yAxis->selectedBasePen();
            QPen sx2Pen = m_ui->math->xAxis2->selectedBasePen();
            QPen sy2Pen = m_ui->math->yAxis2->selectedBasePen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->math->xAxis->setBasePen(x1Pen);
            m_ui->math->yAxis->setBasePen(y1Pen);
            m_ui->math->xAxis2->setBasePen(x2Pen);
            m_ui->math->yAxis2->setBasePen(y2Pen);

            m_ui->math->xAxis->setSelectedBasePen(sx1Pen);
            m_ui->math->yAxis->setSelectedBasePen(sy1Pen);
            m_ui->math->xAxis2->setSelectedBasePen(sx2Pen);
            m_ui->math->yAxis2->setSelectedBasePen(sy2Pen);
        }

        // Axis Tick Pen
        {
            QPen x1Pen = m_ui->math->xAxis->tickPen();
            QPen y1Pen = m_ui->math->yAxis->tickPen();
            QPen x2Pen = m_ui->math->xAxis2->tickPen();
            QPen y2Pen = m_ui->math->yAxis2->tickPen();

            QPen sx1Pen = m_ui->math->xAxis->selectedTickPen();
            QPen sy1Pen = m_ui->math->yAxis->selectedTickPen();
            QPen sx2Pen = m_ui->math->xAxis2->selectedTickPen();
            QPen sy2Pen = m_ui->math->yAxis2->selectedTickPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->math->xAxis->setTickPen(x1Pen);
            m_ui->math->yAxis->setTickPen(y1Pen);
            m_ui->math->xAxis2->setTickPen(x2Pen);
            m_ui->math->yAxis2->setTickPen(y2Pen);

            m_ui->math->xAxis->setSelectedTickPen(sx1Pen);
            m_ui->math->yAxis->setSelectedTickPen(sy1Pen);
            m_ui->math->xAxis2->setSelectedTickPen(sx2Pen);
            m_ui->math->yAxis2->setSelectedTickPen(sy2Pen);
        }

        // Axis Sub Tick Pen
        {
            QPen x1Pen = m_ui->math->xAxis->subTickPen();
            QPen y1Pen = m_ui->math->yAxis->subTickPen();
            QPen x2Pen = m_ui->math->xAxis2->subTickPen();
            QPen y2Pen = m_ui->math->yAxis2->subTickPen();

            QPen sx1Pen = m_ui->math->xAxis->selectedSubTickPen();
            QPen sy1Pen = m_ui->math->yAxis->selectedSubTickPen();
            QPen sx2Pen = m_ui->math->xAxis2->selectedSubTickPen();
            QPen sy2Pen = m_ui->math->yAxis2->selectedSubTickPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(Qt::black);
                y1Pen.setColor(Qt::black);
                x2Pen.setColor(Qt::black);
                y2Pen.setColor(Qt::black);

                sx1Pen.setColor(QColor(000, 000, 255));
                sy1Pen.setColor(QColor(000, 000, 255));
                sx2Pen.setColor(QColor(000, 000, 255));
                sy2Pen.setColor(QColor(000, 000, 255));
            }
            else
            {
                x1Pen.setColor(Qt::white);
                y1Pen.setColor(Qt::white);
                x2Pen.setColor(Qt::white);
                y2Pen.setColor(Qt::white);

                sx1Pen.setColor(QColor(200, 200, 255));
                sy1Pen.setColor(QColor(200, 200, 255));
                sx2Pen.setColor(QColor(200, 200, 255));
                sy2Pen.setColor(QColor(200, 200, 255));
            }

            m_ui->math->xAxis->setSubTickPen(x1Pen);
            m_ui->math->yAxis->setSubTickPen(y1Pen);
            m_ui->math->xAxis2->setSubTickPen(x2Pen);
            m_ui->math->yAxis2->setSubTickPen(y2Pen);

            m_ui->math->xAxis->setSelectedSubTickPen(sx1Pen);
            m_ui->math->yAxis->setSelectedSubTickPen(sy1Pen);
            m_ui->math->xAxis2->setSelectedSubTickPen(sx2Pen);
            m_ui->math->yAxis2->setSelectedSubTickPen(sy2Pen);
        }

        // Axis Grid Pen
        {
            QPen x1Pen = m_ui->math->xAxis->grid()->pen();
            QPen y1Pen = m_ui->math->yAxis->grid()->pen();
            QPen x2Pen = m_ui->math->xAxis2->grid()->pen();
            QPen y2Pen = m_ui->math->yAxis2->grid()->pen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->math->xAxis->grid()->setPen(x1Pen);
            m_ui->math->yAxis->grid()->setPen(y1Pen);
            m_ui->math->xAxis2->grid()->setPen(x2Pen);
            m_ui->math->yAxis2->grid()->setPen(y2Pen);
        }

        // Axis Sub Grid Pen
        {
            QPen x1Pen = m_ui->math->xAxis->grid()->subGridPen();
            QPen y1Pen = m_ui->math->yAxis->grid()->subGridPen();
            QPen x2Pen = m_ui->math->xAxis2->grid()->subGridPen();
            QPen y2Pen = m_ui->math->yAxis2->grid()->subGridPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->math->xAxis->grid()->setSubGridPen(x1Pen);
            m_ui->math->yAxis->grid()->setSubGridPen(y1Pen);
            m_ui->math->xAxis2->grid()->setSubGridPen(x2Pen);
            m_ui->math->yAxis2->grid()->setSubGridPen(y2Pen);
        }

        // Axis Zero Line Pen
        {
            QPen x1Pen = m_ui->math->xAxis->grid()->zeroLinePen();
            QPen y1Pen = m_ui->math->yAxis->grid()->zeroLinePen();
            QPen x2Pen = m_ui->math->xAxis2->grid()->zeroLinePen();
            QPen y2Pen = m_ui->math->yAxis2->grid()->zeroLinePen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                x1Pen.setColor(QColor(Qt::black).lighter());
                y1Pen.setColor(QColor(Qt::black).lighter());
                x2Pen.setColor(QColor(Qt::black).lighter());
                y2Pen.setColor(QColor(Qt::black).lighter());
            }
            else
            {
                x1Pen.setColor(QColor(Qt::white).darker());
                y1Pen.setColor(QColor(Qt::white).darker());
                x2Pen.setColor(QColor(Qt::white).darker());
                y2Pen.setColor(QColor(Qt::white).darker());
            }

            m_ui->math->xAxis->grid()->setZeroLinePen(x1Pen);
            m_ui->math->yAxis->grid()->setZeroLinePen(y1Pen);
            m_ui->math->xAxis2->grid()->setZeroLinePen(x2Pen);
            m_ui->math->yAxis2->grid()->setZeroLinePen(y2Pen);
        }

        // Axis Label Color

        if(QColor::fromRgb(bc).lightness() > 150)
        {
            m_ui->math->xAxis->setLabelColor(Qt::black);
            m_ui->math->yAxis->setLabelColor(Qt::black);
            m_ui->math->xAxis2->setLabelColor(Qt::black);
            m_ui->math->yAxis2->setLabelColor(Qt::black);

            m_ui->math->xAxis->setTickLabelColor(Qt::black);
            m_ui->math->yAxis->setTickLabelColor(Qt::black);
            m_ui->math->xAxis2->setTickLabelColor(Qt::black);
            m_ui->math->yAxis2->setTickLabelColor(Qt::black);

            m_ui->math->xAxis->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->math->yAxis->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->math->xAxis2->setSelectedLabelColor(QColor(000, 000, 255));
            m_ui->math->yAxis2->setSelectedLabelColor(QColor(000, 000, 255));

            m_ui->math->xAxis->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->math->yAxis->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->math->xAxis2->setSelectedTickLabelColor(QColor(000,000,255));
            m_ui->math->yAxis2->setSelectedTickLabelColor(QColor(000,000,255));
        }
        else
        {
            m_ui->math->xAxis->setLabelColor(Qt::white);
            m_ui->math->yAxis->setLabelColor(Qt::white);
            m_ui->math->xAxis2->setLabelColor(Qt::white);
            m_ui->math->yAxis2->setLabelColor(Qt::white);

            m_ui->math->xAxis->setTickLabelColor(Qt::white);
            m_ui->math->yAxis->setTickLabelColor(Qt::white);
            m_ui->math->xAxis2->setTickLabelColor(Qt::white);
            m_ui->math->yAxis2->setTickLabelColor(Qt::white);

            m_ui->math->xAxis->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->math->yAxis->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->math->xAxis2->setSelectedLabelColor(QColor(200, 200, 255));
            m_ui->math->yAxis2->setSelectedLabelColor(QColor(200, 200, 255));

            m_ui->math->xAxis->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->math->yAxis->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->math->xAxis2->setSelectedTickLabelColor(QColor(200,200,255));
            m_ui->math->yAxis2->setSelectedTickLabelColor(QColor(200,200,255));
        }

        // Legend Brush
        {
            QBrush brush = m_ui->math->legend->brush();
            QBrush sbrush = m_ui->math->legend->selectedBrush();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                brush.setColor(QColor::fromRgb(bc)); // Qt::black);
                sbrush.setColor(QColor::fromRgb(bc)); // Qt::black);
            }
            else
            {
                brush.setColor(QColor::fromRgb(bc)); // Qt::white);
                sbrush.setColor(QColor::fromRgb(bc)); // Qt::white);
            }

            m_ui->math->legend->setBrush(brush);
            m_ui->math->legend->setSelectedBrush(sbrush);
        }

        // Legend Border Pen
        {
            QPen pen = m_ui->math->legend->borderPen();
            QPen spen = m_ui->math->legend->selectedBorderPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                pen.setColor(Qt::black);
                spen.setColor(QColor(000, 000, 255));
            }
            else
            {
                pen.setColor(Qt::white);
                spen.setColor(QColor(200, 200, 255));
            }

            m_ui->math->legend->setBorderPen(pen);
            m_ui->math->legend->setSelectedBorderPen(spen);
        }

        // Legend Icon Border Pen
        {
            QPen pen = m_ui->math->legend->iconBorderPen();
            QPen spen = m_ui->math->legend->selectedIconBorderPen();

            if(QColor::fromRgb(bc).lightness() > 150)
            {
                pen.setColor(QColor::fromRgb(bc)); // Qt::black);
                spen.setColor(QColor(000, 000, 255));
            }
            else
            {
                pen.setColor(QColor::fromRgb(bc)); // Qt::white);
                spen.setColor(QColor(200, 200, 255));
            }

            m_ui->math->legend->setIconBorderPen(pen);
            m_ui->math->legend->setSelectedIconBorderPen(spen);
        }

        // Legend Text Color

        if(QColor::fromRgb(bc).lightness() > 150)
        {
            m_ui->math->legend->setTextColor(Qt::black);
            m_ui->math->legend->setSelectedTextColor(QColor(000, 000, 255));
        }
        else
        {
            m_ui->math->legend->setTextColor(Qt::white);
            m_ui->math->legend->setSelectedTextColor(QColor(200, 200, 255));
        }
    }
}

void SerialOscilloscope::initFttw(QSettings *settings, QWidget *parent)
{
    finiFttw();

    m_fftw_multi_threaded = false;

    if(fftw_init_threads())
    {
        m_fftw_multi_threaded = true;

        fftw_plan_with_nthreads(qMax(QThread::idealThreadCount(), 1));
    }

    bool fast = false;

    QSettings i_settings(settings->fileName(), settings->format());

    i_settings.beginGroup(PROPINO_FFTW_KEY_GROUP);

    if(i_settings.contains(PROPINO_FFTW_KEY_WISDOM))
    {
        QByteArray wisdom = i_settings.value(PROPINO_FFTW_KEY_WISDOM).
                            toString().toLatin1();

        if(fftw_import_wisdom_from_string(wisdom.constData()))
        {
            fast = true;
        }
        else
        {
            QMessageBox::critical(parent, tr("FFT Benchmark"),
            tr("Benchmark data corrupted. Propino must rebenchmark."));
        }
    }
    else
    {
        QMessageBox::information(parent, tr("FFT Benchmark"),
        tr("Propino has to benchmark your computer processor to enable "
           "Oscilloscope FFT support. Propino only needs to do this once. "
           "However, it may take a little while."));
    }

    QProgressDialog dialog(parent, Qt::MSWindowsFixedSizeDialogHint |
                                   Qt::WindowTitleHint |
                                   Qt::WindowSystemMenuHint |
                                   Qt::WindowCloseButtonHint);

    dialog.setWindowTitle(parent->windowTitle());
    dialog.setWindowModality(Qt::ApplicationModal);
    dialog.setLabelText(tr("Benchmarking Processor..."));
    dialog.setRange(0, ((sizeof(m_fftw_sizes) / sizeof(int)) * 3) - 1);

    if(!fast)
    {
        dialog.show();
    }

    QEventLoop loop;
    QFuture<fftw_plan> future;
    QFutureWatcher<fftw_plan> watcher;

    connect(&watcher, SIGNAL(finished()),
            &loop, SLOT(quit()));

    for(size_t i = 0; i < (sizeof(m_fftw_sizes) / sizeof(int)); i++)
    {
        {
            double *in; fftw_complex *out;

            q_check_ptr(in = fftw_alloc_real(m_fftw_sizes[i]));
            q_check_ptr(out = fftw_alloc_complex((m_fftw_sizes[i] / 2) + 1));

            future = QtConcurrent::run(fftw_plan_dft_r2c_1d, m_fftw_sizes[i],
            in, out, (fast?FFTW_ESTIMATE:FFTW_MEASURE) | FFTW_DESTROY_INPUT);

            watcher.setFuture(future);
            loop.exec();

            fftw_free(in);
            fftw_free(out);

            fftw_plan plan = future.result();

            if(plan)
            {
                m_fftw_plans.insertMulti(m_fftw_sizes[i], plan);
            }

        }

        if(dialog.wasCanceled())
        {
            break;
        }

        dialog.setValue((i * 3) + 0);

        {
            fftw_complex *in; fftw_complex *out;

            q_check_ptr(in = fftw_alloc_complex(m_fftw_sizes[i]));
            q_check_ptr(out = fftw_alloc_complex(m_fftw_sizes[i]));

            future = QtConcurrent::run(fftw_plan_dft_1d, m_fftw_sizes[i],
            in, out, FFTW_FORWARD, (fast?FFTW_ESTIMATE:FFTW_MEASURE)
            | FFTW_DESTROY_INPUT);

            watcher.setFuture(future);
            loop.exec();

            fftw_free(in);
            fftw_free(out);

            fftw_plan plan = future.result();

            if(plan)
            {
                m_fftw_f_plans.insertMulti(m_fftw_sizes[i], plan);
            }
        }

        if(dialog.wasCanceled())
        {
            break;
        }

        dialog.setValue((i * 3) + 1);

        {
            fftw_complex *in; fftw_complex *out;

            q_check_ptr(in = fftw_alloc_complex(m_fftw_sizes[i]));
            q_check_ptr(out = fftw_alloc_complex(m_fftw_sizes[i]));

            future = QtConcurrent::run(fftw_plan_dft_1d, m_fftw_sizes[i],
            in, out, FFTW_BACKWARD, (fast?FFTW_ESTIMATE:FFTW_MEASURE)
            | FFTW_DESTROY_INPUT);

            watcher.setFuture(future);
            loop.exec();

            fftw_free(in);
            fftw_free(out);

            fftw_plan plan = future.result();

            if(plan)
            {
                m_fftw_b_plans.insertMulti(m_fftw_sizes[i], plan);
            }
        }

        if(dialog.wasCanceled())
        {
            break;
        }

        dialog.setValue((i * 3) + 2);
    }

    m_fftw_enabled = true;

    if((m_fftw_plans.size() == (sizeof(m_fftw_sizes) / sizeof(int)))
    && (m_fftw_f_plans.size() == (sizeof(m_fftw_sizes) / sizeof(int)))
    && (m_fftw_b_plans.size() == (sizeof(m_fftw_sizes) / sizeof(int))))
    {
        char *wisdom = fftw_export_wisdom_to_string();

        i_settings.setValue(PROPINO_FFTW_KEY_WISDOM, wisdom);

        free(wisdom);

        if(!fast)
        {
            QMessageBox::information(parent, tr("FFT Benchmark"),
            tr("Benchmarking succeeded. Enabling Oscilloscope FFT support."));
        }
    }
    else
    {
        finiFttw();

        QMessageBox::critical(parent, tr("FFT Benchmark"),
        tr("Benchmarking failed. Disabling Oscilloscope FFT support."));
    }
}

void SerialOscilloscope::finiFttw()
{
    if(m_fftw_enabled)
    {
        m_fftw_enabled = false;

        foreach(fftw_plan plan, m_fftw_plans.values())
        {
            fftw_destroy_plan(plan);
        }

        m_fftw_plans.clear();

        foreach(fftw_plan plan, m_fftw_f_plans.values())
        {
            fftw_destroy_plan(plan);
        }

        m_fftw_f_plans.clear();

        foreach(fftw_plan plan, m_fftw_b_plans.values())
        {
            fftw_destroy_plan(plan);
        }

        m_fftw_b_plans.clear();

        if(m_fftw_multi_threaded)
        {
            fftw_cleanup_threads();
        }
        else
        {
            fftw_cleanup();
        }

        fftw_forget_wisdom();
    }
}

void SerialOscilloscope::zoomPlotDefault(QMouseEvent *event)
{
    if(event && (event->button() != Qt::LeftButton))
    {
        return;
    }

    if(!m_ui->plot->plottableCount())
    {
        m_ui->plot->xAxis->setRange(MIN_X_DEF_RANGE, MAX_X_DEF_RANGE);
        m_ui->plot->yAxis->setRange(MIN_Y_DEF_RANGE, MAX_Y_DEF_RANGE);
    }

    m_rescalePlotAxes = true; scheduleReplot();
}

void SerialOscilloscope::zoomMathDefault(QMouseEvent *event)
{
    if(event && (event->button() != Qt::LeftButton))
    {
        return;
    }

    if(!m_ui->math->plottableCount())
    {
        m_ui->math->xAxis->setRange(MIN_X_DEF_RANGE, MAX_X_DEF_RANGE);
        m_ui->math->yAxis->setRange(MIN_Y_DEF_RANGE, MAX_Y_DEF_RANGE);
    }

    m_rescaleMathAxes = true; scheduleReplot();
}

void SerialOscilloscope::saveBmp()
{
    QCustomPlot *choice = NULL;

    if(m_ui->math->isVisible())
    {
        QMessageBox box(QMessageBox::Question, tr("Save Bmp"),
        tr("Which plot do you want to save?"), QMessageBox::Cancel, this);

        QPushButton *timeDomain =
        box.addButton(tr("Time"), QMessageBox::ActionRole);

        QPushButton *freqDomain =
        box.addButton(tr("Frequency"), QMessageBox::ActionRole);

        box.exec();

        if(box.clickedButton() == timeDomain)
        {
            choice = m_ui->plot; m_ui->plot->deselectAll();
            m_ui->plot->replot();
        }

        if(box.clickedButton() == freqDomain)
        {
            choice = m_ui->math; m_ui->math->deselectAll();
            m_ui->math->replot();
        }
    }
    else
    {
        choice = m_ui->plot; m_ui->plot->deselectAll(); m_ui->plot->replot();
    }

    if(choice)
    {
        QSettings settings(m_settings->fileName(), m_settings->format());

        settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
        settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

        QString savePath = settings.value(
        SERIAL_OSCILLOSCOPE_KEY_SAVE_BMP_FILE,
        QDir::homePath()).toString();

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Bmp"),
        savePath + QDir::separator() + tr("untitled.bmp"),
        tr("Image Format (*.bmp)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Save Bmp Error"),
                tr("Invalid empty filename string"));
            }
            else if(!choice->saveBmp(fileName))
            {
                QMessageBox::critical(this, tr("Save Bmp Error"),
                tr("An unknown error occured"));
            }
            else
            {
                QFileInfo fileInfo(fileName);
                settings.setValue(SERIAL_OSCILLOSCOPE_KEY_SAVE_BMP_FILE,
                                  fileInfo.canonicalPath());
            }
        }
    }
}

void SerialOscilloscope::saveJpg()
{
    QCustomPlot *choice = NULL;

    if(m_ui->math->isVisible())
    {
        QMessageBox box(QMessageBox::Question, tr("Save Jpg"),
        tr("Which plot do you want to save?"), QMessageBox::Cancel, this);

        QPushButton *timeDomain =
        box.addButton(tr("Time"), QMessageBox::ActionRole);

        QPushButton *freqDomain =
        box.addButton(tr("Frequency"), QMessageBox::ActionRole);

        box.exec();

        if(box.clickedButton() == timeDomain)
        {
            choice = m_ui->plot; m_ui->plot->deselectAll();
            m_ui->plot->replot();
        }

        if(box.clickedButton() == freqDomain)
        {
            choice = m_ui->math; m_ui->math->deselectAll();
            m_ui->math->replot();
        }
    }
    else
    {
        choice = m_ui->plot; m_ui->plot->deselectAll(); m_ui->plot->replot();
    }

    if(choice)
    {
        QSettings settings(m_settings->fileName(), m_settings->format());

        settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
        settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

        QString savePath = settings.value(
        SERIAL_OSCILLOSCOPE_KEY_SAVE_JPG_FILE,
        QDir::homePath()).toString();

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Jpg"),
        savePath + QDir::separator() + tr("untitled.jpg"),
        tr("Image Format (*.jpg)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Save Jpg Error"),
                tr("Invalid empty filename string"));
            }
            else if(!choice->saveJpg(fileName))
            {
                QMessageBox::critical(this, tr("Save Jpg Error"),
                tr("An unknown error occured"));
            }
            else
            {
                QFileInfo fileInfo(fileName);
                settings.setValue(SERIAL_OSCILLOSCOPE_KEY_SAVE_JPG_FILE,
                                  fileInfo.canonicalPath());
            }
        }
    }
}

void SerialOscilloscope::savePng()
{ 
    QCustomPlot *choice = NULL;

    if(m_ui->math->isVisible())
    {
        QMessageBox box(QMessageBox::Question, tr("Save Png"),
        tr("Which plot do you want to save?"), QMessageBox::Cancel, this);

        QPushButton *timeDomain =
        box.addButton(tr("Time"), QMessageBox::ActionRole);

        QPushButton *freqDomain =
        box.addButton(tr("Frequency"), QMessageBox::ActionRole);

        box.exec();

        if(box.clickedButton() == timeDomain)
        {
            choice = m_ui->plot; m_ui->plot->deselectAll();
            m_ui->plot->replot();
        }

        if(box.clickedButton() == freqDomain)
        {
            choice = m_ui->math; m_ui->math->deselectAll();
            m_ui->math->replot();
        }
    }
    else
    {
        choice = m_ui->plot; m_ui->plot->deselectAll(); m_ui->plot->replot();
    }

    if(choice)
    {
        QSettings settings(m_settings->fileName(), m_settings->format());

        settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
        settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

        QString savePath = settings.value(
        SERIAL_OSCILLOSCOPE_KEY_SAVE_PNG_FILE,
        QDir::homePath()).toString();

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Png"),
        savePath + QDir::separator() + tr("untitled.png"),
        tr("Image Format (*.png)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Save Png Error"),
                tr("Invalid empty filename string"));
            }
            else if(!choice->savePng(fileName))
            {
                QMessageBox::critical(this, tr("Save Png Error"),
                tr("An unknown error occured"));
            }
            else
            {
                QFileInfo fileInfo(fileName);
                settings.setValue(SERIAL_OSCILLOSCOPE_KEY_SAVE_PNG_FILE,
                                  fileInfo.canonicalPath());
            }
        }
    }
}

void SerialOscilloscope::savePdf()
{
    QCustomPlot *choice = NULL;

    if(m_ui->math->isVisible())
    {
        QMessageBox box(QMessageBox::Question, tr("Save Pdf"),
        tr("Which plot do you want to save?"), QMessageBox::Cancel, this);

        QPushButton *timeDomain =
        box.addButton(tr("Time"), QMessageBox::ActionRole);

        QPushButton *freqDomain =
        box.addButton(tr("Frequency"), QMessageBox::ActionRole);

        box.exec();

        if(box.clickedButton() == timeDomain)
        {
            choice = m_ui->plot; m_ui->plot->deselectAll();
            m_ui->plot->replot();
        }

        if(box.clickedButton() == freqDomain)
        {
            choice = m_ui->math; m_ui->math->deselectAll();
            m_ui->math->replot();
        }
    }
    else
    {
        choice = m_ui->plot; m_ui->plot->deselectAll(); m_ui->plot->replot();
    }

    if(choice)
    {
        QSettings settings(m_settings->fileName(), m_settings->format());

        settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
        settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

        QString savePath = settings.value(
        SERIAL_OSCILLOSCOPE_KEY_SAVE_PDF_FILE,
        QDir::homePath()).toString();

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Pdf"),
        savePath + QDir::separator() + tr("untitled.pdf"),
        tr("Image Format (*.pdf)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Save Pdf Error"),
                tr("Invalid empty filename string"));
            }
            else if(!choice->savePdf(fileName))
            {
                QMessageBox::critical(this, tr("Save Pdf Error"),
                tr("An unknown error occured"));
            }
            else
            {
                QFileInfo fileInfo(fileName);
                settings.setValue(SERIAL_OSCILLOSCOPE_KEY_SAVE_PDF_FILE,
                                  fileInfo.canonicalPath());
            }
        }
    }
}

void SerialOscilloscope::exportState()
{
    QJsonArray plottableList;

    QMap<unsigned int, QCPAbstractPlottable*>::const_iterator i =
    m_plottables.constBegin();

    QMap<unsigned int, QCPAbstractPlottable*>::const_iterator j =
    m_plottables.constEnd();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for(; i != j; i++)
    {
        QJsonObject plottable;

        plottable.insert("handle",
        double(i.key()));

        plottable.insert("name",
        i.value()->name());

        plottable.insert("penColor",
        i.value()->pen().color().name()); // ignores alpha

        plottable.insert("penStyle",
        i.value()->pen().style());

        plottable.insert("brushColor",
        i.value()->brush().color().name()); // ignores alpha

        plottable.insert("brushStyle",
        i.value()->brush().style());

        QCPBars *bar =
        qobject_cast<QCPBars*>(i.value());

        if(bar)
        {
            plottable.insert("type", QString("bar"));

            QList<QVariant> keyList, valueList;

            foreach(QCPBarData data, bar->data()->values())
            {
                keyList.append(data.key);
                valueList.append(data.value);
            }

            plottable.insert("dataKeys",
            QVariant(keyList).toJsonArray());

            plottable.insert("dataValues",
            QVariant(valueList).toJsonArray());

            if(bar->barAbove())
            {
                plottable.insert("barAbove",
                double(m_plottables.key(bar->barAbove())));
            }

            if(bar->barBelow())
            {
                plottable.insert("barBelow",
                double(m_plottables.key(bar->barBelow())));
            }
        }

        MyQCPCurve *curve =
        qobject_cast<MyQCPCurve*>(i.value());

        if(curve)
        {
            plottable.insert("type", QString("curve"));

            plottable.insert("deltaMode", curve->deltaMode);
            plottable.insert("size", double(curve->size));
            plottable.insert("sampleRate", curve->sampleRate);
            plottable.insert("scaler", curve->scaler);

            plottable.insert("lineStyle", curve->lineStyle());
            plottable.insert("scatterStyle", curve->scatterStyle().shape());

            plottable.insert("FFTWOption", curve->FFTWOption);

            QList<QVariant> timeList, keyList, valueList;

            foreach(QCPCurveData data, curve->data()->values())
            {
                timeList.append(data.t);
                keyList.append(data.key);
                valueList.append(data.value);
            }

            plottable.insert("dataTimes",
            QVariant(timeList).toJsonArray());

            plottable.insert("dataKeys",
            QVariant(keyList).toJsonArray());

            plottable.insert("dataValues",
            QVariant(valueList).toJsonArray());

            /*

            if(curve->channelFillCurve())
            {
                plottable.insert("channelFillCurve",
                double(m_plottables.key(graph->channelFillCurve())));
            }

            */
        }

        MyQCPGraph *graph =
        qobject_cast<MyQCPGraph*>(i.value());

        if(graph)
        {
            plottable.insert("type", QString("graph"));

            plottable.insert("deltaMode", graph->deltaMode);
            plottable.insert("size", double(graph->size));
            plottable.insert("sampleRate", graph->sampleRate);
            plottable.insert("scaler", graph->scaler);

            plottable.insert("lineStyle", graph->lineStyle());
            plottable.insert("scatterStyle", graph->scatterStyle().shape());

            plottable.insert("FFTWOption", graph->FFTWOption);

            QList<QVariant> keyList, valueList;

            foreach(QCPData data, graph->data()->values())
            {
                keyList.append(data.key);
                valueList.append(data.value);
            }

            plottable.insert("dataKeys",
            QVariant(keyList).toJsonArray());

            plottable.insert("dataValues",
            QVariant(valueList).toJsonArray());

            if(graph->channelFillGraph())
            {
                plottable.insert("channelFillGraph",
                double(m_plottables.key(graph->channelFillGraph())));
            }
        }

        QCPStatisticalBox *box =
        qobject_cast<QCPStatisticalBox*>(i.value());

        if(box)
        {
            plottable.insert("type", QString("box"));

            plottable.insert("key", box->key());
            plottable.insert("minimum", box->minimum());
            plottable.insert("lowerQuartile", box->lowerQuartile());
            plottable.insert("median", box->median());
            plottable.insert("upperQuartile", box->upperQuartile());
            plottable.insert("maximum", box->maximum());
        }

        plottableList.append(plottable);
    }

    QApplication::restoreOverrideCursor();

    ///////////////////////////////////////////////////////////////////////////

    QJsonObject jsonObject;

    jsonObject.insert("name", windowTitle());

    // plot

    jsonObject.insert("XAxisScaleType",
    m_ui->plot->xAxis->scaleType());

    jsonObject.insert("XAxisScaleLogBase",
    m_ui->plot->xAxis->scaleLogBase());

    jsonObject.insert("XAxisRangeReversed",
    m_ui->plot->xAxis->rangeReversed());

    jsonObject.insert("XAxisLabel",
    m_ui->plot->xAxis->label());

    jsonObject.insert("YAxisScaleType",
    m_ui->plot->yAxis->scaleType());

    jsonObject.insert("YAxisScaleLogBase",
    m_ui->plot->yAxis->scaleLogBase());

    jsonObject.insert("YAxisRangeReversed",
    m_ui->plot->yAxis->rangeReversed());

    jsonObject.insert("YAxisLabel",
    m_ui->plot->yAxis->label());

    // math

    jsonObject.insert("FFTXAxisScaleType",
    m_ui->math->xAxis->scaleType());

    jsonObject.insert("FFTXAxisScaleLogBase",
    m_ui->math->xAxis->scaleLogBase());

    jsonObject.insert("FFTXAxisRangeReversed",
    m_ui->math->xAxis->rangeReversed());

    jsonObject.insert("FFTXAxisLabel",
    m_ui->math->xAxis->label());

    jsonObject.insert("FFTYAxisScaleType",
    m_ui->math->yAxis->scaleType());

    jsonObject.insert("FFTYAxisScaleLogBase",
    m_ui->math->yAxis->scaleLogBase());

    jsonObject.insert("FFTYAxisRangeReversed",
    m_ui->math->yAxis->rangeReversed());

    jsonObject.insert("FFTYAxisLabel",
    m_ui->math->yAxis->label());

    jsonObject.insert("background", m_background.name());

    jsonObject.insert("plottables", plottableList);

    ///////////////////////////////////////////////////////////////////////////

    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(SERIAL_OSCILLOSCOPE_KEY_GROUP);
    settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    QString saveFile = settings.value(SERIAL_OSCILLOSCOPE_KEY_EXPORT_S_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export State"),
    saveFile, tr("JSON File (*.json)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Export State Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QByteArray json = QJsonDocument(jsonObject).toJson();

                if(file.write(json) == json.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(SERIAL_OSCILLOSCOPE_KEY_EXPORT_S_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Export State Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Export State Error"),
                file.errorString());
            }
        }
    }
}

void SerialOscilloscope::importState()
{

}

void SerialOscilloscope::generalHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open General Help Error"),
        tr("Unable to open the URL to the Propino General Help page"));
    }
}

void SerialOscilloscope::oscilloscopeHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open Oscilloscope Help Error"),
        tr("Unable to open the URL to the Propino Oscilloscope Help page"));
    }
}

void SerialOscilloscope::plotMousePress(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QCPAbstractPlottable *plottable =
        m_ui->plot->plottableAt(event->posF());

        if(plottable)
        {
            double x = m_ui->plot->xAxis->pixelToCoord(event->posF().x());
            double y = m_ui->plot->yAxis->pixelToCoord(event->posF().y());

            QCPBars *bar =
            qobject_cast<QCPBars*>(plottable);

            if(bar)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPBarData data, bar->data()->values())
                {
                    double d = qAbs(x - data.key);

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    for(QCPBars *below = bar->barBelow();
                    ((below != NULL) && below->data()->contains(key));
                    below = below->barBelow())
                    {
                        value += below->data()->value(key).value;
                    }

                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>Key:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Val:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(bar->name().isEmpty() ? "..." : bar->name()).
                       arg(key).
                       arg(value),
                       m_ui->plot, m_ui->plot->rect());
                }
            }

            QCPCurve *curve =
            qobject_cast<QCPCurve*>(plottable);

            if(curve)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPCurveData data, curve->data()->values())
                {
                    double d = qSqrt(qPow(x-data.key,2)+qPow(y-data.value,2));

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>X:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Y:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(curve->name().isEmpty() ? "..." : curve->name()).
                       arg(key).
                       arg(value),
                       m_ui->plot, m_ui->plot->rect());
                }
            }

            QCPGraph *graph =
            qobject_cast<QCPGraph*>(plottable);

            if(graph)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPData data, graph->data()->values())
                {
                    double d = qAbs(x - data.key);

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>X:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Y:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(graph->name().isEmpty() ? "..." : graph->name()).
                       arg(key).
                       arg(value),
                       m_ui->plot, m_ui->plot->rect());
                }
            }

            QCPStatisticalBox *box =
            qobject_cast<QCPStatisticalBox*>(plottable);

            if(box)
            {
                double key = box->key();
                double minimum = box->minimum();
                double lowerQuartile = box->lowerQuartile();
                double median = box->median();
                double upperQuartile = box->upperQuartile();
                double maximum = box->maximum();

                QToolTip::hideText();
                QToolTip::showText(event->globalPos(),
                tr("<table>"
                     "<tr>"
                       "<th colspan=\"2\">%L1</th>"
                     "</tr>"
                     "<tr>"
                       "<td>Key:</td>" "<td>%L2</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Min:</td>" "<td>%L3</td>"
                     "</tr>"
                     "<tr>"
                       "<td>L-Q:</td>" "<td>%L4</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Mid:</td>" "<td>%L5</td>"
                     "</tr>"
                     "<tr>"
                       "<td>U-Q:</td>" "<td>%L6</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Max:</td>" "<td>%L7</td>"
                     "</tr>"
                   "</table>").
                   arg(box->name().isEmpty() ? "..." : box->name()).
                   arg(key).
                   arg(minimum).
                   arg(lowerQuartile).
                   arg(median).
                   arg(upperQuartile).
                   arg(maximum),
                   m_ui->plot, m_ui->plot->rect());
            }
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        m_rescalePlotAxes = false;
    }
}

void SerialOscilloscope::plotMouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event); m_rescalePlotAxes = false;
}

void SerialOscilloscope::mathMousePress(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QCPAbstractPlottable *plottable =
        m_ui->math->plottableAt(event->posF());

        if(plottable)
        {
            double x = m_ui->math->xAxis->pixelToCoord(event->posF().x());
            double y = m_ui->math->yAxis->pixelToCoord(event->posF().y());

            QCPBars *bar =
            qobject_cast<QCPBars*>(plottable);

            if(bar)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPBarData data, bar->data()->values())
                {
                    double d = qAbs(x - data.key);

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    for(QCPBars *below = bar->barBelow();
                    ((below != NULL) && below->data()->contains(key));
                    below = below->barBelow())
                    {
                        value += below->data()->value(key).value;
                    }

                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>Key:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Val:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(bar->name().isEmpty() ? "..." : bar->name()).
                       arg(key).
                       arg(value),
                       m_ui->math, m_ui->math->rect());
                }
            }

            QCPCurve *curve =
            qobject_cast<QCPCurve*>(plottable);

            if(curve)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPCurveData data, curve->data()->values())
                {
                    double d = qSqrt(qPow(x-data.key,2)+qPow(y-data.value,2));

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>X:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Y:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(curve->name().isEmpty() ? "..." : curve->name()).
                       arg(key).
                       arg(value),
                       m_ui->math, m_ui->math->rect());
                }
            }

            QCPGraph *graph =
            qobject_cast<QCPGraph*>(plottable);

            if(graph)
            {
                double key = 0;
                double value = 0;

                bool ok = false;
                double m = std::numeric_limits<double>::max();

                foreach(QCPData data, graph->data()->values())
                {
                    double d = qAbs(x - data.key);

                    if(d < m)
                    {
                        key = data.key;
                        value = data.value;

                        ok = true;
                        m = d;
                    }
                }

                if(ok)
                {
                    QToolTip::hideText();
                    QToolTip::showText(event->globalPos(),
                    tr("<table>"
                         "<tr>"
                           "<th colspan=\"2\">%L1</th>"
                         "</tr>"
                         "<tr>"
                           "<td>X:</td>" "<td>%L2</td>"
                         "</tr>"
                         "<tr>"
                           "<td>Y:</td>" "<td>%L3</td>"
                         "</tr>"
                       "</table>").
                       arg(graph->name().isEmpty() ? "..." : graph->name()).
                       arg(key).
                       arg(value),
                       m_ui->math, m_ui->math->rect());
                }
            }

            QCPStatisticalBox *box =
            qobject_cast<QCPStatisticalBox*>(plottable);

            if(box)
            {
                double key = box->key();
                double minimum = box->minimum();
                double lowerQuartile = box->lowerQuartile();
                double median = box->median();
                double upperQuartile = box->upperQuartile();
                double maximum = box->maximum();

                QToolTip::hideText();
                QToolTip::showText(event->globalPos(),
                tr("<table>"
                     "<tr>"
                       "<th colspan=\"2\">%L1</th>"
                     "</tr>"
                     "<tr>"
                       "<td>Key:</td>" "<td>%L2</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Min:</td>" "<td>%L3</td>"
                     "</tr>"
                     "<tr>"
                       "<td>L-Q:</td>" "<td>%L4</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Mid:</td>" "<td>%L5</td>"
                     "</tr>"
                     "<tr>"
                       "<td>U-Q:</td>" "<td>%L6</td>"
                     "</tr>"
                     "<tr>"
                       "<td>Max:</td>" "<td>%L7</td>"
                     "</tr>"
                   "</table>").
                   arg(box->name().isEmpty() ? "..." : box->name()).
                   arg(key).
                   arg(minimum).
                   arg(lowerQuartile).
                   arg(median).
                   arg(upperQuartile).
                   arg(maximum),
                   m_ui->math, m_ui->math->rect());
            }
        }
    }
    else if(event->button() == Qt::LeftButton)
    {
        m_rescaleMathAxes = false;
    }
}

void SerialOscilloscope::mathMouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event); m_rescaleMathAxes = false;
}

void SerialOscilloscope::replot()
{
    if(m_fftw_enabled)
    {
        m_ui->math->legend->setVisible(false);

        QList<MyQCPCurve*> curveList;
        QList<MyQCPGraph*> graphList;

        foreach(QCPAbstractPlottable *plottable, m_plottables.values())
        {
            MyQCPCurve *curve = qobject_cast<MyQCPCurve*>(plottable);

            if(curve && (curve->FFTWOption != MyQCPCurve::none))
            {
                curveList.append(curve);

                if(!curve->name().isEmpty())
                {
                    m_ui->math->legend->setVisible(true);
                }
            }

            MyQCPGraph *graph = qobject_cast<MyQCPGraph*>(plottable);

            if(graph && (graph->FFTWOption != MyQCPGraph::none))
            {
                graphList.append(graph);

                if(!graph->name().isEmpty())
                {
                    m_ui->math->legend->setVisible(true);
                }
            }
        }

        if(((!curveList.isEmpty()) || (!graphList.isEmpty())))
        {
            // do not use ui->math->isVisible() here

            if(m_ui->plot->plotLayout()->rowCount() == 1)
            {
                m_ui->plot->plotLayout()->insertRow(0);
                m_ui->plot->plotLayout()->addElement(0, 0,
                new QCPPlotTitle(m_ui->plot, tr("Time Domain")));

                m_ui->math->plotLayout()->insertRow(0);
                m_ui->math->plotLayout()->addElement(0, 0,
                new QCPPlotTitle(m_ui->math, tr("Frequency Domain")));
            }

            m_ui->math->setVisible(true);

            QFutureSynchronizer<void> sync;

            if(!curveList.isEmpty())
            {
                sync.addFuture(QtConcurrent::map(curveList,
                SerialOscilloscope::calculateCurveFft));
            }

            if(!graphList.isEmpty())
            {
                sync.addFuture(QtConcurrent::map(graphList,
                SerialOscilloscope::calculateGraphFft));
            }

            sync.waitForFinished();
        }
        else
        {
            // do not use ui->math->isVisible() here

            if(m_ui->plot->plotLayout()->rowCount() != 1)
            {
                m_ui->plot->plotLayout()->remove(
                m_ui->plot->plotLayout()->element(0, 0));

                m_ui->math->plotLayout()->remove(
                m_ui->math->plotLayout()->element(0, 0));
            }

            m_ui->math->setVisible(false);
        }
    }

    m_ui->plot->legend->setVisible(false);

    foreach(QCPAbstractPlottable *plottable, m_plottables.values())
    {
        if(!plottable->name().isEmpty())
        {
            m_ui->plot->legend->setVisible(true); break;
        }
    }

    if(m_rescalePlotAxes)
    {
        m_ui->plot->rescaleAxes();
    }

    if(m_rescaleMathAxes)
    {
        m_ui->math->rescaleAxes();
    }

    m_ui->plot->deselectAll(); m_ui->plot->replot();
    m_ui->math->deselectAll(); m_ui->math->replot();

    m_replotLock.unlock();
}

void SerialOscilloscope::scheduleReplot()
{
    if(m_replotLock.tryLock())
    {
        QTimer::singleShot(UPDATE_RATE, this, SLOT(replot()));
    }
}

void SerialOscilloscope::calculateCurveFft(MyQCPCurve *curve)
{
    size_t i = 0;

    for(; (i < (sizeof(m_fftw_sizes) / sizeof(int)))
    && (curve->data()->size() > m_fftw_sizes[i]); i++);

    fftw_complex *in; fftw_complex *out;

    q_check_ptr(in = fftw_alloc_complex(m_fftw_sizes[i]));
    q_check_ptr(out = fftw_alloc_complex(m_fftw_sizes[i]));

    qMemSet(in, 0, (sizeof(fftw_complex) * m_fftw_sizes[i]));
    qMemSet(out, 0, (sizeof(fftw_complex) * m_fftw_sizes[i]));

    int j = 0;

    foreach(QCPCurveData data, curve->data()->values())
    {
        in[j][0] = data.key; in[j++][1] = data.value;
    }

    switch(curve->FFTWOption)
    {
        case MyQCPCurve::none: break;

        case MyQCPCurve::forward:
        fftw_execute_dft(m_fftw_f_plans[m_fftw_sizes[i]], in, out); break;

        case MyQCPCurve::backward:
        fftw_execute_dft(m_fftw_b_plans[m_fftw_sizes[i]], in, out); break;

        default: break;
    }

    curve->FFTWPlot->clearData();

    double deltaFrequency = ((1 / curve->sampleRate) / m_fftw_sizes[i]);

    int size = m_fftw_sizes[i];

    for(int k = 0; k < size; k++)
    {
        double f = k * deltaFrequency;

        curve->FFTWPlot->addData(f, out[k][0], out[k][1]);
    }

    fftw_free(in);
    fftw_free(out);
}

void SerialOscilloscope::calculateGraphFft(MyQCPGraph *graph)
{
    size_t i = 0;

    for(; (i < (sizeof(m_fftw_sizes) / sizeof(int)))
    && (graph->data()->size() > m_fftw_sizes[i]); i++);

    double *in; fftw_complex *out;

    q_check_ptr(in = fftw_alloc_real(m_fftw_sizes[i]));
    q_check_ptr(out = fftw_alloc_complex((m_fftw_sizes[i] / 2) + 1));

    qMemSet(in, 0, (sizeof(double) * m_fftw_sizes[i]));
    qMemSet(out, 0, (sizeof(fftw_complex) * ((m_fftw_sizes[i] / 2) + 1)));

    int j = 0;

    foreach(QCPData data, graph->data()->values())
    {
        in[j++] = data.value;
    }

    fftw_execute_dft_r2c(m_fftw_plans[m_fftw_sizes[i]], in, out);

    graph->FFTWPlot->clearData();

    double deltaFrequency = ((1 / graph->sampleRate) / m_fftw_sizes[i]);

    int size = ((m_fftw_sizes[i] / 2) + 1);

    for(int k = 0; k < size; k++)
    {
        double f = k * deltaFrequency;

        switch(graph->FFTWOption)
        {
            case MyQCPGraph::none: break;

            case MyQCPGraph::magnitude:
            {
                graph->FFTWPlot->addData(f,
                (qSqrt(qPow(out[k][0], 2) + qPow(out[k][1], 2)) * 2)
                / m_fftw_sizes[i]); break;
            }

            case MyQCPGraph::magnitude2:
            {
                graph->FFTWPlot->addData(f,
                log2((qSqrt(qPow(out[k][0], 2) + qPow(out[k][1], 2)) * 2)
                / m_fftw_sizes[i])); break;
            }

            case MyQCPGraph::magnitude10: // 10*log10()
            {
                graph->FFTWPlot->addData(f, 10 *
                log10((qSqrt(qPow(out[k][0], 2) + qPow(out[k][1], 2)) * 2)
                / m_fftw_sizes[i])); break;
            }

            case MyQCPGraph::magnitude20: // 20*log10()
            {
                graph->FFTWPlot->addData(f, 20 *
                log10((qSqrt(qPow(out[k][0], 2) + qPow(out[k][1], 2)) * 2)
                / m_fftw_sizes[i])); break;
            }

            case MyQCPGraph::phaseRadians:
            {
                graph->FFTWPlot->addData(f,
                qAtan2(out[k][1], out[k][0])); break;
            }

            case MyQCPGraph::phaseDegrees:
            {
                graph->FFTWPlot->addData(f,
                qRadiansToDegrees(qAtan2(out[k][1], out[k][0]))); break;
            }

            default: break;
        }
    }

    fftw_free(in);
    fftw_free(out);
}

template <class QCP>
void SerialOscilloscope::newPlottable(unsigned int handle, const QString &name)
{
    if(!m_plottables.contains(handle))
    {
        QCP *plottable = new QCP(m_ui->plot->xAxis, m_ui->plot->yAxis);
        m_ui->plot->addPlottable(plottable);

        plottable->setPen(m_pens[handle %
        (sizeof(m_pens) / sizeof(QPen))]);

        plottable->setSelectedPen(m_spens[handle %
        (sizeof(m_spens) / sizeof(QPen))]);

        plottable->setBrush(m_brushes[handle %
        (sizeof(m_brushes) / sizeof(QBrush))]);

        plottable->setSelectedBrush(m_sbrushes[handle %
        (sizeof(m_sbrushes) / sizeof(QBrush))]);

        m_plottables.insert(handle, plottable);
    }

    if(name.isNull())
    {
        m_plottables.value(handle)->setName(tr("Plot %L1").arg(handle + 1));
    }
    else
    {
        m_plottables.value(handle)->setName(name);
    }

    if(m_plottables.value(handle)->name().isEmpty())
    {
        m_plottables.value(handle)->removeFromLegend();
    }
    else
    {
        m_plottables.value(handle)->addToLegend();
    }
}

template <class QCP>
QCP *SerialOscilloscope::verifyHandle(unsigned int handle, const QString &name)
{
    if(m_plottables.contains(handle))
    {
        QCP *plottable = qobject_cast<QCP*>(m_plottables.value(handle));

        if(!plottable)
        {
            emit error("[" + windowTitle() + "]::[" +
                       m_plottables.value(handle)->name() + "]::" +
                       name + tr(": Invalid plottable handle type"));
        }

        return plottable;
    }

    emit error("[" + windowTitle() + "]::" +
               name + tr(": Invalid plottable handle number %L1").arg(handle));

    return NULL;
}

void SerialOscilloscope::error(unsigned int handle, const QString &message)
{
    emit error("[" + windowTitle() + "]::[" +
               m_plottables.value(handle)->name() + "]::" +
               message);
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
