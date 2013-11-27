/***************************************************************************//**
* @file
* Serial Terminal
*
* @version @n 1.0
* @date @n 10/17/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/17/2013
*******************************************************************************/

#include "serialterminal.h"
#include "ui_serialterminal.h"

SerialTerminal::SerialTerminal(const QString &title,
                               QSettings *settings,
                               QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::SerialTerminal)
{
    Q_CHECK_PTR(settings);

    m_ui->setupUi(this); setWindowTitle(title); m_settings = settings;
    m_ui->statusBar->hide(); // Default style doesn't look quite right...

    m_ui->rx->setMaximumBlockCount(MAX_LINE_SIZE);

    m_textCursor = m_ui->rx->textCursor(); m_brushColor = Qt::black;
    m_stateMachine = ASCII; m_shiftReg = QByteArray(); m_tabWidth = TAB_WIDTH;

    connect(m_ui->actionClear_Screen, SIGNAL(triggered()),
            this, SLOT(clearScreen()));

    connect(m_ui->actionSave_Text_File, SIGNAL(triggered()),
            this, SLOT(saveTextFile()));

    connect(m_ui->actionSend_Text_File, SIGNAL(triggered()),
            this, SLOT(sendTextFile()));

    connect(m_ui->actionClose_Window, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_ui->actionGeneral_Help, SIGNAL(triggered()),
            this, SLOT(generalHelp()));

    connect(m_ui->actionTerminal_Help, SIGNAL(triggered()),
            this, SLOT(terminalHelp()));

    connect(m_ui->tx, SIGNAL(returnPressed()),
            this, SLOT(returnPressed()));

    QSettings i_settings(m_settings->fileName(), m_settings->format());

    i_settings.beginGroup(SERIAL_TERMINAL_KEY_GROUP);
    i_settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    if(i_settings.contains(SERIAL_TERMINAL_KEY_STATE))
    {
        restoreState(i_settings.value(SERIAL_TERMINAL_KEY_STATE).
                     toByteArray());
    }

    if(i_settings.contains(SERIAL_TERMINAL_KEY_GEOMETRY))
    {
        restoreGeometry(i_settings.value(SERIAL_TERMINAL_KEY_GEOMETRY).
                        toByteArray());
    }
}

SerialTerminal::~SerialTerminal()
{
    delete m_ui;
}

void SerialTerminal::closeEvent(QCloseEvent *event)
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

    settings.beginGroup(SERIAL_TERMINAL_KEY_GROUP);
    settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    if(isVisible())
    {
        settings.setValue(SERIAL_TERMINAL_KEY_STATE,
                          saveState());
    }

    if(isVisible())
    {
        settings.setValue(SERIAL_TERMINAL_KEY_GEOMETRY,
                          saveGeometry());
    }

    QWidget::closeEvent(event);
}

