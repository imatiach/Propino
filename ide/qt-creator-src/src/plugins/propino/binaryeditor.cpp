/***************************************************************************//**
* @file
* Binary Editor
*
* @version @n 1.0
* @date @n 11/9/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 11/9/2013
*******************************************************************************/

#include "binaryeditor.h"
#include "ui_binaryeditor.h"

using namespace ExtensionSystem;

using namespace BINEditor;

BinaryEditor::BinaryEditor(const QString &title, QSettings *settings,
                           QWidget *parent, const QString &prefix) :
    QMainWindow(parent),
    m_ui(new Ui::BinaryEditor)
{
    Q_CHECK_PTR(settings);

    m_ui->setupUi(this); setWindowTitle(title); m_settings = settings;

    m_prefix = prefix;

    m_ui->actionFirst_Difference->setEnabled(false);
    m_ui->actionNext_Difference->setEnabled(false);
    m_ui->actionPrevious_Difference->setEnabled(false);
    m_ui->actionLast_Difference->setEnabled(false);
    m_ui->actionClose->setEnabled(false);

    // Create Hex Edit Widget
    {
        QObject *binEditorWidgetFactory = PluginManager::instance()->
        getObjectByClassName("BINEditor::BinEditorWidgetFactory");
        Q_CHECK_PTR(binEditorWidgetFactory);

        QWidget *binEditorWidget = invoke<QWidget*>(binEditorWidgetFactory,
        "createWidget", m_ui->holderWidget); Q_CHECK_PTR(binEditorWidget);

        m_ui->editFrameLayout->addWidget(binEditorWidget);
        m_ui->editFrame->show();

        m_hexEdit = static_cast<BinEditorWidget*>(binEditorWidget);
        m_hexEdit->setReadOnly(false);

        m_hexEdit->viewport()->installEventFilter(this);
        m_hexEdit->installEventFilter(this);
    }

    // Create Hex Diff Widget
    {
        QObject *binEditorWidgetFactory = PluginManager::instance()->
        getObjectByClassName("BINEditor::BinEditorWidgetFactory");
        Q_CHECK_PTR(binEditorWidgetFactory);

        QWidget *binEditorWidget = invoke<QWidget*>(binEditorWidgetFactory,
        "createWidget", m_ui->holderWidget); Q_CHECK_PTR(binEditorWidget);

        m_ui->diffFrameLayout->addWidget(binEditorWidget);
        m_ui->diffFrame->hide();

        m_hexDiff = static_cast<BinEditorWidget*>(binEditorWidget);
        m_hexDiff->setReadOnly(true);

        m_hexDiff->viewport()->installEventFilter(this);
        m_hexDiff->installEventFilter(this);
    }

    m_ui->statusBar->addPermanentWidget(m_positionLabel = new QLabel(), 2);
    m_ui->statusBar->addPermanentWidget(m_offsetLabel = new QLabel(), 1);
    m_ui->statusBar->addPermanentWidget(m_sizeLabel = new QLabel(), 1);

    m_hexThis = m_hexEdit; m_hexThisSize = 1;

    m_finder = new BinaryFinder(m_settings, this, m_prefix);
    m_ui->centralWidgetLayout->addWidget(m_finder); m_finder->hide();

    m_diffCursorPosition = m_hexThis->cursorPosition();
    m_diffMarkupDown.clear(); m_diffLastDownIndex = -1;
    m_diffMarkupUp.clear(); m_diffLastUpIndex = -1;

    m_ftdi = NULL;
    m_dialog = NULL;

    m_maxBaudRate = 0;
    m_maxBlockSize = 0;
    m_memoryOffset = 0;
    m_memorySize = 0;

    m_diffMemoryOffset = 0;
    m_diffMemorySize = 0;

    connect(m_hexEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_hexDiff->verticalScrollBar(), SLOT(setValue(int)));

    connect(m_hexEdit->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            m_hexDiff->horizontalScrollBar(), SLOT(setValue(int)));

    connect(m_hexDiff->verticalScrollBar(), SIGNAL(valueChanged(int)),
            m_hexEdit->verticalScrollBar(), SLOT(setValue(int)));

    connect(m_hexDiff->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            m_hexEdit->horizontalScrollBar(), SLOT(setValue(int)));

    connect(m_hexEdit, SIGNAL(cursorPositionChanged(int)),
            this, SLOT(cursorPositionChanged(int)));

    connect(m_hexDiff, SIGNAL(cursorPositionChanged(int)),
            this, SLOT(cursorPositionChanged(int)));

    connect(m_ui->actionInsert_File, SIGNAL(triggered()),
            this, SLOT(insertFile()));

    connect(m_ui->actionSave, SIGNAL(triggered()),
            this, SLOT(save()));

    connect(m_ui->actionSave_Selection, SIGNAL(triggered()),
            this, SLOT(saveSelection()));

    connect(m_ui->actionASCII_To_Clipboard, SIGNAL(triggered()),
            this, SLOT(exportASCIITextToClipboard()));

    connect(m_ui->actionASCII_To_File, SIGNAL(triggered()),
            this, SLOT(exportASCIITextToFile()));

    connect(m_ui->actionC_To_Clipboard, SIGNAL(triggered()),
            this, SLOT(exportCCodeToClipboard()));

    connect(m_ui->actionC_To_File, SIGNAL(triggered()),
            this, SLOT(exportCCodeToFile()));

    connect(m_ui->actionExit, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(m_ui->menuEdit, SIGNAL(aboutToShow()),
            this, SLOT(editAboutToShow()));

    connect(m_ui->actionUndo, SIGNAL(triggered()),
            this, SLOT(undoShortcut()));

    connect(m_ui->actionRedo, SIGNAL(triggered()),
            this, SLOT(redoShortcut()));

    connect(m_ui->actionCut, SIGNAL(triggered()),
            this, SLOT(cutShortcut()));

    connect(m_ui->actionCopy, SIGNAL(triggered()),
            this, SLOT(copyShortcut()));

    connect(m_ui->actionPaste, SIGNAL(triggered()),
            this, SLOT(pasteShortcut()));

    connect(m_ui->actionDelete, SIGNAL(triggered()),
            this, SLOT(deleteShortcut()));

    connect(m_ui->actionSelect_All, SIGNAL(triggered()),
            this, SLOT(selectAllShortcut()));

    connect(m_ui->actionFind_Replace, SIGNAL(triggered()),
            m_finder, SLOT(show()));

    connect(m_ui->actionGoto_Offset, SIGNAL(triggered()),
            this, SLOT(gotoOffset()));

    connect(m_ui->actionRead_Memory, SIGNAL(triggered()),
            this, SLOT(readMemory()));

    connect(m_ui->actionWrite_Memory, SIGNAL(triggered()),
            this, SLOT(writeMemory()));

    connect(m_ui->actionErase_Memory, SIGNAL(triggered()),
            this, SLOT(eraseMemory()));

    connect(m_ui->action8_Bit_Checksum, SIGNAL(triggered()),
            this, SLOT(checksum8Bit()));

    connect(m_ui->action16_Bit_Checksum, SIGNAL(triggered()),
            this, SLOT(checksum16Bit()));

    connect(m_ui->action32_Bit_Checksum, SIGNAL(triggered()),
            this, SLOT(checksum32Bit()));

    connect(m_ui->action64_Bit_Checksum, SIGNAL(triggered()),
            this, SLOT(checksum64Bit()));

    connect(m_ui->actionCRC_16_CCITT_Checksum, SIGNAL(triggered()),
            this, SLOT(CRC16CCITTChecksum()));

    connect(m_ui->actionMD4_Hash, SIGNAL(triggered()),
            this, SLOT(MD4Hash()));

    connect(m_ui->actionMD5_Hash, SIGNAL(triggered()),
            this, SLOT(MD5Hash()));

    connect(m_ui->actionSHA1_Hash, SIGNAL(triggered()),
            this, SLOT(SHA1Hash()));

    connect(m_ui->actionSHA224_Hash, SIGNAL(triggered()),
            this, SLOT(SHA224Hash()));

    connect(m_ui->actionSHA256_Hash, SIGNAL(triggered()),
            this, SLOT(SHA256Hash()));

    connect(m_ui->actionSHA384_Hash, SIGNAL(triggered()),
            this, SLOT(SHA384Hash()));

    connect(m_ui->actionSHA512_Hash, SIGNAL(triggered()),
            this, SLOT(SHA512Hash()));

    connect(m_ui->action8_Bit_Unsigned_Hex, SIGNAL(triggered()),
            this, SLOT(hex8BitUnsigned()));

    connect(m_ui->action8_Bit_Signed_Hex, SIGNAL(triggered()),
            this, SLOT(hex8BitSigned()));

    connect(m_ui->action16_Bit_Unsigned_Hex, SIGNAL(triggered()),
            this, SLOT(hex16BitUnsigned()));

    connect(m_ui->action16_Bit_Signed_Hex, SIGNAL(triggered()),
            this, SLOT(hex16BitSigned()));

    connect(m_ui->action32_Bit_Unsigned_Hex, SIGNAL(triggered()),
            this, SLOT(hex32BitUnsigned()));

    connect(m_ui->action32_Bit_Signed_Hex, SIGNAL(triggered()),
            this, SLOT(hex32BitSigned()));

    connect(m_ui->action64_Bit_Unsigned_Hex, SIGNAL(triggered()),
            this, SLOT(hex64BitUnsigned()));

    connect(m_ui->action64_Bit_Signed_Hex, SIGNAL(triggered()),
            this, SLOT(hex64BitSigned()));

    connect(m_ui->actionFloat_Hex, SIGNAL(triggered()),
            this, SLOT(hexFloat()));

    connect(m_ui->actionDouble_Hex, SIGNAL(triggered()),
            this, SLOT(hexDouble()));

    connect(m_ui->action8_Bit_Unsigned_Num, SIGNAL(triggered()),
            this, SLOT(num8BitUnsigned()));

    connect(m_ui->action8_Bit_Signed_Num, SIGNAL(triggered()),
            this, SLOT(num8BitSigned()));

    connect(m_ui->action16_Bit_Unsigned_Num, SIGNAL(triggered()),
            this, SLOT(num16BitUnsigned()));

    connect(m_ui->action16_Bit_Signed_Num, SIGNAL(triggered()),
            this, SLOT(num16BitSigned()));

    connect(m_ui->action32_Bit_Unsigned_Num, SIGNAL(triggered()),
            this, SLOT(num32BitUnsigned()));

    connect(m_ui->action32_Bit_Signed_Num, SIGNAL(triggered()),
            this, SLOT(num32BitSigned()));

    connect(m_ui->action64_Bit_Unsigned_Num, SIGNAL(triggered()),
            this, SLOT(num64BitUnsigned()));

    connect(m_ui->action64_Bit_Signed_Num, SIGNAL(triggered()),
            this, SLOT(num64BitSigned()));

    connect(m_ui->actionFloat_Num, SIGNAL(triggered()),
            this, SLOT(numFloat()));

    connect(m_ui->actionDouble_Num, SIGNAL(triggered()),
            this, SLOT(numDouble()));

    connect(m_ui->actionCompare, SIGNAL(triggered()),
            this, SLOT(compareFiles()));

    connect(m_ui->actionFirst_Difference, SIGNAL(triggered()),
            this, SLOT(firstDifference()));

    connect(m_ui->actionNext_Difference, SIGNAL(triggered()),
            this, SLOT(nextDifference()));

    connect(m_ui->actionPrevious_Difference, SIGNAL(triggered()),
            this, SLOT(previousDifference()));

    connect(m_ui->actionLast_Difference, SIGNAL(triggered()),
            this, SLOT(lastDifference()));

    connect(m_ui->actionClose, SIGNAL(triggered()),
            this, SLOT(closeCompare()));

    connect(m_ui->actionGeneral_Help, SIGNAL(triggered()),
            this, SLOT(generalHelp()));

    connect(m_ui->actionEditor_Help, SIGNAL(triggered()),
            this, SLOT(editorHelp()));

    QSettings i_settings(m_settings->fileName(), m_settings->format());

    i_settings.beginGroup(PROPINO_KEY_GROUP);
    i_settings.beginGroup(m_prefix);

    if(i_settings.contains(BINARY_EDITOR_KEY_STATE))
    {
        restoreState(i_settings.value(BINARY_EDITOR_KEY_STATE).
                     toByteArray());
    }

    if(i_settings.contains(BINARY_EDITOR_KEY_GEOMETRY))
    {
        restoreGeometry(i_settings.value(BINARY_EDITOR_KEY_GEOMETRY).
                        toByteArray());
    }
}

BinaryEditor::~BinaryEditor()
{
    delete m_ui;
}

void BinaryEditor::warn()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    if(!settings.value(BINARY_EDITOR_KEY_WARNING_MESSAGE, false).toBool())
    {
        QMessageBox mb(QMessageBox::Warning, windowTitle(),
        tr("The binary editor backend is designed primarily for viewing "
           "binary files and because of this, undo and redo operate one "
           "character at a time... among other known problems... If you need "
           "to edit a large amount data please download a fully fledged "
           "binary editor. These limitations will be fixed in the future when "
           "the Qt-Creator backend is upgraded."), QMessageBox::Ok, this);

        QCheckBox *c = new QCheckBox(tr("Do not show me this again"));
        c->setChecked(true);

        QGridLayout *l = q_check_ptr(qobject_cast<QGridLayout*>(mb.layout()));
        l->addWidget(c, 1, 1, 1, 1); mb.exec();

        settings.setValue(BINARY_EDITOR_KEY_WARNING_MESSAGE, c->isChecked());
    }
}

