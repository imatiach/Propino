/***************************************************************************//**
* @file
* Program Loader Dialog
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

#ifndef PROGRAMLOADERDIALOG_H
#define PROGRAMLOADERDIALOG_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "propino_settings.h"

#include "qftdi.h"
#include "serialescape.h"
#include "serialloader.h"

namespace Ui
{
    class ProgramLoaderDialog;
}

class ProgramLoaderDialog : public QDialog
{
    Q_OBJECT

public:

    explicit ProgramLoaderDialog(QFtdi *ftdi, QSettings *settings,
                                 QWidget *parent = NULL,
                                 const QString &prefix = PROGRAM_BUILDER,
                                 SerialEscape *escape = NULL);
    ~ProgramLoaderDialog();

public slots:

    void setLoadButtonVisibility(bool state);

    void loadBinary(QProgressDialog *dialog = NULL,
                    SerialLoader *loader = NULL);

private slots:

    void currentIndexChanged(int index);

    void binaryButton();
    void stage1Button();
    void stage2Button();

    void accept();
    void reject();

private:

    Q_DISABLE_COPY(ProgramLoaderDialog)

    static const QString m_protocol[];
    static const int m_hiddenThings[];

    QFtdi *m_ftdi; QSettings *m_settings;
    QString m_prefix; SerialEscape *m_escape;

    Ui::ProgramLoaderDialog *m_ui;
};

#endif // PROGRAMLOADERDIALOG_H

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