void SerialTerminal::receive(const QByteArray &bytes)
{
    QByteArray buffer; int column = m_textCursor.columnNumber();

    for(int i = 0; i < bytes.size(); i++)
    {
        if((m_stateMachine == UTF8) && ((bytes.at(i) & 0xC0) != 0x80))
        {
            m_stateMachine = ASCII;
        }

        switch(m_stateMachine)
        {
            case ASCII:
            {
                if(((bytes.at(i) & 0xE0) == 0xC0)
                || ((bytes.at(i) & 0xF0) == 0xE0)
                || ((bytes.at(i) & 0xF8) == 0xF0)
                || ((bytes.at(i) & 0xFC) == 0xF8)
                || ((bytes.at(i) & 0xFE) == 0xFC)) // UTF8
                {
                    m_stateMachine = UTF8;

                    m_shiftReg.clear();
                }
                else if(!(bytes.at(i) & 128)) // ASCII
                {
                    switch(bytes.at(i))
                    {
                        case CC_NULL: break; // Null

                        case CC_HOME_CURSOR: // Home Cursor
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::Start);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_END_CURSOR: // End Cursor
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::End);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_CURSOR_LEFT: // Move Cursor Left
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::Left);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_CURSOR_RIGHT: // Move Cursor Right
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::Right);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_CURSOR_UP: // Move Cursor Up
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::Up);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_CURSOR_DOWN: // Move Cursor Down
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(QTextCursor::Down);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_BEEP_SPEAKER: // Beep Speaker
                        {
                            insertText(buffer); buffer.clear();

                            QApplication::beep(); break;
                        }

                        case CC_BACKSPACE: // Backspace
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.deletePreviousChar();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_TAB: // Tab
                        {
                            int j = m_tabWidth - (column % m_tabWidth);

                            while(j--)
                            {
                                buffer.append(' '); column++;
                            }

                            break;
                        }

                        case CC_LINE_FEED: // Line Feed
                        {
                            if(m_shiftReg.isEmpty()
                            || (!m_shiftReg.endsWith('\r')))
                            {
                                buffer.append('\n'); column = 0;
                            }

                            break;
                        }

                        case CC_CLEAR_TO_END_OF_LINE: // ... End of Line
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                            m_textCursor.removeSelectedText();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_CLEAR_LINES_BELOW: // Clear Lines Below
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::End, QTextCursor::KeepAnchor);
                            m_textCursor.removeSelectedText();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_CARRIAGE_RETURN: // Carriage Return
                        {
                            buffer.append('\n'); column = 0; break;
                        }

                        case CC_CLEAR_TO_START_OF_LINE: // ... Start of Line
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
                            m_textCursor.removeSelectedText();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_CLEAR_LINES_ABOVE: // Clear Lines Above
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::Start, QTextCursor::KeepAnchor);
                            m_textCursor.removeSelectedText();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_CLEAR_SCREEN: // Clear Screen
                        {
                            insertText(buffer); buffer.clear();

                            m_ui->rx->clear();
                            m_textCursor = m_ui->rx->textCursor();

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_XON: break; // Xon

                        case CC_SET_FOREGROUND_TO_GRAY: // ... Gray
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::gray; break;
                        }

                        case CC_XOFF: break; // Xoff

                        case CC_SET_FOREGROUND_TO_BLACK: // ... Black
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::black; break;
                        }

                        case CC_SET_FOREGROUND_TO_BLUE: // ... Blue
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::blue; break;
                        }

                        case CC_SET_FOREGROUND_TO_RED: // ... Red
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::red; break;
                        }

                        case CC_SET_FOREGROUND_TO_GREEN: // ... Green
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::green; break;
                        }

                        case CC_SET_FOREGROUND_TO_MAGENTA: // ... Magenta
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::magenta; break;
                        }

                        case CC_SET_FOREGROUND_TO_CYAN: // ... Cyan
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::cyan; break;
                        }

                        case CC_SET_FOREGROUND_TO_YELLOW: // ... Yellow
                        {
                            insertText(buffer); buffer.clear();

                            m_brushColor = Qt::yellow; break;
                        }

                        case CC_ESCAPE: break; // Escape

                        case CC_MOVE_TO_START_OF_LINE: // Start of Line
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::StartOfLine);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_TO_END_OF_LINE: // End of Line
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::EndOfLine);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_TO_PREVIOUS_START_OF_LINE: // Previous ...
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::PreviousBlock);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_MOVE_TO_NEXT_START_OF_LINE: // Next ...
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.movePosition(
                            QTextCursor::NextBlock);
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        case CC_DELETE: // Delete
                        {
                            insertText(buffer); buffer.clear();

                            m_textCursor.deleteChar();
                            m_ui->rx->setTextCursor(m_textCursor);

                            column = m_textCursor.columnNumber(); break;
                        }

                        default: // Printable
                        {
                            buffer.append(bytes.at(i)); column++; break;
                        }
                    }
                }

                break;
            }

            case UTF8:
            {
                if((((m_shiftReg.at(0)&0xE0)==0xC0)&&(m_shiftReg.size()>=1))
                || (((m_shiftReg.at(0)&0xF0)==0xE0)&&(m_shiftReg.size()>=2))
                || (((m_shiftReg.at(0)&0xF8)==0xF0)&&(m_shiftReg.size()>=3))
                || (((m_shiftReg.at(0)&0xFC)==0xF8)&&(m_shiftReg.size()>=4))
                || (((m_shiftReg.at(0)&0xFE)==0xFC)&&(m_shiftReg.size()>=5)))
                {
                    buffer.append(m_shiftReg + bytes.at(i)); column++;

                    m_stateMachine = ASCII;
                }

                break;
            }
        }

        if(m_shiftReg.append(bytes.at(i)).size() > SHIFT_REG)
        {
            m_shiftReg = m_shiftReg.right(SHIFT_REG);
        }
    }

    insertText(buffer); buffer.clear();
}