void BinaryEditor::closeEvent(QCloseEvent *event)
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

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    settings.setValue(BINARY_EDITOR_KEY_STATE, saveState());
    settings.setValue(BINARY_EDITOR_KEY_GEOMETRY, saveGeometry());

    QWidget::closeEvent(event); emit closed();
}

bool BinaryEditor::eventFilter(QObject *object, QEvent *event)
{
    if((object == m_hexEdit)
    || (object == m_hexDiff)
    || (object == m_hexEdit->viewport())
    || (object == m_hexDiff->viewport()))
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if(keyEvent == QKeySequence::ZoomIn)
            {
                m_hexThis->zoomIn(); return true;
            }

            if(keyEvent == QKeySequence::ZoomOut)
            {
                m_hexThis->zoomOut(); return true;
            }

            if(keyEvent->key() == Qt::Key_Up) // bugfix workaround
            {
                BINEditor::BinEditorWidget::MoveMode moveMode =
                keyEvent->modifiers() & Qt::ShiftModifier ?
                BINEditor::BinEditorWidget::KeepAnchor :
                BINEditor::BinEditorWidget::MoveAnchor;

                int position = m_hexThis->cursorPosition() - 16;

                if(position >= 0)
                {
                    m_hexThis->setCursorPosition(position, moveMode);
                }

                return true;
            }

            if(keyEvent->key() == Qt::Key_Down) // bugfix workaround
            {
                BINEditor::BinEditorWidget::MoveMode moveMode =
                keyEvent->modifiers() & Qt::ShiftModifier ?
                BINEditor::BinEditorWidget::KeepAnchor :
                BINEditor::BinEditorWidget::MoveAnchor;

                int position = m_hexThis->cursorPosition() + 16;

                if(position <= (m_hexThisSize - 1))
                {
                    m_hexThis->setCursorPosition(position, moveMode);
                }

                return true;
            }

            if(!m_hexThis->isReadOnly())
            {
                if(keyEvent->key() == Qt::Key_Insert)
                {
                    QMessageBox::information(this, tr("Binary Editor"),
                    tr("Insert mode isn't supported"));

                    return true;
                }

                if(keyEvent->key() == Qt::Key_Delete)
                {
                    int position = m_hexThis->selectionStart();
                    int size = m_hexThis->selectionEnd() -
                    m_hexThis->selectionStart() + 1;
                    QString zero(size * 2, '0');

                    m_hexThis->setCursorPosition(m_hexThis->selectionStart());

                    QApplication::setOverrideCursor(Qt::WaitCursor);
                    QKeyEvent e(QEvent::KeyPress, 0, 0, zero);
                    QApplication::sendEvent(m_hexThis, &e);
                    QApplication::restoreOverrideCursor();

                    m_hexThis->setCursorPosition(position - 0);

                    return true;
                }

                if(keyEvent->key() == Qt::Key_Backspace)
                {
                    int position = m_hexThis->selectionStart();
                    int size = m_hexThis->selectionEnd() -
                    m_hexThis->selectionStart() + 1;
                    QString zero(size * 2, '0');

                    m_hexThis->setCursorPosition(m_hexThis->selectionStart());

                    QApplication::setOverrideCursor(Qt::WaitCursor);
                    QKeyEvent e(QEvent::KeyPress, 0, 0, zero);
                    QApplication::sendEvent(m_hexThis, &e);
                    QApplication::restoreOverrideCursor();

                    m_hexThis->setCursorPosition(position - 1);

                    return true;
                }

                if(keyEvent == QKeySequence::Cut)
                {
                    copyShortcut(); deleteShortcut(); return true;
                }

                if(keyEvent == QKeySequence::Paste)
                {
                    QString text = QApplication::clipboard()->text();

                    m_hexThis->setCursorPosition(m_hexThis->selectionStart());

                    QApplication::setOverrideCursor(Qt::WaitCursor);
                    QKeyEvent e(QEvent::KeyPress, 0, 0, text);
                    QApplication::sendEvent(m_hexThis, &e);
                    QApplication::restoreOverrideCursor();

                    return true;
                }
            }
        }

        if(event->type() == QEvent::ContextMenu)
        {
            QMenu menu;

            if(!m_hexThis->isReadOnly())
            {
                menu.addAction(m_ui->actionUndo);
                menu.addAction(m_ui->actionRedo);
                menu.addSeparator();
                menu.addAction(m_ui->actionCut);
            }

            menu.addAction(m_ui->actionCopy);

            if(!m_hexThis->isReadOnly())
            {
                menu.addAction(m_ui->actionPaste);
                menu.addAction(m_ui->actionDelete);
            }

            menu.addSeparator();
            menu.addAction(m_ui->actionSelect_All);

            // Add ASCII export support
            {
                menu.addSeparator(); QAction *actionCopy =
                new QAction(tr("Copy as formatted ASCII Text"), &menu);

                connect(actionCopy, SIGNAL(triggered()),
                        this, SLOT(exportASCIITextToClipboard()));

                menu.addAction(actionCopy); QAction *actionSave =
                new QAction(tr("Save as formatted ASCII Text"), &menu);

                connect(actionSave, SIGNAL(triggered()),
                        this, SLOT(exportASCIITextToFile()));

                menu.addAction(actionSave);
            }

            // Add C export support
            {
                menu.addSeparator(); QAction *actionCopy =
                new QAction(tr("Copy as formatted C Code"), &menu);

                connect(actionCopy, SIGNAL(triggered()),
                        this, SLOT(exportCCodeToClipboard()));

                menu.addAction(actionCopy); QAction *actionSave =
                new QAction(tr("Save as formatted C Code"), &menu);

                connect(actionSave, SIGNAL(triggered()),
                        this, SLOT(exportCCodeToFile()));

                menu.addAction(actionSave);
            }

            menu.exec(static_cast<QContextMenuEvent*>(event)->globalPos());

            return true;
        }

        if(event->type() == QEvent::FocusIn)
        {
            if((m_hexThis == m_hexEdit) // prev == m_hexEdit
            && (object == m_hexDiff)) // next = m_hexDiff
            {
                m_hexThis = m_hexDiff; m_hexThisSize = m_diffMemorySize;
                emit hexEditChanged();

                m_diffCursorPosition = m_hexThis->cursorPosition();
                m_diffLastDownIndex = -1; m_diffLastUpIndex = -1;

                m_offsetLabel->setText(tr("Offset: ") + "0x" + QString("%1").
                arg(m_diffMemoryOffset, 0x08L, 0x10L, QLatin1Char('0')).
                toUpper() + tr("h (%L1 Bytes) ").arg(m_diffMemoryOffset));

                m_sizeLabel->setText(tr("Size: ") + "0x" + QString("%1").
                arg(m_diffMemorySize, 0x08L, 0x10L, QLatin1Char('0')).
                toUpper() + tr("h (%L1 Bytes) ").arg(m_diffMemorySize));
            }

            if((m_hexThis == m_hexDiff) // prev == m_hexDiff
            && (object == m_hexEdit)) // next = m_hexEdit
            {
                m_hexThis = m_hexEdit; m_hexThisSize = m_memorySize;
                emit hexEditChanged();

                m_diffCursorPosition = m_hexThis->cursorPosition();
                m_diffLastDownIndex = -1; m_diffLastUpIndex = -1;

                m_offsetLabel->setText(tr("Offset: ") + "0x" + QString("%1").
                arg(m_memoryOffset, 0x08L, 0x10L, QLatin1Char('0')).
                toUpper() + tr("h (%L1 Bytes) ").arg(m_memoryOffset));

                m_sizeLabel->setText(tr("Size: ") + "0x" + QString("%1").
                arg(m_memorySize, 0x08L, 0x10L, QLatin1Char('0')).
                toUpper() + tr("h (%L1 Bytes) ").arg(m_memorySize));
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
}

void BinaryEditor::insertFile()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Insert File"),
        tr("This binary editor is read only!")); return;
    }

    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString openFile = settings.value(BINARY_EDITOR_KEY_OPEN_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Insert File"),
    openFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Insert File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFileInfo fileInfo(fileName);

            if(fileInfo.size() <= MAX_MEM_SIZE)
            {
                QFile file(fileName);

                if(file.open(QIODevice::ReadOnly))
                {
                    QByteArray data = file.readAll();

                    settings.setValue(BINARY_EDITOR_KEY_OPEN_FILE,
                    fileInfo.canonicalFilePath());

                    switch(QMessageBox::question(this, tr("Insert File"),
                    tr("Would you like to replace the contents of the "
                       "binary editor with the selected file instead?"),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::Yes))
                    {
                        case QMessageBox::Yes:
                        {
                            closeCompare();

                            if(data.size() > m_hexThisSize)
                            {
                                data.resize(m_hexThisSize);
                            }
                            else
                            {
                                data.append(QByteArray(m_hexThisSize -
                                data.size(), '\0'));
                            }

                            setBinEditorData(m_hexThis, data);

                            emit hexEditChanged();

                            break;
                        }

                        case QMessageBox::No:
                        {
                            closeCompare();

                            QApplication::clipboard()->setText(data.toHex());

                            pasteShortcut();

                            emit hexEditChanged();

                            break;
                        }

                        default: break;
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("Insert File Error"),
                    file.errorString());
                }
            }
            else
            {
                QString title = tr("Insert File Error"); QString text;

                if(MAX_MEM_SIZE >= ONE_GIGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln GB(s)",
                    "", MAX_MEM_SIZE / ONE_GIGABYTE);
                }
                else if(MAX_MEM_SIZE >= ONE_MEGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln MB(s)",
                    "", MAX_MEM_SIZE / ONE_MEGABYTE);
                }
                else if(MAX_MEM_SIZE >= ONE_KILOBYTE)
                {
                    text = tr("The maximum supported file size is %Ln KB(s)",
                    "", MAX_MEM_SIZE / ONE_KILOBYTE);
                }
                else // if(MAX_MEM_SIZE >= ONE_BYTE)
                {
                    text = tr("The maximum supported file size is %Ln byte(s)",
                    "", MAX_MEM_SIZE / ONE_BYTE);
                }

                QMessageBox::critical(this, title, text);
            }
        }
    }
}

