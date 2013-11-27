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

#include "binaryfinder.h"
#include "ui_binaryfinder.h"

BinaryFinder::BinaryFinder(QSettings *settings,
                           BinaryEditor *parent,
                           const QString &prefix) :
    QFrame(parent),
    m_ui(new Ui::BinaryFinder)
{
    Q_CHECK_PTR(settings); Q_CHECK_PTR(parent);

    m_ui->setupUi(this); m_settings = settings; m_prefix = prefix;

    m_editor = parent;
    m_hexThis = m_editor->getCurrentHexEdit();
    m_hexThisSize = m_editor->getCurrentHexEditSize();

    m_autoReplaceAll = false;

    m_cursorPosition = m_hexThis->cursorPosition();
    m_lastDownIndex = -1; m_lastUpIndex = -1;

    m_ui->replaceButton->setDisabled(m_hexThis->isReadOnly());
    m_ui->replaceAllButton->setDisabled(m_hexThis->isReadOnly());

    connect(m_ui->findEdit, SIGNAL(textEdited(QString)),
            this, SLOT(highlightSearchResults(QString)));

    connect(m_ui->findEdit, SIGNAL(textEdited(QString)),
            this, SLOT(findFast(QString)));

    connect(m_editor, SIGNAL(hexEditChanged()),
            this, SLOT(hexEditChanged()));

    connect(m_hexThis, SIGNAL(cursorPositionChanged(int)),
            this, SLOT(cursorPositionChanged(int)));

    connect(m_ui->findButton, SIGNAL(clicked()),
            this, SLOT(find()));

    connect(m_ui->replaceButton, SIGNAL(clicked()),
            this, SLOT(replace()));

    connect(m_ui->replaceAllButton, SIGNAL(clicked()),
            this, SLOT(replaceAll()));

    connect(m_ui->closeButton, SIGNAL(clicked()),
            this, SLOT(hide()));

    connect(m_ui->findEdit, SIGNAL(returnPressed()),
            this, SLOT(find()));

    connect(m_ui->replaceEdit, SIGNAL(returnPressed()),
            this, SLOT(replace()));

    QSettings i_settings(m_settings->fileName(), m_settings->format());

    i_settings.beginGroup(PROPINO_KEY_GROUP);
    i_settings.beginGroup(m_prefix);

    m_ui->findEdit->setCompleter(new QCompleter(
    i_settings.value(BINARY_EDITOR_KEY_FINDER_F_STRINGS, QStringList()).
    toStringList(), this));

    m_ui->replaceEdit->setCompleter(new QCompleter(
    i_settings.value(BINARY_EDITOR_KEY_FINDER_R_STRINGS, QStringList()).
    toStringList(), this));
}

BinaryFinder::~BinaryFinder()
{
    QSettings settings(m_settings->fileName(), m_settings->format());

    settings.beginGroup(PROPINO_KEY_GROUP);
    settings.beginGroup(m_prefix);

    settings.setValue(BINARY_EDITOR_KEY_FINDER_F_STRINGS,
    q_check_ptr(qobject_cast<QStringListModel*>
    (m_ui->findEdit->completer()->model()))->stringList());

    settings.setValue(BINARY_EDITOR_KEY_FINDER_R_STRINGS,
    q_check_ptr(qobject_cast<QStringListModel*>
    (m_ui->replaceEdit->completer()->model()))->stringList());

    delete m_ui;
}

void BinaryFinder::hide()
{
    // ---------- Clear Search Results ---------- //
    m_hexThis->highlightSearchResults(QByteArray());

    QWidget::hide();
}

void BinaryFinder::show()
{
    m_editor->copyShortcut(); QString text =
    QApplication::clipboard()->text().remove(QRegExp("\\s+"));

    m_ui->findEdit->setText(text);
    m_ui->findEdit->setFocus();

    int end = m_hexThis->selectionEnd();
    int start = m_hexThis->selectionStart();

    m_hexThis->setCursorPosition(end);
    m_hexThis->setCursorPosition(start,
    BINEditor::BinEditorWidget::KeepAnchor);

    highlightSearchResults(text);
    findFast(text);

    QWidget::show();
}

void BinaryFinder::highlightSearchResults(const QString &data)
{
    m_hexThis->highlightSearchResults(data.toUtf8(),
    (m_ui->findCheckBox->isChecked() ? QTextDocument::FindFlag(0) :
                                       QTextDocument::FindCaseSensitively));
}