void SerialTerminal::saveTextFile()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(SERIAL_TERMINAL_KEY_GROUP);
    settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    QString saveFile = settings.value(SERIAL_TERMINAL_KEY_SAVE_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Text File"),
    saveFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save Text File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QByteArray utf8 = m_ui->rx->toPlainText().toUtf8();

                if(file.write(utf8) == utf8.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(SERIAL_TERMINAL_KEY_SAVE_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Save Text File Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Save Text File Error"),
                file.errorString());
            }
        }
    }
}

void SerialTerminal::sendTextFile()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(SERIAL_TERMINAL_KEY_GROUP);
    settings.beginGroup(windowTitle().replace(QRegExp("[\\/]"), " "));

    QString openFile = settings.value(SERIAL_TERMINAL_KEY_OPEN_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Send Text File"),
    openFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Send Text File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFileInfo fileInfo(fileName);

            if(fileInfo.size() <= MAX_FILE_SIZE)
            {
                QFile file(fileName);

                if(file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    emit transmit(file.readAll());

                    settings.setValue(SERIAL_TERMINAL_KEY_OPEN_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Send Text File Error"),
                    file.errorString());
                }
            }
            else
            {
                QString title = tr("Send Text File Error"); QString text;

                if(MAX_FILE_SIZE >= ONE_GIGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln GB(s)",
                    "", MAX_FILE_SIZE / ONE_GIGABYTE);
                }
                else if(MAX_FILE_SIZE >= ONE_MEGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln MB(s)",
                    "", MAX_FILE_SIZE / ONE_MEGABYTE);
                }
                else if(MAX_FILE_SIZE >= ONE_KILOBYTE)
                {
                    text = tr("The maximum supported file size is %Ln KB(s)",
                    "", MAX_FILE_SIZE / ONE_KILOBYTE);
                }
                else // if(MAX_FILE_SIZE >= ONE_BYTE)
                {
                    text = tr("The maximum supported file size is %Ln byte(s)",
                    "", MAX_FILE_SIZE / ONE_BYTE);
                }

                QMessageBox::critical(this, title, text);
            }
        }
    }
}

void SerialTerminal::generalHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open General Help Error"),
        tr("Unable to open the URL to the Propino General Help page"));
    }
}

void SerialTerminal::terminalHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open Terminal Help Error"),
        tr("Unable to open the URL to the Propino Terminal Help page"));
    }
}

void SerialTerminal::clearScreen()
{
    m_ui->rx->clear();
    m_textCursor = m_ui->rx->textCursor();
}

void SerialTerminal::returnPressed()
{
    QString text = m_ui->tx->text() + '\n';

    emit transmit(text.toUtf8());

    insertText(text);

    m_ui->tx->clear();
}

void SerialTerminal::insertText(const QString &text)
{
    if(!text.isEmpty())
    {
        QTextCharFormat format = m_textCursor.charFormat();
        format.setForeground(m_brushColor);
        m_textCursor.setCharFormat(format);

        m_textCursor.insertText(text);
        m_ui->rx->setTextCursor(m_textCursor);
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
