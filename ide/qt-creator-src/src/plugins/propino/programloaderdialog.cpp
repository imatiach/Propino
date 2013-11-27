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

#include "programloaderdialog.h"
#include "ui_programloaderdialog.h"

const QString ProgramLoaderDialog::m_protocol[] =
{
    "propeller1-shutdown",
    "propeller1-load-run",
    "propeller1-program-shutdown",
    "propeller1-program-run",
    "propeller1-s1",
    "propeller1-s1-s2"
};

const int ProgramLoaderDialog::m_hiddenThings[] =
{
    3, // binary = hide, stage1 = hide, stage2 = hide
    2, // binary = show, stage1 = hide, stage2 = hide
    2, // binary = show, stage1 = hide, stage2 = hide
    2, // binary = show, stage1 = hide, stage2 = hide
    1, // binary = show, stage1 = show, stage2 = hide
    0 // binary = show, stage1 = show, stage2 = show
};

ProgramLoaderDialog::ProgramLoaderDialog(QFtdi *ftdi, QSettings *settings,
                                         QWidget *parent,
                                         const QString &prefix,
                                         SerialEscape *escape) :
    QDialog(parent),
    m_ui(new Ui::ProgramLoaderDialog)
{
    m_ui->setupUi(this); Q_CHECK_PTR(ftdi); Q_CHECK_PTR(settings);

    m_ftdi = ftdi; m_settings = settings;
    m_prefix = prefix; m_escape = escape;

    QSettings i_settings(m_settings->fileName(),
                         m_settings->format());

    i_settings.beginGroup(PROPINO_KEY_GROUP);
    i_settings.beginGroup(m_prefix);

    QString loaderProtocol=i_settings.value(PROGRAM_BUILDER_KEY_LP).toString();

    for(size_t i = 0; i < (sizeof(m_protocol) / sizeof(QString)); i++)
    {
        if(loaderProtocol == m_protocol[i])
        {
            m_ui->loaderProtocolMenu->setCurrentIndex(i); break;
        }
    }

    currentIndexChanged(m_ui->loaderProtocolMenu->currentIndex());

    m_ui->binaryFileEdit->setText(
    i_settings.value(PROGRAM_BUILDER_KEY_BF).toString());
    m_ui->binaryFileEdit->selectAll();

    m_ui->stage1BinaryFileEdit->setText(
    i_settings.value(PROGRAM_BUILDER_KEY_S1BF).toString());
    m_ui->stage1BinaryFileEdit->selectAll();

    m_ui->stage2BinaryFileEdit->setText(
    i_settings.value(PROGRAM_BUILDER_KEY_S2BF).toString());
    m_ui->stage2BinaryFileEdit->selectAll();

    QFileIconProvider provider;

    m_ui->binaryFileButton->setIcon(
    provider.icon(QFileIconProvider::Folder));

    m_ui->stage1BinaryFileButton->setIcon(
    provider.icon(QFileIconProvider::Folder));

    m_ui->stage2BinaryFileButton->setIcon(
    provider.icon(QFileIconProvider::Folder));

    connect(m_ui->loaderProtocolMenu, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIndexChanged(int)));

    connect(m_ui->loadButton, SIGNAL(clicked()),
            this, SLOT(loadBinary()));

    connect(m_ui->binaryFileButton, SIGNAL(clicked()),
            this, SLOT(binaryButton()));

    connect(m_ui->stage1BinaryFileButton, SIGNAL(clicked()),
            this, SLOT(stage1Button()));

    connect(m_ui->stage2BinaryFileButton, SIGNAL(clicked()),
            this, SLOT(stage2Button()));

    connect(m_ui->saveButton, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(m_ui->cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(m_ftdi, SIGNAL(aboutToClose()),
            this, SLOT(reject()));

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog);
}

ProgramLoaderDialog::~ProgramLoaderDialog()
{
    delete m_ui;
}

void ProgramLoaderDialog::setLoadButtonVisibility(bool state)
{
    m_ui->loadButton->setVisible(state);
}

void ProgramLoaderDialog::currentIndexChanged(int index)
{
    m_ui->binaryFileLabel->setVisible(m_hiddenThings[index] < 3);
    m_ui->binaryFileEdit->setVisible(m_hiddenThings[index] < 3);
    m_ui->binaryFileButton->setVisible(m_hiddenThings[index] < 3);

    m_ui->stage1BinaryFileLabel->setVisible(m_hiddenThings[index] < 2);
    m_ui->stage1BinaryFileEdit->setVisible(m_hiddenThings[index] < 2);
    m_ui->stage1BinaryFileButton->setVisible(m_hiddenThings[index] < 2);

    m_ui->stage2BinaryFileLabel->setVisible(m_hiddenThings[index] < 1);
    m_ui->stage2BinaryFileEdit->setVisible(m_hiddenThings[index] < 1);
    m_ui->stage2BinaryFileButton->setVisible(m_hiddenThings[index] < 1);
}