void BinaryEditor::save()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString saveFile = settings.value(BINARY_EDITOR_KEY_SAVE_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
    saveFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly))
            {
                QByteArray data = getBinEditorData(m_hexThis, m_hexThisSize);

                if(file.write(data) == data.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(BINARY_EDITOR_KEY_SAVE_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Save File Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Save File Error"),
                file.errorString());
            }
        }
    }
}

void BinaryEditor::saveSelection()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString saveFile = settings.value(BINARY_EDITOR_KEY_SAVE_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
    saveFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly))
            {
                copyShortcut(); QByteArray data =
                QByteArray::fromHex(QApplication::clipboard()->text().
                                    toUtf8());

                if(file.write(data) == data.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(BINARY_EDITOR_KEY_SAVE_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Save File Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Save File Error"),
                file.errorString());
            }
        }
    }
}

void BinaryEditor::exportASCIITextToClipboard()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int size = data.size();
    QString text;

    for(int i = 0; i < ((size + 15) / 16); i++)
    {
        for(int j = 0; j < 16; j++)
        {
            if(((i * 16) + j) >= size)
            {
                break;
            }

            text.append(QString("%1 ").arg(uchar(data.at
            ((i * 16) + j)), 0x02L, 0x10L, QLatin1Char('0')).toUpper());
        }

        text.remove(text.lastIndexOf(' '), 01).append('\n');
    }

    QApplication::clipboard()->setText(text);
}

