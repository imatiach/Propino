/***************************************************************************//**
* @file
* Binary Finder
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

#ifndef BINARYFINDER_H
#define BINARYFINDER_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include <bineditor/bineditor.h>
#include <bineditor/bineditorconstants.h>
#include <bineditor/bineditorplugin.h>
#include <bineditor/markup.h>

#include "propino_settings.h"

#include "binaryeditor.h"

namespace Ui
{
    class BinaryFinder;
}

class BinaryEditor;

class BinaryFinder : public QFrame
{
    Q_OBJECT

public:

    explicit BinaryFinder(QSettings *settings,
                          BinaryEditor *parent = NULL,
                          const QString &prefix = BINARY_EDITOR);
    ~BinaryFinder();

public slots:

    void hide(); void show();

private slots:

    void highlightSearchResults(const QString &data);
    void findFast(const QString &data);

    void hexEditChanged();
    void cursorPositionChanged(int position);

    int find(); int replace(); void replaceAll();

private:

    Q_DISABLE_COPY(BinaryFinder) friend class BinaryEditor;

    BinaryEditor *m_editor;
    BINEditor::BinEditorWidget *m_hexThis; int m_hexThisSize;

    bool m_autoReplaceAll;
    int m_cursorPosition; int m_lastDownIndex; int m_lastUpIndex;

    QSettings *m_settings; QString m_prefix; Ui::BinaryFinder *m_ui;
};

#endif // BINARYFINDER_H

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