void BinaryFinder::findFast(const QString &data)
{
    QByteArray text = data.toUtf8();

    int origin = -1;

    if(m_ui->beginningButton->isChecked())
    {
        origin = 0;
    }
    else if(m_ui->cursorButton->isChecked())
    {
        origin = m_hexThis->cursorPosition();
    }
    else if(m_ui->endButton->isChecked())
    {
        origin = m_hexThisSize - 1;
    }

    int result = -1;

    if(m_ui->downButton->isChecked())
    {
        result = m_hexThis->find(text, origin,
        (m_ui->findCheckBox->isChecked() ?
        QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));

        if(result < 0) // Try again from beginning
        {
            result = m_hexThis->find(text, 0,
            (m_ui->findCheckBox->isChecked() ?
            QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));
        }
    }
    else if(m_ui->upButton->isChecked())
    {
        result = m_hexThis->find(text, origin,
        QTextDocument::FindBackward |
        (m_ui->findCheckBox->isChecked() ?
        QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));

        if(result < 0) // Try again from end
        {
            result = m_hexThis->find(text, m_hexThisSize - 1,
            QTextDocument::FindBackward |
            (m_ui->findCheckBox->isChecked() ?
            QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));
        }
    }

    if(result >= 0)
    {
        int selectionSize = m_hexThis->selectionEnd() -
                            m_hexThis->selectionStart();

        // Begin find() Selection Workaround //
        {
            if(m_hexThis->selectionEnd() == (m_hexThisSize - 1))
            {
                int size = text.size();

                if(((size / 2) == selectionSize)
                || ((size / 2) == (selectionSize + 1)))
                {
                    size /= 2;
                }

                selectionSize = size;
            }
        }
        // End find() Selection Workaround //

        m_hexThis->setCursorPosition(result + selectionSize - 1);
        m_hexThis->setCursorPosition(result,
        BINEditor::BinEditorWidget::KeepAnchor);

        m_ui->cursorButton->setChecked(true);

        m_lastDownIndex = (result + selectionSize) % m_hexThisSize;
        m_lastUpIndex = result - selectionSize;

        while(m_lastUpIndex < 0)
        {
            m_lastUpIndex += m_hexThisSize;
        }
    }
    else
    {
        m_lastDownIndex = -1; m_lastUpIndex = -1;
    }
}

void BinaryFinder::hexEditChanged()
{
    // ---------- Clear Search Results ---------- //
    m_hexThis->highlightSearchResults(QByteArray());

    BINEditor::BinEditorWidget *oldHexThis = m_hexThis; Q_UNUSED(oldHexThis);
    int oldHexThisSize = m_hexThisSize; Q_UNUSED(oldHexThisSize);

    m_hexThis = m_editor->getCurrentHexEdit();
    m_hexThisSize = m_editor->getCurrentHexEditSize();

    m_cursorPosition = m_hexThis->cursorPosition();
    m_lastDownIndex = -1; m_lastUpIndex = -1;

    m_ui->replaceButton->setDisabled(m_hexThis->isReadOnly());
    m_ui->replaceAllButton->setDisabled(m_hexThis->isReadOnly());

    disconnect(oldHexThis, SIGNAL(cursorPositionChanged(int)),
               this, SLOT(cursorPositionChanged(int)));

    connect(m_hexThis, SIGNAL(cursorPositionChanged(int)),
            this, SLOT(cursorPositionChanged(int)));
}

void BinaryFinder::cursorPositionChanged(int position)
{
    if(m_cursorPosition != position)
    {
        m_cursorPosition = position; m_lastDownIndex = -1; m_lastUpIndex = -1;
    }
}

