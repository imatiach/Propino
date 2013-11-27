/***************************************************************************//**
* @file
* Util Path Picker
*
* @version @n 1.0
* @date @n 10/10/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/10/2013
*******************************************************************************/

#include "utilpathpicker.h"
#include "ui_utilpathpicker.h"

UtilPathPicker::UtilPathPicker(const QString &title,
                               const QString &label,
                               const QString &defaultDir,
                               QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::UtilPathPicker)
{
    m_ui->setupUi(this); setWindowTitle(title); m_ui->label->setText(label);

    m_ui->edit->setText(defaultDir);
    m_ui->edit->selectAll();

    QFileIconProvider provider;

    m_ui->button->setIcon(provider.icon(QFileIconProvider::Folder));

    connect(m_ui->button, SIGNAL(clicked()),
            this, SLOT(pick()));

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog);
}

UtilPathPicker::~UtilPathPicker()
{
    delete m_ui;
}

void UtilPathPicker::pick()
{
    QString dirReturn = QFileDialog::getExistingDirectory(this,
                                                          windowTitle(),
                                                          m_ui->edit->text());

    if(!dirReturn.isNull())
    {
        m_ui->edit->setText(dirReturn);
    }
}

void UtilPathPicker::setDefaultDir(const QString &text)
{
    m_ui->edit->setText(text);
}

QString UtilPathPicker::getDefaultDir() const
{
    return m_ui->edit->text();
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