void ProgramLoaderDialog::loadBinary(QProgressDialog *dialog,
                                     SerialLoader *loader)
{
    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 3)
    && m_ui->binaryFileEdit->text().isEmpty()) // can't do isVisible()
    {
        QMessageBox::critical(this, tr("Binary Loader"),
        tr("Binary file name empty")); return;
    }

    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 2)
    && m_ui->stage1BinaryFileEdit->text().isEmpty()) // can't do isVisible()
    {
        QMessageBox::critical(this, tr("Binary Loader"),
        tr("Stage 1 binary file name empty")); return;
    }

    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 1)
    && m_ui->stage2BinaryFileEdit->text().isEmpty()) // can't do isVisible()
    {
        QMessageBox::critical(this, tr("Binary Loader"),
        tr("Stage 2 binary file name empty")); return;
    }

    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString binaryFile = m_ui->binaryFileEdit->text();

    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 3)
    && (!QFileInfo(binaryFile).isAbsolute())) // can't do isVisible()
    {
        QDir dU(settings.value(PROGRAM_BUILDER_KEY_USER_BIN).toString());
        QDir dS(settings.value(PROGRAM_BUILDER_KEY_SYSTEM_BIN).toString());

        if(dU.exists(binaryFile))
        {
            binaryFile = dU.absoluteFilePath(binaryFile);
        }
        else if(dS.exists(binaryFile))
        {
            binaryFile = dS.absoluteFilePath(binaryFile);
        }
        else
        {
            QMessageBox box(QMessageBox::Critical, tr("Binary Loader"),
            tr("Unable to find binary file \"%1\"").
            arg(binaryFile), QMessageBox::Ok, this);

            QString errorMessage = tr("User Path:\n\n") +
            '\"' + dU.canonicalPath() + "\"\n\n" +
                                   tr("System Path:\n\n") +
            '\"' + dS.canonicalPath() + "\"";

            box.setDetailedText(errorMessage); box.exec(); return;
        }
    }

    QString stage1BinaryFile = m_ui->stage1BinaryFileEdit->text();

    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 2)
    && (!QFileInfo(stage1BinaryFile).isAbsolute())) // can't do isVisible()
    {
        QDir dU(settings.value(PROGRAM_BUILDER_KEY_USER_S1BIN).toString());
        QDir dS(settings.value(PROGRAM_BUILDER_KEY_SYSTEM_S1BIN).toString());

        if(dU.exists(stage1BinaryFile))
        {
            stage1BinaryFile = dU.absoluteFilePath(stage1BinaryFile);
        }
        else if(dS.exists(stage1BinaryFile))
        {
            stage1BinaryFile = dS.absoluteFilePath(stage1BinaryFile);
        }
        else
        {
            QMessageBox box(QMessageBox::Critical, tr("Binary Loader"),
            tr("Unable to find stage 1 binary file \"%1\"").
            arg(stage1BinaryFile), QMessageBox::Ok, this);

            QString errorMessage = tr("User Path:\n\n") +
            '\"' + dU.canonicalPath() + "\"\n\n" +
                                   tr("System Path:\n\n") +
            '\"' + dS.canonicalPath() + "\"";

            box.setDetailedText(errorMessage); box.exec(); return;
        }
    }

    QString stage2BinaryFile = m_ui->stage2BinaryFileEdit->text();

    if((m_hiddenThings[m_ui->loaderProtocolMenu->currentIndex()] < 1)
    && (!QFileInfo(stage2BinaryFile).isAbsolute())) // can't do isVisible()
    {
        QDir dU(settings.value(PROGRAM_BUILDER_KEY_USER_S2BIN).toString());
        QDir dS(settings.value(PROGRAM_BUILDER_KEY_SYSTEM_S2BIN).toString());

        if(dU.exists(stage2BinaryFile))
        {
            stage2BinaryFile = dU.absoluteFilePath(stage2BinaryFile);
        }
        else if(dS.exists(stage2BinaryFile))
        {
            stage2BinaryFile = dS.absoluteFilePath(stage2BinaryFile);
        }
        else
        {
            QMessageBox box(QMessageBox::Critical, tr("Binary Loader"),
            tr("Unable to find stage 2 binary file \"%1\"").
            arg(stage2BinaryFile), QMessageBox::Ok, this);

            QString errorMessage = tr("User Path:\n\n") +
            '\"' + dU.canonicalPath() + "\"\n\n" +
                                   tr("System Path:\n\n") +
            '\"' + dS.canonicalPath() + "\"";

            box.setDetailedText(errorMessage); box.exec(); return;
        }
    }

    QProgressDialog *pDialog; SerialLoader *pLoad;

    if(dialog)
    {
        pDialog = dialog;
    }
    else
    {
        pDialog = new QProgressDialog(this);
    }

    if(loader)
    {
        pLoad = loader;
    }
    else
    {
        pLoad = new SerialLoader(m_ftdi, pDialog);
    }

    if(m_escape)
    {
        m_escape->setEnabled(false);
    }

    if(pLoad->loadBinary(m_protocol[m_ui->loaderProtocolMenu->currentIndex()],
       binaryFile, stage1BinaryFile, stage2BinaryFile))
    {
        if(!dialog) { pDialog->close(); } accept();
    }

    if(m_escape)
    {
        m_escape->setEnabled(true);
    }

    if(!loader)
    {
        delete pLoad;
    }

    if(!dialog)
    {
        delete pDialog;
    }
}