void BinaryEditor::exportASCIITextToFile()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString saveFile = settings.value(BINARY_EDITOR_KEY_EX_A_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
    saveFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                exportASCIITextToClipboard();

                QString text = QApplication::clipboard()->text();

                if(file.write(text.toUtf8()) == text.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(BINARY_EDITOR_KEY_EX_A_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Save File Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Save File Error"),
                file.errorString());
            }
        }
    }
}

void BinaryEditor::exportCCodeToClipboard()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int size = data.size();
    QString text = "const unsigned char bytes[] =\n{\n";

    for(int i = 0; i < ((size + 7) / 8); i++)
    {
        text.append("   ");

        for(int j = 0; j < 8; j++)
        {
            if(((i * 8) + j) >= size)
            {
                break;
            }

            text.append(" 0x" + QString("%1,").arg(uchar(data.at
            ((i * 8) + j)), 0x02L, 0x10L, QLatin1Char('0')).toUpper());
        }

        text.append('\n');
    }

    text.remove(text.lastIndexOf(','), 1).append("};\n");

    QApplication::clipboard()->setText(text);
}

void BinaryEditor::exportCCodeToFile()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString saveFile = settings.value(BINARY_EDITOR_KEY_EX_C_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
    saveFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save File Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFile file(fileName);

            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                exportCCodeToClipboard();

                QString text = QApplication::clipboard()->text();

                if(file.write(text.toUtf8()) == text.size())
                {
                    QFileInfo fileInfo(fileName);

                    settings.setValue(BINARY_EDITOR_KEY_EX_C_FILE,
                    fileInfo.canonicalFilePath());
                }
                else
                {
                    QMessageBox::critical(this, tr("Save File Error"),
                    file.errorString());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Save File Error"),
                file.errorString());
            }
        }
    }
}

void BinaryEditor::editAboutToShow()
{
    m_ui->menuEdit->clear();

    if(!m_hexThis->isReadOnly())
    {
        m_ui->menuEdit->addAction(m_ui->actionUndo);
        m_ui->menuEdit->addAction(m_ui->actionRedo);
        m_ui->menuEdit->addSeparator();
        m_ui->menuEdit->addAction(m_ui->actionCut);
    }

    m_ui->menuEdit->addAction(m_ui->actionCopy);

    if(!m_hexThis->isReadOnly())
    {
        m_ui->menuEdit->addAction(m_ui->actionPaste);
        m_ui->menuEdit->addAction(m_ui->actionDelete);
    }

    m_ui->menuEdit->addSeparator();
    m_ui->menuEdit->addAction(m_ui->actionSelect_All);
    m_ui->menuEdit->addSeparator();
    m_ui->menuEdit->addAction(m_ui->actionFind_Replace);
    m_ui->menuEdit->addSeparator();
    m_ui->menuEdit->addAction(m_ui->actionGoto_Offset);
}

void BinaryEditor::undoShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_Z, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::redoShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_Y, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::cutShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_X, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::copyShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_C, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::pasteShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_V, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::deleteShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::selectAllShortcut()
{
    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_A, Qt::ControlModifier);
    QApplication::sendEvent(m_hexThis, &e);
}

void BinaryEditor::gotoOffset()
{
    bool ok = false; QString text = QInputDialog::getText(this,
    tr("Goto Offset"), tr("Please enter an Offset:"), QLineEdit::Normal,
    QString(), &ok, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        qulonglong offset = text.toULongLong(&ok, 0);

        if(ok)
        {
            if(offset > static_cast<qulonglong>(m_hexThisSize - 1))
            {
                QMessageBox::critical(this, tr("Goto Offset"),
                                      tr("Invalid Offset"));
            }
            else
            {
                m_hexThis->setCursorPosition(offset);
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Goto Offset"),
                                  tr("Invalid Offset"));
        }
    }
}

void BinaryEditor::checksum8Bit()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint8) - 1) /
    sizeof(quint8)) * sizeof(quint8)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    quint8 sum = 0; quint8 *ptr = reinterpret_cast<quint8*>(data.data());

    for(size_t i = 0; i < (data.size() / sizeof(quint8)); i++)
    {
        sum += ptr[i];
    }

    QString checksum = QString("%1").arg(sum,
    0x02L, 0x10L, QLatin1Char('0'));

    QMessageBox box(QMessageBox::Information, tr("Data Checksum"),
    tr("The 8-bit checksum is:        "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + checksum.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::checksum16Bit()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint16) - 1) /
    sizeof(quint16)) * sizeof(quint16)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    quint16 sum = 0; quint16 *ptr = reinterpret_cast<quint16*>(data.data());

    for(size_t i = 0; i < (data.size() / sizeof(quint16)); i++)
    {
        sum += ptr[i];
    }

    QString checksum = QString("%1").arg(sum,
    0x04L, 0x10L, QLatin1Char('0'));

    QMessageBox box(QMessageBox::Information, tr("Data Checksum"),
    tr("The 16-bit checksum is:       "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + checksum.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::checksum32Bit()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint32) - 1) /
    sizeof(quint32)) * sizeof(quint32)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    quint32 sum = 0; quint32 *ptr = reinterpret_cast<quint32*>(data.data());

    for(size_t i = 0; i < (data.size() / sizeof(quint32)); i++)
    {
        sum += ptr[i];
    }

    QString checksum = QString("%1").arg(sum,
    0x08L, 0x10L, QLatin1Char('0'));

    QMessageBox box(QMessageBox::Information, tr("Data Checksum"),
    tr("The 32-bit checksum is:       "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + checksum.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::checksum64Bit()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint64) - 1) /
    sizeof(quint64)) * sizeof(quint64)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    quint64 sum = 0; quint64 *ptr = reinterpret_cast<quint64*>(data.data());

    for(size_t i = 0; i < (data.size() / sizeof(quint64)); i++)
    {
        sum += ptr[i];
    }

    QString checksum = QString("%1").arg(sum,
    0x10L, 0x10L, QLatin1Char('0'));

    QMessageBox box(QMessageBox::Information, tr("Data Checksum"),
    tr("The 64-bit checksum is:       "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + checksum.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::CRC16CCITTChecksum()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString checksum = QString("%1").arg(qChecksum(data.constData(),
    data.size()), 0x04L, 0x10L, QLatin1Char('0'));

    QMessageBox box(QMessageBox::Information, tr("Data Checksum"),
    tr("The CRC-16-CCITT checksum is: "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + checksum.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::MD4Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Md4).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The MD4 hash is:              "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::MD5Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Md5).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The MD5 hash is:              "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::SHA1Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Sha1).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The SHA1 hash is:             "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::SHA224Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Sha224).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The SHA224 hash is:           "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::SHA256Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Sha256).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The SHA256 hash is:           "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::SHA384Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Sha384).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The SHA384 hash is:           "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::SHA512Hash()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    QString hash = QCryptographicHash::hash(data,
                   QCryptographicHash::Sha512).toHex();

    QMessageBox box(QMessageBox::Information, tr("Data Hash"),
    tr("The SHA512 hash is:           "), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit("0x" + hash.toUpper());
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex8BitUnsigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint8) - 1) /
    sizeof(quint8)) * sizeof(quint8)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const quint8 *output = reinterpret_cast<const quint8*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 8-bit unsigned number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex8BitSigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(qint8) - 1) /
    sizeof(qint8)) * sizeof(qint8)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const qint8 *output = reinterpret_cast<const qint8*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 8-bit signed number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex16BitUnsigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint16) - 1) /
    sizeof(quint16)) * sizeof(quint16)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const quint16 *output = reinterpret_cast<const quint16*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 16-bit unsigned number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex16BitSigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(qint16) - 1) /
    sizeof(qint16)) * sizeof(qint16)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const qint16 *output = reinterpret_cast<const qint16*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 16-bit signed number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex32BitUnsigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint32) - 1) /
    sizeof(quint32)) * sizeof(quint32)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const quint32 *output = reinterpret_cast<const quint32*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 32-bit unsigned number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex32BitSigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(qint32) - 1) /
    sizeof(qint32)) * sizeof(qint32)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const qint32 *output = reinterpret_cast<const qint32*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 32-bit signed number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex64BitUnsigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(quint64) - 1) /
    sizeof(quint64)) * sizeof(quint64)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const quint64 *output = reinterpret_cast<const quint64*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 64-bit unsigned number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hex64BitSigned()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(qint64) - 1) /
    sizeof(qint64)) * sizeof(qint64)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const qint64 *output = reinterpret_cast<const qint64*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The 64-bit signed number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hexFloat()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(float) - 1) /
    sizeof(float)) * sizeof(float)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const float *output = reinterpret_cast<const float*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The float number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::hexDouble()
{
    copyShortcut(); QByteArray data =
    QByteArray::fromHex(QApplication::clipboard()->text().toUtf8());

    int zeroPadding = (((data.size() + sizeof(double) - 1) /
    sizeof(double)) * sizeof(double)) - data.size();

    while(zeroPadding--)
    {
        data.append(char(0));
    }

    const double *output = reinterpret_cast<const double*>(data.constData());

    QMessageBox box(QMessageBox::Information, tr("Hex to Num"),
    tr("The double number is:"), QMessageBox::Ok, this);

    QLineEdit *edit = new QLineEdit(QString::number(*output));
    edit->setReadOnly(true); edit->setDragEnabled(true); edit->home(true);

    QGridLayout *layout=q_check_ptr(qobject_cast<QGridLayout*>(box.layout()));
    layout->addWidget(edit, 1, 1, 1, 1); box.exec();
}

