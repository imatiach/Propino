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

#ifndef BINARYEDITOR_H
#define BINARYEDITOR_H

#include <QtConcurrent>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <extensionsystem/invoker.h>
#include <extensionsystem/pluginmanager.h>

#include <bineditor/bineditor.h>
#include <bineditor/bineditorconstants.h>
#include <bineditor/bineditorplugin.h>
#include <bineditor/markup.h>

#include "propino_settings.h"

#include "qftdi.h"
#include "serialescape.h"
#include "serialloader.h"

#include "binaryfinder.h"

#define ONE_BYTE 01
#define ONE_KILOBYTE (ONE_BYTE * 1024)
#define ONE_MEGABYTE (ONE_KILOBYTE * 1024)
#define ONE_GIGABYTE (ONE_MEGABYTE * 1024)

#define MIN_MEM_SIZE 0x0000001UL
#define MAX_MEM_SIZE 0x1000000UL

#define MIN_MEM_OFFSET 0x000000001UL
#define MAX_MEM_OFFSET 0x100000000UL

namespace Ui
{
    class BinaryEditor;
}

class BinaryFinder;

class BinaryEditor : public QMainWindow
{
    Q_OBJECT

public:

    explicit BinaryEditor(const QString &title, QSettings * settings,
    QWidget *parent = NULL, const QString &prefix = BINARY_EDITOR);
    ~BinaryEditor(); void warn();

    BINEditor::BinEditorWidget *getCurrentHexEdit() { return m_hexThis; }
    int getCurrentHexEditSize() { return m_hexThisSize; }

    bool memoryInit(QFtdi *ftdi, QProgressDialog *dialog);
    enum MemCmd { settings_cmd, read_cmd, write_cmd, erase_cmd, boot_cmd };
    bool runCommand(MemCmd command, QByteArray *bytes = NULL);

private slots:

    // File Menu

    void insertFile();
    void save();
    void saveSelection();

    void exportASCIITextToClipboard();
    void exportASCIITextToFile();
    void exportCCodeToClipboard();
    void exportCCodeToFile();

    // Edit Menu

    void editAboutToShow();

    void undoShortcut();
    void redoShortcut();
    void cutShortcut();
    void copyShortcut();
    void pasteShortcut();
    void deleteShortcut();
    void selectAllShortcut();

    void gotoOffset();

    // Memory Menu

    void readMemory();
    void writeMemory();
    void eraseMemory();

    void checksum8Bit();
    void checksum16Bit();
    void checksum32Bit();
    void checksum64Bit();
    void CRC16CCITTChecksum();
    void MD4Hash();
    void MD5Hash();
    void SHA1Hash();
    void SHA224Hash();
    void SHA256Hash();
    void SHA384Hash();
    void SHA512Hash();

    void hex8BitUnsigned();
    void hex8BitSigned();
    void hex16BitUnsigned();
    void hex16BitSigned();
    void hex32BitUnsigned();
    void hex32BitSigned();
    void hex64BitUnsigned();
    void hex64BitSigned();
    void hexFloat();
    void hexDouble();

    void num8BitUnsigned();
    void num8BitSigned();
    void num16BitUnsigned();
    void num16BitSigned();
    void num32BitUnsigned();
    void num32BitSigned();
    void num64BitUnsigned();
    void num64BitSigned();
    void numFloat();
    void numDouble();

    void compareFiles();
    void firstDifference();
    void nextDifference();
    void previousDifference();
    void lastDifference();
    void closeCompare();

    // Help Menu

    void generalHelp();
    void editorHelp();

    void cursorPositionChanged(int position);

signals:

    void hexEditChanged();

    void closed();

protected:

    void closeEvent(QCloseEvent *event);

private:

    Q_DISABLE_COPY(BinaryEditor) friend class BinaryFinder;

    bool eventFilter(QObject *object, QEvent *event);

    BINEditor::BinEditorWidget *m_hexEdit;
    BINEditor::BinEditorWidget *m_hexDiff;

    BINEditor::BinEditorWidget *m_hexThis; int m_hexThisSize;

    QLabel *m_positionLabel;
    QLabel *m_offsetLabel;
    QLabel *m_sizeLabel;

    BinaryFinder *m_finder;

    int m_diffCursorPosition;

    QMap<int, BINEditor::Markup> m_diffMarkupDown; int m_diffLastDownIndex;
    QMap<int, BINEditor::Markup> m_diffMarkupUp; int m_diffLastUpIndex;

    QSettings *m_settings; QString m_prefix; Ui::BinaryEditor *m_ui;

    QString commandSwitch(MemCmd command, QByteArray *bytes = NULL);
    QString commandSettings();
    QString commandRead(QByteArray *bytes);
    QString commandWrite(const QByteArray *bytes);
    QString commandErase();
    QString commandBoot();

    QFtdi *m_ftdi; QProgressDialog *m_dialog;

    quint32 m_maxBaudRate;
    quint32 m_maxBlockSize;
    quint32 m_memoryOffset;
    quint32 m_memorySize;

    quint32 m_diffMemoryOffset;
    quint32 m_diffMemorySize;
};

// BINEditor Workaround ///////////////////////////////////////////////////////

QByteArray getBinEditorData(BINEditor::BinEditorWidget *edit,
                            int size);

void setBinEditorData(BINEditor::BinEditorWidget *edit,
                      const QByteArray &data);

#endif // BINARYEDITOR_H

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
