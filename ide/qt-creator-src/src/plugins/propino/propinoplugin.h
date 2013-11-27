/***************************************************************************//**
* @file
* Propino Plugin
*
* @version @n 1.0
* @date @n 9/15/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 9/15/2013
*******************************************************************************/

#ifndef PROPINO_H
#define PROPINO_H

#include <QtConcurrent>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "propinoconstants.h"
#include "propino_global.h"
#include "propino_settings.h"
#include "propino_shared.h"

#include "boardsettingsdialog.h"
#include "programloaderdialog.h"
#include "binaryeditor.h"

#include "qftdi.h"
#include "serialescape.h"

#include "utilitempicker.h"
#include "utilpathpicker.h"

#include <extensionsystem/iplugin.h>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/id.h>

#include <coreplugin/messagemanager.h>
#include <coreplugin/progressmanager/futureprogress.h>
#include <coreplugin/progressmanager/progressmanager.h>

#define RESCAN_WAIT 5000 // 5 seconds works for windows

namespace Propino {
namespace Internal {

// Get Devices Class //////////////////////////////////////////////////////////

class GetDevices : public QObject
{
    Q_OBJECT

public:

    GetDevices(QObject *parent = NULL) : QObject(parent) { }

public slots:

    void start()
    {
        QList<QFtdiInfo> devs; QString errorString;

        bool result = QFtdi::getDevices(devs,
        Core::ICore::settings(), errorString);

        emit devices(result, devs, errorString); emit finished();
    }

signals:

    void devices(bool result,
                 const QList<QFtdiInfo> &devs,
                 const QString &errorString);

    void finished();

private:

    Q_DISABLE_COPY(GetDevices)
};

// Propino Plugin Class ///////////////////////////////////////////////////////

class PropinoPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" \
                      FILE "Propino.json")

public:

    PropinoPlugin();
    ~PropinoPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();

private slots:

    // Board Menu /////////////////////////////////////////////////////////////

    void editBoardSettings();
    void freeSelectedBoard();

    void changedSelectedBoardTriggered(QAction *action);

    void editProgramMemory();
    void editStorageMemory();

    void binaryLoader();

    // Widgets Menu ///////////////////////////////////////////////////////////

    void widgetsAboutToShow();
    void widgetsTriggered();
    void removeWidgetTriggered();
    void removeAllWidgets();

    // Help Menu //////////////////////////////////////////////////////////////

    void generalHelp();
    void editorHelp();

    void about();

    // Propino Internal ///////////////////////////////////////////////////////

    void startGetDevices();
    void endGetDevices(bool result,
                       const QList<QFtdiInfo> &devs,
                       const QString &errorString);

    void pingDevice();
    void aboutToOpenCloseFdti(bool open = false);

    void printError(const QString &errorMessage);

protected:

    bool eventFilter(QObject *object, QEvent *event);

private:

    Q_DISABLE_COPY(PropinoPlugin)

    // Menu Data //////////////////////////////////////////////////////////////

    Core::ActionContainer *m_boardMenu;

    Core::Command *m_editBoardSettings;
    Core::Command *m_freeSelectedBoard;
    Core::ActionContainer *m_changeSelectedBoardMenu;
    Core::Command *m_openSerialTerminal;

    Core::Command *m_editProgramMemory;
    Core::Command *m_editStorageMemory;
    Core::Command *m_binaryLoader;

    Core::ActionContainer *m_widgetsMenu;

    Core::ActionContainer *m_removeWidgetMenu;
    Core::Command *m_removeAllWidgets;

    Core::Command *m_generalHelp;
    Core::Command *m_editorHelp;

    Core::Command *m_aboutPropino;
    Core::Command *m_aboutQt;

    // Ftdi Data //////////////////////////////////////////////////////////////

    QFtdi *m_ftdi; SerialEscape *m_escape; void openFtdi();

    QMutex m_getDevicesLock; QFutureInterface<void> m_getDevicesProgress;

    QLabel *m_selectedBoardName; void updateSelectedBoardName(bool open);
};

} // namespace Internal
} // namespace Propino

#endif // PROPINO_H

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