void BinaryEditor::num8BitUnsigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 8-bit unsigned value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(quint8), 0); *(reinterpret_cast<quint8*>
        (output.data())) = text.toULongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num8BitSigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 8-bit signed value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(qint8), 0); *(reinterpret_cast<qint8*>
        (output.data())) = text.toLongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num16BitUnsigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 16-bit unsigned value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(quint16), 0); *(reinterpret_cast<quint16*>
        (output.data())) = text.toULongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num16BitSigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 16-bit signed value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(qint16), 0); *(reinterpret_cast<qint16*>
        (output.data())) = text.toLongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num32BitUnsigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 32-bit unsigned value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(quint32), 0); *(reinterpret_cast<quint32*>
        (output.data())) = text.toULongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num32BitSigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 32-bit signed value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(qint32), 0); *(reinterpret_cast<qint32*>
        (output.data())) = text.toLongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num64BitUnsigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 64-bit unsigned value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(quint64), 0); *(reinterpret_cast<quint64*>
        (output.data())) = text.toULongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::num64BitSigned()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the 64-bit signed value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(qint64), 0); *(reinterpret_cast<qint64*>
        (output.data())) = text.toLongLong(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::numFloat()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the float value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(float), 0); *(reinterpret_cast<float*>
        (output.data())) = text.toFloat(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::numDouble()
{
    if(m_hexThis->isReadOnly())
    {
        QMessageBox::critical(this, tr("Num to Hex"),
        tr("This binary editor is read only!")); return;
    }

    bool ok = false;

    QString text = QInputDialog::getText(this, tr("Num to Hex"),
    tr("Enter the double value:"), QLineEdit::Normal, QString(), &ok,
    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
    Qt::ImhPreferNumbers);

    if(ok)
    {
        QByteArray output(sizeof(double), 0); *(reinterpret_cast<double*>
        (output.data())) = text.toDouble(&ok);

        if(ok)
        {
            QApplication::clipboard()->setText(output.toHex());
            pasteShortcut();
        }
        else
        {
            QMessageBox::critical(this, tr("Num to Hex"),
                                  tr("Invalid input value"));
        }
    }
}

void BinaryEditor::compareFiles()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    QString openFile = settings.value(BINARY_EDITOR_KEY_DIFF_FILE,
                                      QDir::homePath()).toString();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Compare Files"),
    openFile, tr("All Files (*)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Compare Files Error"),
            tr("Invalid empty filename string"));
        }
        else
        {
            QFileInfo fileInfo(fileName);

            if(fileInfo.size() <= MAX_MEM_SIZE)
            {
                int offset = 0;

                if(fileInfo.size() > m_memorySize)
                {
                    int diff = fileInfo.size() - m_memorySize;

                    bool ok = false; offset = QInputDialog::getInt(this,
                    tr("Compare Files"), tr("Please choose a byte offset to "
                    "compare the files from"), 0, 0, diff, 1, &ok,
                    Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint |
                    Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

                    if(!ok)
                    {
                        return;
                    }
                }

                QFile file(fileName);

                if(file.open(QIODevice::ReadOnly))
                {
                    settings.setValue(BINARY_EDITOR_KEY_DIFF_FILE,
                    fileInfo.canonicalFilePath());

                    QByteArray data = file.readAll().mid(offset, m_memorySize);

                    m_diffMemoryOffset = offset;
                    m_diffMemorySize = data.size();

                    m_diffCursorPosition = 0;
                    m_diffMarkupDown.clear(); m_diffLastDownIndex = -1;
                    m_diffMarkupUp.clear(); m_diffLastUpIndex = -1;

                    m_hexDiff->setUpdatesEnabled(false);
                    m_hexDiff->setReadOnly(false);
                    setBinEditorData(m_hexDiff, data);
                    m_hexDiff->setReadOnly(true);

                    QProgressDialog progress(this,
                    Qt::MSWindowsFixedSizeDialogHint |
                    Qt::WindowTitleHint |
                    Qt::WindowSystemMenuHint |
                    Qt::WindowCloseButtonHint);

                    progress.setWindowModality(Qt::ApplicationModal);
                    progress.setLabelText(tr("Highlighting Differences..."));
                    progress.setWindowTitle(tr("Comparing Files"));
                    progress.setRange(0, 0);

                    progress.show();

                    QByteArray EData = getBinEditorData(m_hexEdit,
                                                        m_memorySize);

                    QByteArray DData = getBinEditorData(m_hexDiff,
                                                        m_diffMemorySize);

                    int size = qMin(EData.size(), DData.size()); int i = 0;

                    while(!progress.wasCanceled())
                    {
                        int j = i;

                        for(; (j < size) &&
                        (EData.at(j) == DData.at(j)); j++);

                        for(i = j; (j < size) &&
                        (EData.at(j) != DData.at(j)); j++);

                        if(i == j)
                        {
                            break;
                        }

                        BINEditor::Markup markup(i, j - i, QColor("#F99"));

                        m_diffMarkupDown.insert(i, markup);
                        m_diffMarkupUp.insert(-i, markup);

                        i = j;

                        QApplication::processEvents();
                    }

                    progress.hide();

                    m_ui->actionFirst_Difference->setEnabled(true);
                    m_ui->actionNext_Difference->setEnabled(true);
                    m_ui->actionPrevious_Difference->setEnabled(true);
                    m_ui->actionLast_Difference->setEnabled(true);
                    m_ui->actionClose->setEnabled(true);

                    m_hexEdit->setMarkup(m_diffMarkupDown.values());
                    m_hexDiff->setMarkup(m_diffMarkupDown.values());

                    m_ui->diffFrame->show();
                    m_hexDiff->setUpdatesEnabled(true);

                    if(m_diffMarkupDown.isEmpty())
                    {
                        QMessageBox::information(this, tr("Compare Files"),
                                                 tr("Both Files Match"));
                    }
                    else
                    {
                        QMessageBox::information(this, tr("Compare Files"),
                                                 tr("The Files Differ"));
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("Compare Files Error"),
                    file.errorString());
                }
            }
            else
            {
                QString title = tr("Compare Files Error"); QString text;

                if(MAX_MEM_SIZE >= ONE_GIGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln GB(s)",
                    "", MAX_MEM_SIZE / ONE_GIGABYTE);
                }
                else if(MAX_MEM_SIZE >= ONE_MEGABYTE)
                {
                    text = tr("The maximum supported file size is %Ln MB(s)",
                    "", MAX_MEM_SIZE / ONE_MEGABYTE);
                }
                else if(MAX_MEM_SIZE >= ONE_KILOBYTE)
                {
                    text = tr("The maximum supported file size is %Ln KB(s)",
                    "", MAX_MEM_SIZE / ONE_KILOBYTE);
                }
                else // if(MAX_MEM_SIZE >= ONE_BYTE)
                {
                    text = tr("The maximum supported file size is %Ln byte(s)",
                    "", MAX_MEM_SIZE / ONE_BYTE);
                }

                QMessageBox::critical(this, title, text);
            }
        }
    }
}