int BinaryFinder::find()
{
    if(((m_lastDownIndex < 0) || (m_lastUpIndex < 0)))
    {
        int end = m_hexThis->selectionEnd();
        int start = m_hexThis->selectionStart();

        m_hexThis->setCursorPosition(end);
        m_hexThis->setCursorPosition(start,
        BINEditor::BinEditorWidget::KeepAnchor);

        if(sender() == m_ui->findButton)
        {
            m_ui->cursorButton->setChecked(true); show();
        }

        if(sender() == m_ui->findEdit)
        {
            findFast(m_ui->findEdit->text());
        }
    }

    if(!m_autoReplaceAll)
    {
        if(m_ui->findEdit->text().isEmpty())
        {
            QApplication::beep(); m_ui->findEdit->setFocus(); return -1;
        }

        QStringList list = q_check_ptr(qobject_cast<QStringListModel*>
        (m_ui->findEdit->completer()->model()))->stringList();

        if(!list.contains(m_ui->findEdit->text()))
        {
            list.append(m_ui->findEdit->text());

            while(list.size() >
            (m_ui->findEdit->completer()->maxVisibleItems() * 2))
            {
                list.removeFirst();
            }
        }

        q_check_ptr(qobject_cast<QStringListModel*>
        (m_ui->findEdit->completer()->model()))->setStringList(list);
    }

    // do find ////////////////////////////////////////////////////////////////

    QByteArray text = m_ui->findEdit->text().toUtf8();

    int origin = -1;

    if(m_ui->beginningButton->isChecked())
    {
        origin = 0;
    }
    else if(m_ui->cursorButton->isChecked())
    {
        origin = m_hexThis->cursorPosition();

        if(m_ui->downButton->isChecked() && (m_lastDownIndex >= 0))
        {
            origin = m_lastDownIndex;
        }
        else if(m_ui->upButton->isChecked() && (m_lastUpIndex >= 0))
        {
            origin = m_lastUpIndex;
        }
    }
    else if(m_ui->endButton->isChecked())
    {
        origin = m_hexThisSize - 1;
    }

    int result = -1;

    if(m_ui->downButton->isChecked())
    {
        result = m_hexThis->find(text, origin,
        (m_ui->findCheckBox->isChecked() ?
        QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));

        if(result < 0) // Try again from beginning
        {
            result = m_hexThis->find(text, 0,
            (m_ui->findCheckBox->isChecked() ?
            QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));
        }
    }
    else if(m_ui->upButton->isChecked())
    {
        result = m_hexThis->find(text, origin,
        QTextDocument::FindBackward |
        (m_ui->findCheckBox->isChecked() ?
        QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));

        if(result < 0) // Try again from end
        {
            result = m_hexThis->find(text, m_hexThisSize - 1,
            QTextDocument::FindBackward |
            (m_ui->findCheckBox->isChecked() ?
            QTextDocument::FindFlag(0) : QTextDocument::FindCaseSensitively));
        }
    }

    if(result >= 0)
    {
        int selectionSize = m_hexThis->selectionEnd() -
                            m_hexThis->selectionStart();

        // Begin find() Selection Workaround //
        {
            if(m_hexThis->selectionEnd() == (m_hexThisSize - 1))
            {
                int size = text.size();

                if(((size / 2) == selectionSize)
                || ((size / 2) == (selectionSize + 1)))
                {
                    size /= 2;
                }

                selectionSize = size;
            }
        }
        // End find() Selection Workaround //

        m_hexThis->setCursorPosition(result + selectionSize - 1);
        m_hexThis->setCursorPosition(result,
        BINEditor::BinEditorWidget::KeepAnchor);

        m_ui->cursorButton->setChecked(true);

        m_lastDownIndex = (result + selectionSize) % m_hexThisSize;
        m_lastUpIndex = result - selectionSize;

        while(m_lastUpIndex < 0)
        {
            m_lastUpIndex += m_hexThisSize;
        }
    }
    else
    {
        m_lastDownIndex = -1; m_lastUpIndex = -1;

        if(!m_autoReplaceAll)
        {
            QMessageBox::critical(m_editor, tr("Find Error"), tr("String ")
            + tr("\"%1\" not found").arg(m_ui->findEdit->text()));
        }
    }

    return result;
}

int BinaryFinder::replace()
{
    if(!m_autoReplaceAll)
    {
        if(m_ui->replaceEdit->text().isEmpty() || m_hexThis->isReadOnly())
        {
            QApplication::beep(); m_ui->replaceEdit->setFocus(); return -1;
        }

        QStringList list = q_check_ptr(qobject_cast<QStringListModel*>
        (m_ui->replaceEdit->completer()->model()))->stringList();

        if(!list.contains(m_ui->replaceEdit->text()))
        {
            list.append(m_ui->replaceEdit->text());

            while(list.size() >
            (m_ui->replaceEdit->completer()->maxVisibleItems() * 2))
            {
                list.removeFirst();
            }
        }

        q_check_ptr(qobject_cast<QStringListModel*>
        (m_ui->replaceEdit->completer()->model()))->setStringList(list);
    }

    // do replace /////////////////////////////////////////////////////////////

    int position = find();

    if(position >= 0)
    {
        int selectionSize = m_hexThis->selectionEnd() -
                            m_hexThis->selectionStart() + 1;

        QByteArray text = m_ui->replaceEdit->text().toUtf8();

        if(m_ui->replaceCheckBox->isChecked())
        {
            text = QByteArray::fromHex(text);
        }

        int size = qMin(selectionSize, text.size()); text.resize(size);

        QApplication::clipboard()->setText(text.toHex());
        m_editor->pasteShortcut();

        m_hexThis->setCursorPosition(position + selectionSize - 1);
        m_hexThis->setCursorPosition(position,
        BINEditor::BinEditorWidget::KeepAnchor);
    }

    return position;
}