void ProgramLoaderDialog::binaryButton()
{
    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(PROGRAM_LOADER_KEY_GROUP);

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
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_USER_F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_USER_BIN).
                       toString());

                m_ui->binaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_F,
                filePath.canonicalFilePath());
            }
        }
    }

    if(messageBox.clickedButton() == systemButton)
    {
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_SYSTEM_F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_SYSTEM_BIN).
                       toString());

                m_ui->binaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_F,
                filePath.canonicalFilePath());
            }
        }
    }
}

void ProgramLoaderDialog::stage1Button()
{
    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(PROGRAM_LOADER_KEY_GROUP);

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
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_USER_S1F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Stage 1 Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Stage 1 Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_USER_S1BIN).
                       toString());

                m_ui->stage1BinaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_S1F,
                filePath.canonicalFilePath());
            }
        }
    }

    if(messageBox.clickedButton() == systemButton)
    {
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S1F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Stage 1 Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Stage 1 Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_SYSTEM_S1BIN).
                       toString());

                m_ui->stage1BinaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S1F,
                filePath.canonicalFilePath());
            }
        }
    }
}

void ProgramLoaderDialog::stage2Button()
{
    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(PROGRAM_LOADER_KEY_GROUP);

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
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_USER_S2F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Stage 2 Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Stage 2 Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_USER_S2BIN).
                       toString());

                m_ui->stage2BinaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_S2F,
                filePath.canonicalFilePath());
            }
        }
    }

    if(messageBox.clickedButton() == systemButton)
    {
        QString openFile = settings.value(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S2F).
                           toString();

        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Stage 2 Binary"), openFile, tr("All Files (*)"));

        if(!fileName.isNull())
        {
            if(fileName.isEmpty())
            {
                QMessageBox::critical(this, tr("Select Stage 2 Binary Error"),
                tr("Invalid empty filename string"));
            }
            else
            {
                QFileInfo filePath(fileName);
                QSettings i_settings(m_settings->fileName(),
                                     m_settings->format());

                i_settings.beginGroup(PROPINO_KEY_GROUP);
                i_settings.beginGroup(m_prefix);

                QDir d(i_settings.value(PROGRAM_BUILDER_KEY_SYSTEM_S2BIN).
                       toString());

                m_ui->stage2BinaryFileEdit->setText(d.
                relativeFilePath(filePath.canonicalFilePath()));

                settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S2F,
                filePath.canonicalFilePath());
            }
        }
    }
}

void ProgramLoaderDialog::accept()
{
    QSettings settings(m_settings->fileName(),
                       m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    settings.setValue(PROGRAM_BUILDER_KEY_LP,
    m_protocol[m_ui->loaderProtocolMenu->currentIndex()]);

    settings.setValue(PROGRAM_BUILDER_KEY_BF,
    m_ui->binaryFileEdit->text());

    settings.setValue(PROGRAM_BUILDER_KEY_S1BF,
    m_ui->stage1BinaryFileEdit->text());

    settings.setValue(PROGRAM_BUILDER_KEY_S2BF,
    m_ui->stage2BinaryFileEdit->text());

    QDialog::accept();
}

void ProgramLoaderDialog::reject()
{
    foreach(QObject *child, children())
    {
        QDialog *childDialog = qobject_cast<QDialog*>(child);

        if(childDialog)
        {
            childDialog->reject();
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