void BinaryEditor::firstDifference()
{
    if(m_diffMarkupDown.isEmpty())
    {
        QMessageBox::information(this, tr("Find Difference"),
        tr("No differences found")); return;
    }

    QMap<int, BINEditor::Markup>::const_iterator i=m_diffMarkupDown.begin()+0;

    m_hexEdit->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexEdit->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_hexDiff->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexDiff->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_diffLastDownIndex = (i.value().address + i.value().length) %
                          m_diffMemorySize;

    m_diffLastUpIndex = i.value().address - 1;

    while(m_diffLastUpIndex < 0)
    {
        m_diffLastUpIndex += m_diffMemorySize;
    }
}

void BinaryEditor::nextDifference()
{
    if(m_diffMarkupDown.isEmpty())
    {
        QMessageBox::information(this, tr("Find Difference"),
        tr("No differences found")); return;
    }

    QMap<int, BINEditor::Markup>::const_iterator i;

    if(m_diffLastDownIndex >= 0)
    {
        i = m_diffMarkupDown.lowerBound(m_diffLastDownIndex);
    }
    else
    {
        i = m_diffMarkupDown.lowerBound(m_hexThis->cursorPosition());

        if((m_hexThis->cursorPosition() < i.key())
        && (i != m_diffMarkupDown.begin())
        && ((i - 1).value().covers(m_hexThis->cursorPosition())))
        {
            i -= 1; // should select the current block
        }
    }

    if(i == m_diffMarkupDown.end()) // support wraparound
    {
        i = m_diffMarkupDown.lowerBound(0);
    }

    m_hexEdit->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexEdit->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_hexDiff->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexDiff->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_diffLastDownIndex = (i.value().address + i.value().length) %
                          m_diffMemorySize;

    m_diffLastUpIndex = i.value().address - 1;

    while(m_diffLastUpIndex < 0)
    {
        m_diffLastUpIndex += m_diffMemorySize;
    }
}

void BinaryEditor::previousDifference()
{
    if(m_diffMarkupUp.isEmpty())
    {
        QMessageBox::information(this, tr("Find Difference"),
        tr("No differences found")); return;
    }

    QMap<int, BINEditor::Markup>::const_iterator i;

    if(m_diffLastUpIndex >= 0)
    {
        i = m_diffMarkupUp.lowerBound(-(m_diffLastUpIndex));
    }
    else
    {
        i = m_diffMarkupUp.lowerBound(-(m_hexThis->cursorPosition()));

        if(((-m_hexThis->cursorPosition()) < i.key())
        && (i != m_diffMarkupUp.begin())
        && ((i - 1).value().covers(m_hexThis->cursorPosition())))
        {
            i -= 1; // should select the current block
        }
    }

    if(i == m_diffMarkupUp.end()) // support wraparound
    {
        i = m_diffMarkupUp.lowerBound(-(m_diffMemorySize - 1));
    }

    m_hexEdit->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexEdit->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_hexDiff->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexDiff->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_diffLastDownIndex = (i.value().address + i.value().length) %
                          m_diffMemorySize;

    m_diffLastUpIndex = i.value().address - 1;

    while(m_diffLastUpIndex < 0)
    {
        m_diffLastUpIndex += m_diffMemorySize;
    }
}

void BinaryEditor::lastDifference()
{
    if(m_diffMarkupDown.isEmpty())
    {
        QMessageBox::information(this, tr("Find Difference"),
        tr("No differences found")); return;
    }

    QMap<int, BINEditor::Markup>::const_iterator i=m_diffMarkupDown.end()-1;

    m_hexEdit->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexEdit->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_hexDiff->setCursorPosition(i.value().address + i.value().length - 1);
    m_hexDiff->setCursorPosition(i.value().address,
    BINEditor::BinEditorWidget::KeepAnchor);

    m_diffLastDownIndex = (i.value().address + i.value().length) %
                          m_diffMemorySize;

    m_diffLastUpIndex = i.value().address - 1;

    while(m_diffLastUpIndex < 0)
    {
        m_diffLastUpIndex += m_diffMemorySize;
    }
}

void BinaryEditor::closeCompare()
{
    m_ui->diffFrame->hide();

    m_hexEdit->setMarkup(QList<BINEditor::Markup>());
    m_hexDiff->setMarkup(QList<BINEditor::Markup>());

    m_ui->actionFirst_Difference->setEnabled(false);
    m_ui->actionNext_Difference->setEnabled(false);
    m_ui->actionPrevious_Difference->setEnabled(false);
    m_ui->actionLast_Difference->setEnabled(false);
    m_ui->actionClose->setEnabled(false);

    m_hexDiff->clear();

    m_diffCursorPosition = m_hexThis->cursorPosition();
    m_diffMarkupDown.clear(); m_diffLastDownIndex = -1;
    m_diffMarkupUp.clear(); m_diffLastUpIndex = -1;

    m_diffMemoryOffset = 0;
    m_diffMemorySize = 0;
}

void BinaryEditor::generalHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open General Help Error"),
        tr("Unable to open the URL to the Propino General Help page"));
    }
}

void BinaryEditor::editorHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(this, tr("Open Editor Help Error"),
        tr("Unable to open the URL to the Propino Editor Help page"));
    }
}

void BinaryEditor::cursorPositionChanged(int position)
{
    if(m_diffCursorPosition != position)
    {
        m_diffCursorPosition = position;

        m_diffLastDownIndex = -1;
        m_diffLastUpIndex = -1;
    }

    m_positionLabel->setText(tr("Cursor Address: ") + "0x" + QString("%1").
    arg(position, 0x08L, 0x10L, QLatin1Char('0')).toUpper() + "h ");
}

bool BinaryEditor::memoryInit(QFtdi *ftdi, QProgressDialog *dialog)
{
    Q_CHECK_PTR(ftdi); Q_CHECK_PTR(dialog);

    m_ftdi = ftdi; m_dialog = dialog;

    disconnect(m_ftdi, SIGNAL(aboutToClose()), this, SLOT(close()));
    connect(m_ftdi, SIGNAL(aboutToClose()), this, SLOT(close()));

    if(!runCommand(settings_cmd))
    {
        m_dialog->close(); close(); return false;
    }

    QByteArray data(m_hexThisSize = m_memorySize, 0);

    if(!runCommand(read_cmd, &data))
    {
        m_dialog->close(); close(); return false;
    }

    m_offsetLabel->setText(tr("Offset: ") + "0x" + QString("%1").
    arg(m_memoryOffset, 0x08L, 0x10L, QLatin1Char('0')).toUpper() +
    tr("h (%L1 Bytes) ").arg(m_memoryOffset));

    m_sizeLabel->setText(tr("Size: ") + "0x" + QString("%1").
    arg(m_memorySize, 0x08L, 0x10L, QLatin1Char('0')).toUpper() +
    tr("h (%L1 Bytes) ").arg(m_memorySize));

    setBinEditorData(m_hexEdit, data); emit hexEditChanged(); return true;
}

void BinaryEditor::readMemory()
{
    if(QMessageBox::question(this, tr("Binary Editor"),  tr("Read memory?"),
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        closeCompare();

        QProgressDialog *backup = m_dialog;

        QProgressDialog progress(this); m_dialog = &progress;

        QByteArray data = getBinEditorData(m_hexEdit, m_memorySize);

        if(!runCommand(read_cmd, &data))
        {
            m_dialog->close(); close(); m_dialog = backup; return;
        }

        setBinEditorData(m_hexEdit, data); m_dialog = backup;

        emit hexEditChanged();
    }
}

void BinaryEditor::writeMemory()
{
    if(QMessageBox::question(this, tr("Binary Editor"), tr("Write memory?"),
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        closeCompare();

        QProgressDialog *backup = m_dialog;

        QProgressDialog progress(this); m_dialog = &progress;

        QByteArray data = getBinEditorData(m_hexEdit, m_memorySize);

        if(!runCommand(write_cmd, &data))
        {
            m_dialog->close(); close(); m_dialog = backup; return;
        }

        setBinEditorData(m_hexEdit, data); m_dialog = backup;

        emit hexEditChanged();
    }
}

void BinaryEditor::eraseMemory()
{
    if(QMessageBox::question(this, tr("Binary Editor"), tr("Erase memory?"),
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        closeCompare();

        QProgressDialog *backup = m_dialog;

        QProgressDialog progress(this); m_dialog = &progress;

        if(!runCommand(erase_cmd))
        {
            m_dialog->close(); close(); m_dialog = backup; return;
        }

        QByteArray data = getBinEditorData(m_hexEdit, m_memorySize);

        if(!runCommand(read_cmd, &data))
        {
            m_dialog->close(); close(); m_dialog = backup; return;
        }

        setBinEditorData(m_hexEdit, data); m_dialog = backup;

        emit hexEditChanged();
    }
}

bool BinaryEditor::runCommand(MemCmd command, QByteArray *bytes)
{
    Q_CHECK_PTR(m_ftdi); Q_CHECK_PTR(m_dialog);

    m_dialog->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint |
    Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::Dialog);

    m_dialog->setCancelButton(NULL);
    m_dialog->setWindowModality(Qt::ApplicationModal);
    m_dialog->setWindowTitle(tr("Binary Editor"));
    m_dialog->setRange(0, 0);

    QEventLoop loop; QFuture<QString> future; QFutureWatcher<QString> watcher;

    connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));

    m_ftdi->blockSignals(true); m_dialog->show();

    future = QtConcurrent::run(this, &BinaryEditor::commandSwitch,
    command, bytes);

    watcher.setFuture(future); loop.exec();

    m_ftdi->blockSignals(false); bool ok = m_ftdi->isOpen();

    QString result = future.result();

    if(!result.isNull())
    {
        m_dialog->hide();

        QMessageBox::critical(m_dialog->parentWidget(),
        tr("Binary Editor"), result);
    }

    if(!ok)
    {
        emit m_ftdi->aboutToClose();
    }

    return result.isNull();
}

