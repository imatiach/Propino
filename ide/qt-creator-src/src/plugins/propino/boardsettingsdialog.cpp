/***************************************************************************//**
* @file
* Board Settings Dialog
*
* @version @n 1.0
* @date @n 9/18/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 9/18/2013
*******************************************************************************/

#include "boardsettingsdialog.h"
#include "ui_boardsettingsdialog.h"

BoardSettingsDialog::BoardSettingsDialog(QFtdi *ftdi, QSettings *settings,
                                         QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::BoardSettingsDialog)
{
    m_ui->setupUi(this); Q_CHECK_PTR(ftdi); Q_CHECK_PTR(settings);

    m_ftdi = ftdi; m_settings = settings;

    m_ui->boardNameEdit->setText(m_ftdi->getFtdiInfo().name);
    m_ui->boardNameEdit->selectAll();

    m_ui->cmakeScriptEdit->setText(m_ftdi->getFtdiInfo().make);
    m_ui->cmakeScriptEdit->selectAll();

    QFileIconProvider provider;

    m_ui->cmakeScriptButton->setIcon(provider.icon(QFileIconProvider::Folder));

    switch(m_ftdi->getFtdiInfo().node.Type)
    {
        case FT_DEVICE_BM:
            m_ui->deviceTypeEdit->setText(tr("BM")); break;

        case FT_DEVICE_AM:
            m_ui->deviceTypeEdit->setText(tr("AM")); break;

        case FT_DEVICE_100AX:
            m_ui->deviceTypeEdit->setText(tr("100AX")); break;

        case FT_DEVICE_UNKNOWN:
            m_ui->deviceTypeEdit->setText(tr("Unknown")); break;

        case FT_DEVICE_2232C:
            m_ui->deviceTypeEdit->setText(tr("2232C")); break;

        case FT_DEVICE_232R:
            m_ui->deviceTypeEdit->setText(tr("232R")); break;

        case FT_DEVICE_2232H:
            m_ui->deviceTypeEdit->setText(tr("2232H")); break;

        case FT_DEVICE_4232H:
            m_ui->deviceTypeEdit->setText(tr("4232H")); break;

        case FT_DEVICE_232H:
            m_ui->deviceTypeEdit->setText(tr("232H")); break;

        case FT_DEVICE_X_SERIES:
            m_ui->deviceTypeEdit->setText(tr("X Series")); break;

        default:
            m_ui->deviceTypeEdit->setText(tr("Unknown")); break;
    }

    m_ui->deviceIdEdit->setText("0x" + QString("%1").arg(
    m_ftdi->getFtdiInfo().node.ID, 0x08L, 0x10L, QLatin1Char('0')).toUpper());

    m_ui->serialNumberEdit->setText(m_ftdi->getFtdiInfo().node.SerialNumber);
    m_ui->descriptionEdit->setText(m_ftdi->getFtdiInfo().node.Description);

    m_ui->cmakeScript->hide();
    m_ui->cmakeScriptEdit->hide();
    m_ui->cmakeScriptButton->hide();
    m_ui->deviceType->hide();
    m_ui->deviceTypeEdit->hide();
    m_ui->deviceId->hide();
    m_ui->deviceIdEdit->hide();
    m_ui->serialNumber->hide();
    m_ui->serialNumberEdit->hide();
    m_ui->description->hide();
    m_ui->descriptionEdit->hide();

    connect(m_ui->advancedBox, SIGNAL(clicked()),
            this, SLOT(advanced()));

    connect(m_ui->cmakeScriptButton, SIGNAL(clicked()),
            this, SLOT(choose()));

    connect(m_ftdi, SIGNAL(aboutToClose()),
            this, SLOT(reject()));

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog);
}

BoardSettingsDialog::~BoardSettingsDialog()
{
    delete m_ui;
}

void BoardSettingsDialog::advanced()
{
    m_ui->advancedBox->hide();
    m_ui->cmakeScript->show();
    m_ui->cmakeScriptEdit->show();
    m_ui->cmakeScriptButton->show();
    m_ui->deviceType->show();
    m_ui->deviceTypeEdit->show();
    m_ui->deviceId->show();
    m_ui->deviceIdEdit->show();
    m_ui->serialNumber->show();
    m_ui->serialNumberEdit->show();
    m_ui->description->show();
    m_ui->descriptionEdit->show();
}

void BoardSettingsDialog::choose()
{
    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(BOARD_SETTINGS_KEY_GROUP);

    QMessageBox messageBox(QMessageBox::Question, windowTitle(), tr("Do you "
    "want to select a file in your personal folder or system folder?"),
    QMessageBox::Cancel, this);

    QPushButton *userButton =
    messageBox.addButton(tr("Personal"), QMessageBox::ActionRole);

    QPushButton *systemButton =
    messageBox.addButton(tr("System"), QMessageBox::ActionRole);

    messageBox.exec();

    if(messageBox.clickedButton() == userButton)
    {
        QString openFile = settings.value(BOARD_SETTINGS_KEY_OPEN_USER_FILE).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select CMake Script"), openFile, tr("CMake Scripts (*.cmake)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select CMake Script Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROGRAM_BUILDER_KEY_GROUP);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_USER_CMAKE).
                       toString());

                m_ui->cmakeScriptEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(BOARD_SETTINGS_KEY_OPEN_USER_FILE,
                filePath.canonicalFilePath());
            }
        }
    }

    if(messageBox.clickedButton() == systemButton)
    {
        QString openFile = settings.value(BOARD_SETTINGS_KEY_OPEN_SYSTEM_FILE).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select CMake Script"), openFile, tr("CMake Scripts (*.cmake)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select CMake Script Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROGRAM_BUILDER_KEY_GROUP);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_SYSTEM_CMAKE).
                       toString());

                m_ui->cmakeScriptEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(BOARD_SETTINGS_KEY_OPEN_SYSTEM_FILE,
                filePath.canonicalFilePath());
            }
        }
    }
}

void BoardSettingsDialog::accept()
{
    if(m_ui->boardNameEdit->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Editing Error"),
        tr("Board name is empty")); return;
    }

    if((!m_ui->advancedBox->isVisible())
    && m_ui->cmakeScriptEdit->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Editing Error"),
        tr("CMake script is empty")); return;
    }

    QString boardName = m_ui->boardNameEdit->text();
    QString cmakeScript = m_ui->cmakeScriptEdit->text();

    if(!m_ftdi->setDevice(boardName, cmakeScript))
    {
        QMessageBox::critical(this, tr("Editing Error"),
        m_ftdi->errorString());
        reject();
    }

    QDialog::accept();
}

void BoardSettingsDialog::reject()
{
    foreach(QObject *child, children())
    {
        QDialog *dialog = qobject_cast<QDialog*>(child);

        if(dialog)
        {
            dialog->reject();
        }
    }

    QDialog::reject();
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