void BinaryFinder::replaceAll()
{
    if(true)
    {
        // do replace stuff ///////////////////////////////////////////////////
        {
            if(m_ui->replaceEdit->text().isEmpty() || m_hexThis->isReadOnly())
            {
                QApplication::beep(); m_ui->replaceEdit->setFocus(); return;
            }

            QStringList list = q_check_ptr(qobject_cast<QStringListModel*>
            (m_ui->replaceEdit->completer()->model()))->stringList();

            if(!list.contains(m_ui->replaceEdit->text()))
            {
                list.append(m_ui->replaceEdit->text());

                while(list.size() >
                (m_ui->replaceEdit->completer()->maxVisibleItems() * 2))
                {
                    list.removeFirst();
                }
            }

            q_check_ptr(qobject_cast<QStringListModel*>
            (m_ui->replaceEdit->completer()->model()))->setStringList(list);
        }

        // do find stuff //////////////////////////////////////////////////////
        {
            if(m_ui->findEdit->text().isEmpty())
            {
                QApplication::beep(); m_ui->findEdit->setFocus(); return;
            }

            QStringList list = q_check_ptr(qobject_cast<QStringListModel*>
            (m_ui->findEdit->completer()->model()))->stringList();

            if(!list.contains(m_ui->findEdit->text()))
            {
                list.append(m_ui->findEdit->text());

                while(list.size() >
                (m_ui->findEdit->completer()->maxVisibleItems() * 2))
                {
                    list.removeFirst();
                }
            }

            q_check_ptr(qobject_cast<QStringListModel*>
            (m_ui->findEdit->completer()->model()))->setStringList(list);
        }

        QByteArray fText = m_ui->findEdit->text().toUtf8();

        if(m_ui->findCheckBox->isChecked())
        {
            fText = fText.toLower();
        }

        QMessageBox::StandardButton button = QMessageBox::question(m_editor,
        tr("Replace All"), tr("Interpret the find text as a hex pattern?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes);

        if(button == QMessageBox::Cancel)
        {
            return;
        }

        if(button == QMessageBox::Yes)
        {
            fText = QByteArray::fromHex(fText);
        }

        QByteArray rText = m_ui->replaceEdit->text().toUtf8();

        if(m_ui->replaceCheckBox->isChecked())
        {
            rText = QByteArray::fromHex(rText);
        }

        int size = qMin(fText.size(), rText.size()); rText.resize(size);

        QByteArray data = getBinEditorData(m_hexThis, m_hexThisSize);

        int i = 0, k; for(int j = 0; (j = data.indexOf(fText, j)) >= 0; i++)
        {
            data.replace(j, size, rText); k = j; j += fText.size();
        }

        if(i >= 1)
        {
            setBinEditorData(m_hexThis, data);

            m_hexThis->setCursorPosition(k + fText.size() - 1);
            m_hexThis->setCursorPosition(k,
            BINEditor::BinEditorWidget::KeepAnchor);

            m_ui->cursorButton->setChecked(true);

            if(isVisible()) // Prevents popup if hidden already on error...
            {
                QMessageBox::information(m_editor, tr("Replace All"),
                tr("Replaced %Ln occurrence(s) of ", "", i) +
                tr("string \"%1\" with ").arg(m_ui->findEdit->text()) +
                tr("string \"%1\"").arg(m_ui->replaceEdit->text()));
            }
        }
        else
        {
            QMessageBox::critical(m_editor, tr("Find Error"), tr("String ")
            + tr("\"%1\" not found").arg(m_ui->findEdit->text()));
        }
    }
    else
    {
        m_hexThis->setUpdatesEnabled(false);

        if(replace() != -1)
        {
            QProgressDialog dialog(m_editor, Qt::MSWindowsFixedSizeDialogHint |
                                             Qt::WindowTitleHint |
                                             Qt::WindowSystemMenuHint |
                                             Qt::WindowCloseButtonHint);

            dialog.setWindowModality(Qt::ApplicationModal);
            dialog.setLabelText(tr("Replacing Text..."));
            dialog.setWindowTitle(tr("Replace All"));
            dialog.setRange(0, 0);

            dialog.show(); m_autoReplaceAll = true;

            int i = 1; for(; (replace() != -1) && (!dialog.wasCanceled()); i++)
            {
                QApplication::processEvents();
            }

            m_autoReplaceAll = false; dialog.hide();

            if(isVisible()) // Prevents popup if hidden already on error...
            {
                QMessageBox::information(m_editor, tr("Replace All"),
                tr("Replaced %Ln occurrence(s) of ", "", i) +
                tr("string \"%1\" with ").arg(m_ui->findEdit->text()) +
                tr("string \"%1\"").arg(m_ui->replaceEdit->text()));
            }
        }

        m_hexThis->setUpdatesEnabled(true);
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