QString BinaryEditor::commandSwitch(MemCmd command, QByteArray *bytes)
{
    QString result;

    switch(command)
    {
        case settings_cmd: result = commandSettings(); break;
        case read_cmd: result = commandRead(bytes); break;
        case write_cmd: result = commandWrite(bytes); break;
        case erase_cmd: result = commandErase(); break;
        case boot_cmd: result = commandBoot(); break;

        default: return QString();
    }

    if(m_ftdi->isOpen())
    {
        if(result.isNull()) // success
        {
            if(!m_ftdi->setInTask(false, 10))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(10);

            if((!m_ftdi->purge(FT_PURGE_RX))
            || (!m_ftdi->setInTask(true, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }
        }
        else // failure
        {
            if((!m_ftdi->setDtr(true))
            || (!m_ftdi->setRts(true))
            || (!m_ftdi->setInTask(false, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(100);

            if((!m_ftdi->setRts(false)) || (!m_ftdi->setDtr(false)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }

            QThread::msleep(100);

            if((!m_ftdi->purge(FT_PURGE_RX | FT_PURGE_TX))
            || (!m_ftdi->setInTask(true, 10)))
            {
                QString error = m_ftdi->errorString();
                m_ftdi->close(); return error;
            }
        }

        DWORD time = calcUSBRWTimeouts(FT_BAUD_115200);
        DWORD size = calcUSBParameters(FT_BAUD_115200);

        if((!m_ftdi->setBaudRate(FT_BAUD_115200))
        || (!m_ftdi->setTimeouts(time, time))
        || (!m_ftdi->setUSBParameters(size, size))
        || (!m_ftdi->setLatencyTimer(16)))
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }
    }

    return result;
}

QString BinaryEditor::commandSettings()
{
    m_dialog->setLabelText(tr("Reading Settings..."));

    if((!m_ftdi->setBaudRate(FT_BAUD_115200))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // receive sync phrase ////////////////////////////////////////////////////

    bool ok = false; QByteArray shiftreg;

    for(int i = 0; i < 300; i++)
    {
        char c;

        if(m_ftdi->getChar(&c) && shiftreg.append(c).endsWith("Hello World!"))
        {
            ok = true; break;
        }
    }

    if(!ok)
    {
        return tr("Syncing error");
    }

    // receive settings size //////////////////////////////////////////////////

    union { struct { quint16 a, b; } s; char c[sizeof(s)]; } setSize;

    if((m_ftdi->readBytes(setSize.c, sizeof(setSize.c), 100)
    != sizeof(setSize.c)) || ((setSize.s.a & 0xFFFF)
    != ((~setSize.s.b) & 0xFFFF)))
    {
        return tr("Syncing error");
    }

    // receive settings data //////////////////////////////////////////////////

    QByteArray setData(setSize.s.a, 0);

    if(m_ftdi->readBytes(&setData, 500) != setSize.s.a)
    {
        return tr("Syncing error");
    }

    #pragma pack(1) // pack tight

    union settingsBlocks
    {
        struct
        {
            struct settingsBlock0
            {
                union
                {
                    struct
                    {
                        quint32 maxBaudRate;
                        quint32 maxBlockSize;
                        quint32 memoryOffset;
                        quint32 memorySize;

                        quint16 signature;
                    }
                    s;

                    char c[sizeof(s)];
                }
                u;

                quint16 checksum;
            }
            s0;
        }
        s;

        char c[sizeof(s)];
    }
    settings;

    #pragma pack(0) // pack loose

    qMemSet(settings.c, 0x00, sizeof(settings.c));
    qMemCopy(settings.c, setData.constData(),
    qMin(setData.size(), static_cast<int>(sizeof(settings.c))));

    // check settings data ////////////////////////////////////////////////////

    if(settings.s.s0.u.s.signature != 0xAA55)
    {
        return tr("Signature error");
    }

    quint16 checksum = qChecksum(settings.s.s0.u.c, sizeof(settings.s.s0.u.c));

    if(checksum != settings.s.s0.checksum)
    {
        return tr("Checksum error - expected ") +
        "0x" + QString("%1").arg(checksum, 0x04L, 0x10L,
        QLatin1Char('0')).toUpper() + tr(" got ") +
        "0x" + QString("%1").arg(settings.s.s0.checksum, 0x04L, 0x10L,
        QLatin1Char('0')).toUpper() + tr(" instead");
    }

    if(settings.s.s0.u.s.maxBaudRate < MIN_S1_BAUD_RATE)
    {
        return tr("Baud rate less than the min of %Ln bit(s) per second",
               "", MIN_S1_BAUD_RATE);
    }

    if(MAX_S1_BAUD_RATE < settings.s.s0.u.s.maxBaudRate)
    {
        return tr("Baud rate greater than the max of %Ln bit(s) per second",
               "", MAX_S1_BAUD_RATE);
    }

    if(settings.s.s0.u.s.maxBlockSize < MIN_S1_BLOCK_SIZE)
    {
        return tr("Block size less than the min of %Ln byte(s)",
               "", MIN_S1_BLOCK_SIZE);
    }

    if(MAX_S1_BLOCK_SIZE < settings.s.s0.u.s.maxBlockSize)
    {
        return tr("Block size greater than the max of %Ln byte(s)",
               "", MAX_S1_BLOCK_SIZE);
    }

    if(settings.s.s0.u.s.memorySize < MIN_MEM_SIZE)
    {
        return tr("Memory size less than the min of %Ln byte(s)",
               "", MIN_MEM_SIZE);
    }

    if(MAX_MEM_SIZE < settings.s.s0.u.s.memorySize)
    {
        return tr("Memory size greater than the max of %Ln byte(s)",
               "", MAX_MEM_SIZE);
    }

    if((quint64(settings.s.s0.u.s.memoryOffset)
     +  quint64(settings.s.s0.u.s.memorySize))
     < MIN_MEM_OFFSET)
    {
        return tr("Memory offset plus memory size less than the min of "
               "%L1 byte(s)", "", MIN_MEM_OFFSET>1?2:1).arg(MIN_MEM_OFFSET);
    }

    if(MAX_MEM_OFFSET
     < (quint64(settings.s.s0.u.s.memoryOffset)
     +  quint64(settings.s.s0.u.s.memorySize)))
    {
        return tr("Memory offset plus memory size greater than the max of "
               "%L1 byte(s)", "", MAX_MEM_OFFSET>1?2:1).arg(MAX_MEM_OFFSET);
    }

    if(!m_ftdi->setBaudRate(settings.s.s0.u.s.maxBaudRate))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    m_maxBaudRate = settings.s.s0.u.s.maxBaudRate;
    m_maxBlockSize = settings.s.s0.u.s.maxBlockSize;
    m_memoryOffset = settings.s.s0.u.s.memoryOffset;
    m_memorySize = settings.s.s0.u.s.memorySize;

    return QString();
}

QString BinaryEditor::commandRead(QByteArray *bytes)
{
    m_dialog->setLabelText(tr("Receiving Blocks..."));

    if(bytes == NULL)
    {
        return tr("Null pointer error");
    }

    if((!m_ftdi->setBaudRate(m_maxBaudRate))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // read binary data blocks ////////////////////////////////////////////////

    quint32 address = m_memoryOffset;

    int zeroPadding = (((bytes->size() + sizeof(quint32) - 1) /
    sizeof(quint32)) * sizeof(quint32)) - bytes->size();

    while(zeroPadding--)
    {
        bytes->append(char(0));
    }

    while(static_cast<int>(address - m_memoryOffset) < bytes->size())
    {
        quint32 memSize = qMin(m_maxBlockSize,
        bytes->size() - (address - m_memoryOffset));

        QByteArray block;

        // add block address //////////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;
            u.s = address;
            block.append(u.c, sizeof(u.c));
        }

        // add block signature ////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = 0xAA55;
            block.append(u.c, sizeof(u.c));
        }

        // add block checksum /////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = qChecksum(block.constData(), block.size());
            block.append(u.c, sizeof(u.c));
        }

        // add block size /////////////////////////////////////////////////////
        {
            union { struct { quint16 a, b; } s; char c[sizeof(s)]; } u;
            u.s.a = (memSize / sizeof(quint32)); u.s.b = ~u.s.a;
            block.prepend(u.c, sizeof(u.c)); block.prepend("RrRr");
        }

        // write block ////////////////////////////////////////////////////////

        if(m_ftdi->write(block) != block.size())
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }

        // wait for acknowledge ///////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;

            if(m_ftdi->readBytes(u.c, sizeof(u.c), 500) != sizeof(u.c))
            {
                return tr("Acknowledge error");
            }

            if(u.s != 0x41413535)
            {
                return tr("Acknowledge error");
            }
        }

        // receive memory data ////////////////////////////////////////////////

        QByteArray memData(memSize + sizeof(quint16) + sizeof(quint16), 0);

        if(m_ftdi->readBytes(&memData, 500) != memData.size())
        {
            return tr("Receive error");
        }

        union { quint16 i; struct { char a, b; } s; } signature;

        signature.s.a = memData.at(memSize + 0);
        signature.s.b = memData.at(memSize + 1);

        union { quint16 i; struct { char a, b; } s; } checksum;

        checksum.s.a = memData.at(memSize + sizeof(quint16) + 0);
        checksum.s.b = memData.at(memSize + sizeof(quint16) + 1);

        // check memory data //////////////////////////////////////////////////

        if(signature.i != 0xAA55)
        {
            return tr("Signature error");
        }

        if(qChecksum(memData.constData(), memSize + sizeof(quint16))
        != checksum.i)
        {
            return tr("Checksum error");
        }

        // wait for acknowledge ///////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;

            if(m_ftdi->readBytes(u.c, sizeof(u.c), 500) != sizeof(u.c))
            {
                return tr("Acknowledge error");
            }

            if(u.s != 0x41413535)
            {
                return tr("Acknowledge error");
            }
        }

        char *data = bytes->data();

        for(quint32 i = 0; i < memSize; i++)
        {
            data[(address - m_memoryOffset) + i] = memData.at(i);
        }

        address += memSize;
    }

    return QString();
}

QString BinaryEditor::commandWrite(const QByteArray *bytes)
{
    m_dialog->setLabelText(tr("Sending Blocks..."));

    if(bytes == NULL)
    {
        return tr("Null pointer error");
    }

    if((!m_ftdi->setBaudRate(m_maxBaudRate))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // send binary data blocks ////////////////////////////////////////////////

    QByteArray binaryFileData(*bytes); quint32 address = m_memoryOffset;

    while(binaryFileData.size())
    {
        QByteArray data = binaryFileData.left(m_maxBlockSize);

        int zeroPadding = (((data.size() + sizeof(quint32) - 1) /
        sizeof(quint32)) * sizeof(quint32)) - data.size();

        while(zeroPadding--)
        {
            data.append(char(0));
        }

        QByteArray block;

        // add block address //////////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;
            u.s = address; address += data.size();
            block.append(u.c, sizeof(u.c));
        }

        // add block data /////////////////////////////////////////////////////
        {
            block.append(data);
        }

        // add block signature ////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = 0xAA55;
            block.append(u.c, sizeof(u.c));
        }

        // add block checksum /////////////////////////////////////////////////
        {
            union { quint16 s; char c[sizeof(s)]; } u;
            u.s = qChecksum(block.constData(), block.size());
            block.append(u.c, sizeof(u.c));
        }

        // add block size /////////////////////////////////////////////////////
        {
            union { struct { quint16 a, b; } s; char c[sizeof(s)]; } u;
            u.s.a = (data.size() / sizeof(quint32)); u.s.b = ~u.s.a;
            block.prepend(u.c, sizeof(u.c)); block.prepend("WwWw");
        }

        // write block ////////////////////////////////////////////////////////

        if(m_ftdi->write(block) != block.size())
        {
            QString error = m_ftdi->errorString();
            m_ftdi->close(); return error;
        }

        // wait for acknowledge ///////////////////////////////////////////////
        {
            union { quint32 s; char c[sizeof(s)]; } u;

            if(m_ftdi->readBytes(u.c, sizeof(u.c), 500) != sizeof(u.c))
            {
                return tr("Acknowledge error");
            }

            if(u.s != 0x41413535)
            {
                return tr("Acknowledge error");
            }
        }

        binaryFileData.remove(0, m_maxBlockSize);
    }

    return QString();
}

QString BinaryEditor::commandErase()
{
    m_dialog->setLabelText(tr("Erasing Memory..."));

    if((!m_ftdi->setBaudRate(m_maxBaudRate))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // send command ///////////////////////////////////////////////////////////

    if(m_ftdi->write("EeEe") != 4)
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // wait for acknowledge ///////////////////////////////////////////////////
    {
        union { quint32 s; char c[sizeof(s)]; } u;

        if(m_ftdi->readBytes(u.c, sizeof(u.c), 10000) != sizeof(u.c))
        {
            return tr("Acknowledge error");
        }

        if(u.s != 0x41413535)
        {
            return tr("Acknowledge error");
        }
    }

    return QString();
}

QString BinaryEditor::commandBoot()
{
    m_dialog->setLabelText(tr("Rebooting Device..."));

    if((!m_ftdi->setBaudRate(m_maxBaudRate))
    || (!m_ftdi->setTimeouts(1, 0))
    || (!m_ftdi->setUSBParameters(4096, 4096))
    || (!m_ftdi->setLatencyTimer(1)))
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // send command ///////////////////////////////////////////////////////////

    if(m_ftdi->write("BbBb") != 4)
    {
        QString error = m_ftdi->errorString();
        m_ftdi->close(); return error;
    }

    // wait for acknowledge ///////////////////////////////////////////////////
    {
        union { quint32 s; char c[sizeof(s)]; } u;

        if(m_ftdi->readBytes(u.c, sizeof(u.c), 100) != sizeof(u.c))
        {
            return tr("Acknowledge error");
        }

        if(u.s != 0x41413535)
        {
            return tr("Acknowledge error");
        }
    }

    return QString();
}

// BINEditor Workaround ///////////////////////////////////////////////////////

QByteArray getBinEditorData(BINEditor::BinEditorWidget *edit, int size)
{
    // You need to read the BINEditorWidget code to understand this code...

    QTemporaryFile file;

    if(file.open() && file.resize(size))
    {
        file.close();

        QString errorString;

        if(edit->save(&errorString, file.fileName(), file.fileName())
        && file.open())
        {
            return file.readAll();
        }
    }

    return QByteArray();
}

void setBinEditorData(BINEditor::BinEditorWidget *edit, const QByteArray &data)
{
    // You need to read the BINEditorWidget code to understand this code...

    edit->clear();

    int size = data.size() + (0x10 - (data.size() % 0x10)); // this is correct
    edit->setSizes(edit->baseAddress(), data.size(), size); // this is correct

    QByteArray copy(data); copy.append(QByteArray(size - data.size(), 0x00));
    edit->addData(edit->baseAddress(), copy);

    // Init data

    edit->updateContents(); edit->addData(edit->baseAddress(), copy);

    // Init modifiedData

    QByteArray left = data.left(0x01).toHex();
    edit->setCursorPosition(edit->baseAddress());

    QKeyEvent k(QEvent::KeyPress, 0, 0, left);
    QApplication::sendEvent(edit, &k);

    QKeyEvent e(QEvent::KeyPress, Qt::CTRL | Qt::Key_Z, Qt::ControlModifier);
    QApplication::sendEvent(edit, &e);
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
