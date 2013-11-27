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

#include "propinoplugin.h"

using namespace Propino::Internal;

PropinoPlugin::PropinoPlugin()
{
    qRegisterMetaType<QFtdiInfo>(); qRegisterMetaType< QList<QFtdiInfo> >();
}

PropinoPlugin::~PropinoPlugin()
{
    m_getDevicesLock.tryLock(); m_getDevicesLock.unlock();
}

bool PropinoPlugin::initialize(const QStringList &arguments,
                               QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    QSplashScreen *splash = new
    QSplashScreen(QPixmap(Constants::PROPINO_SPLASH_PATH));

    connect(Core::ICore::instance(), SIGNAL(coreOpened()),
    splash, SLOT(close())); splash->show();

    // Setup Ftdi /////////////////////////////////////////////////////////////

    m_ftdi = new QFtdi(Core::ICore::mainWindow(), Core::ICore::settings());
    m_escape = new SerialEscape(m_ftdi, Core::ICore::settings());

    Core::ICore::statusBar()->addPermanentWidget(m_selectedBoardName =
    new QLabel(Core::ICore::statusBar())); updateSelectedBoardName(false);

    // Priority 0 //

    connect(m_ftdi, SIGNAL(deviceRemoved()),
            this, SLOT(pingDevice()));

    connect(m_ftdi, SIGNAL(aboutToClose()),
            this, SLOT(aboutToOpenCloseFdti()));

    // Priority 1 //

    connect(m_ftdi, SIGNAL(deviceArrived()),
            this, SLOT(startGetDevices()));

    connect(m_ftdi, SIGNAL(deviceRemoved()),
            this, SLOT(startGetDevices()));

    // Serial Escape //

    connect(m_escape, SIGNAL(error(QString)),
            this, SLOT(printError(QString)));

    // Other Stuff //

    addObject(m_ftdi); addObject(m_escape);

    Core::ICore::mainWindow()->installEventFilter(this);

    // Register Actions ///////////////////////////////////////////////////////

    // Board Menu //
    {
        m_boardMenu =
        Core::ActionManager::createMenu(Constants::BOARD_MENU_ID);

        m_boardMenu->
        setOnAllDisabledBehavior(Core::ActionContainer::Show);

        m_boardMenu->menu()->
        setTitle(tr("Board"));

        Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->
        addMenu(Core::ActionManager::actionContainer(Core::Constants::M_HELP),
        m_boardMenu);

        connect(m_boardMenu->menu(), SIGNAL(aboutToShow()),
                this, SLOT(startGetDevices()));
    }

    // Edit Board Settings Action //
    {
        m_editBoardSettings = Core::ActionManager::registerAction(
        new QAction(tr("Edit Board Settings"), m_boardMenu->menu()),
        Constants::EDIT_BOARD_SETTINGS_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_editBoardSettings);

        m_editBoardSettings->action()->setEnabled(false);

        connect(m_editBoardSettings->action(), SIGNAL(triggered()),
                this, SLOT(editBoardSettings()));
    }

    // Free Selected Board Action //
    {
        m_freeSelectedBoard = Core::ActionManager::registerAction(
        new QAction(tr("Free Selected Board"), m_boardMenu->menu()),
        Constants::FREE_SELECTED_BOARD_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_freeSelectedBoard);

        m_freeSelectedBoard->action()->setEnabled(false);

        connect(m_freeSelectedBoard->action(), SIGNAL(triggered()),
                this, SLOT(freeSelectedBoard()));
    }

    m_boardMenu->menu()->addSeparator();

    // Change Selected Board Menu //
    {
        m_changeSelectedBoardMenu =
        Core::ActionManager::createMenu(Constants::CHANGE_SELECTED_BOARD_ID);

        m_changeSelectedBoardMenu->
        setOnAllDisabledBehavior(Core::ActionContainer::Show);

        m_changeSelectedBoardMenu->menu()->
        setTitle(tr("Change Selected Board"));

        m_boardMenu->
        addMenu(
        m_changeSelectedBoardMenu);

        connect(m_changeSelectedBoardMenu->menu(), SIGNAL(triggered(QAction*)),
                this, SLOT(changedSelectedBoardTriggered(QAction*)));
    }

    m_boardMenu->menu()->addSeparator();

    // Open Serial Terminal //
    {
        m_openSerialTerminal = Core::ActionManager::registerAction(
        new QAction(tr("Open Serial Terminal"), m_boardMenu->menu()),
        Constants::OPEN_SERIAL_TERMINAL_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_openSerialTerminal);

        m_openSerialTerminal->action()->setEnabled(false);

        connect(m_openSerialTerminal->action(), SIGNAL(triggered()),
                m_escape->getTerminals().constBegin().value(), SLOT(show()));
    }

    m_boardMenu->menu()->addSeparator();

    // Edit Program Memory //
    {
        m_editProgramMemory = Core::ActionManager::registerAction(
        new QAction(tr("Edit Program Memory"), m_boardMenu->menu()),
        Constants::EDIT_PROGRAM_MEMORY_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_editProgramMemory);

        m_editProgramMemory->action()->setEnabled(false);

        connect(m_editProgramMemory->action(), SIGNAL(triggered()),
                this, SLOT(editProgramMemory()));
    }

    // Edit Storage Memory //
    {
        m_editStorageMemory = Core::ActionManager::registerAction(
        new QAction(tr("Edit Storage Memory"), m_boardMenu->menu()),
        Constants::EDIT_STORAGE_MEMORY_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_editStorageMemory);

        m_editStorageMemory->action()->setEnabled(false);

        connect(m_editStorageMemory->action(), SIGNAL(triggered()),
                this, SLOT(editStorageMemory()));
    }

    m_boardMenu->menu()->addSeparator();

    // Binary Loader //
    {
        m_binaryLoader = Core::ActionManager::registerAction(
        new QAction(tr("Binary Loader"), m_boardMenu->menu()),
        Constants::BINARY_LOADER_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_boardMenu->addAction(m_binaryLoader);

        m_binaryLoader->action()->setEnabled(false);

        connect(m_binaryLoader->action(), SIGNAL(triggered()),
                this, SLOT(binaryLoader()));
    }

    // Widgets Menu //
    {
        m_widgetsMenu =
        Core::ActionManager::createMenu(Constants::WIDGETS_MENU_ID);

        m_widgetsMenu->
        setOnAllDisabledBehavior(Core::ActionContainer::Show);

        m_widgetsMenu->menu()->
        setTitle(tr("Widgets"));

        Core::ActionManager::actionContainer(Core::Constants::MENU_BAR)->
        addMenu(Core::ActionManager::actionContainer(Core::Constants::M_HELP),
        m_widgetsMenu);

        connect(m_widgetsMenu->menu(), SIGNAL(aboutToShow()),
                this, SLOT(widgetsAboutToShow()));
    }

    // Remove Widget //
    {
        m_removeWidgetMenu =
        Core::ActionManager::createMenu(Constants::REMOVE_WIDGET_MENU_ID);

        m_removeWidgetMenu->
        setOnAllDisabledBehavior(Core::ActionContainer::Show);

        m_removeWidgetMenu->menu()->
        setTitle(tr("Remove Widget"));

        m_widgetsMenu->
        addMenu(
        m_removeWidgetMenu);
    }

    // Remove All Widgets //
    {
        m_removeAllWidgets = Core::ActionManager::registerAction(
        new QAction(tr("Remove All Widgets"), m_removeWidgetMenu->menu()),
        Constants::REMOVE_ALL_WIDGETS_ID,
        Core::Context(Core::Constants::C_GLOBAL));
        m_removeWidgetMenu->addAction(m_removeAllWidgets);

        m_removeAllWidgets->action()->setEnabled(false);

        connect(m_removeAllWidgets->action(), SIGNAL(triggered()),
                this, SLOT(removeAllWidgets()));
    }

    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()->
    removeAction(
    Core::ActionManager::command(Core::Constants::ABOUT_QTCREATOR)->action());
    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()->
    removeAction(
    Core::ActionManager::command(Core::Constants::ABOUT_PLUGINS)->action());

    // General Help //
    {
        m_generalHelp = Core::ActionManager::registerAction(
        new QAction(tr("General Help"),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        Constants::GENERAL_HELP_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->
        addAction(m_generalHelp);

        m_generalHelp->action()->setEnabled(true);

        connect(m_generalHelp->action(), SIGNAL(triggered()),
                this, SLOT(generalHelp()));
    }

    // Editor Help //
    {
        m_editorHelp = Core::ActionManager::registerAction(
        new QAction(tr("Editor Help"),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        Constants::EDITOR_HELP_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->
        addAction(m_editorHelp);

        m_editorHelp->action()->setEnabled(true);

        connect(m_editorHelp->action(), SIGNAL(triggered()),
                this, SLOT(editorHelp()));
    }

    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()
    ->addSeparator();

    // About Propino //
    {
        m_aboutPropino = Core::ActionManager::registerAction(
        new QAction(tr("About Propino..."),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        Constants::ABOUT_PROPINO_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->
        addAction(m_aboutPropino);

        m_aboutPropino->action()->setEnabled(true);

        connect(m_aboutPropino->action(), SIGNAL(triggered()),
                this, SLOT(about()));
    }

    // About Qt //
    {
        m_aboutQt = Core::ActionManager::registerAction(
        new QAction(tr("About Qt..."),
        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()),
        Constants::ABOUT_QT_ID,
        Core::Context(Core::Constants::C_GLOBAL));

        Core::ActionManager::actionContainer(Core::Constants::M_HELP)->
        addAction(m_aboutQt);

        m_aboutQt->action()->setEnabled(true);

        connect(m_aboutQt->action(), SIGNAL(triggered()),
                qApp, SLOT(aboutQt()));
    }

    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()
    ->addSeparator();

    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()->
    addAction(
    Core::ActionManager::command(Core::Constants::ABOUT_QTCREATOR)->action());
    Core::ActionManager::actionContainer(Core::Constants::M_HELP)->menu()->
    addAction(
    Core::ActionManager::command(Core::Constants::ABOUT_PLUGINS)->action());

    return true;
}

void PropinoPlugin::extensionsInitialized()
{
    QApplication::setOrganizationName(tr("Propino"));
    QApplication::setOrganizationDomain(tr("www.propino.org"));
    QApplication::setApplicationName(tr("Propino"));
    QApplication::setApplicationVersion(tr(PROPINO_VERSION_STR));

    QApplication::setWindowIcon(QIcon(Constants::PROPINO_LOGO_PATH));
}

bool PropinoPlugin::delayedInitialize()
{
    // Default Settings ///////////////////////////////////////////////////////
    {
        QSettings settings(Core::ICore::settings()->fileName(),
                           Core::ICore::settings()->format());

        settings.beginGroup(PROPINO_KEY_GROUP);
        {
            QString workspaceDirPath;

            settings.beginGroup(PROPINO_PLUGIN);
            {
                if(!settings.contains(PROPINO_PLUGIN_KEY_WDIR))
                {
                    if(QMessageBox::information(Core::ICore::mainWindow(),
                    tr("Propino Workspace"),
                    tr("Please specify a default location for Propino to "
                       "store all of your project files")) != QMessageBox::Ok)
                    {
                        QApplication::quit(); return true;
                    }

                    QDir wdir(QStandardPaths::writableLocation(
                              QStandardPaths::DocumentsLocation) +
                              PROPINO_PLUGIN_VAL_WDIR);

                    forever
                    {
                        UtilPathPicker pp(tr("Propino Workspace"),
                        tr("Please choose a folder"), wdir.path(),
                        Core::ICore::mainWindow());

                        if(pp.exec() != QDialog::Accepted)
                        {
                            QApplication::quit(); return true;
                        }

                        wdir.setPath(pp.getDefaultDir());

                        if(!QDir().mkpath(wdir.path()))
                        {
                            if(QMessageBox::critical(Core::ICore::mainWindow(),
                            tr("Propino Workspace"),
                            tr("Invalid directory path \"%1\"").
                               arg(wdir.path())) != QMessageBox::Ok)
                            {
                                QApplication::quit(); return true;
                            }

                            continue;
                        }

                        break;
                    }

                    settings.setValue(PROPINO_PLUGIN_KEY_WDIR, wdir.path());
                }

                workspaceDirPath = settings.value(PROPINO_PLUGIN_KEY_WDIR).
                                   toString();
            }
            settings.endGroup();

            settings.beginGroup(PROGRAM_BUILDER);
            {
                if(!settings.contains(PROGRAM_BUILDER_KEY_USER_CMAKE))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_USER_CMAKE,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_BUILDER_VAL_USER_CMAKE));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_USER_BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_USER_BIN,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_BUILDER_VAL_USER_BIN));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_USER_S1BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_USER_S1BIN,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_BUILDER_VAL_USER_S1BIN));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_USER_S2BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_USER_S2BIN,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_BUILDER_VAL_USER_S2BIN));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_SYSTEM_CMAKE))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_SYSTEM_CMAKE,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_BUILDER_VAL_SYSTEM_CMAKE));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_SYSTEM_BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_SYSTEM_BIN,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_BUILDER_VAL_SYSTEM_BIN));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_SYSTEM_S1BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_SYSTEM_S1BIN,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_BUILDER_VAL_SYSTEM_S1BIN));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_BUILDER_KEY_SYSTEM_S2BIN))
                {
                    QString temp;

                    settings.setValue(PROGRAM_BUILDER_KEY_SYSTEM_S2BIN,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_BUILDER_VAL_SYSTEM_S2BIN));

                    QDir().mkpath(temp);
                }
            }
            settings.endGroup();

            settings.beginGroup(PROGRAM_MEMORY);
            {
                if(!settings.contains(PROGRAM_MEMORY_KEY_UBSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_UBSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_MEMORY_VAL_UBSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_MEMORY_KEY_US1BSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_US1BSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_MEMORY_VAL_US1BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_MEMORY_KEY_US2BSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_US2BSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    PROGRAM_MEMORY_VAL_US2BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_MEMORY_KEY_SBSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_SBSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_MEMORY_VAL_SBSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_MEMORY_KEY_SS1BSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_SS1BSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_MEMORY_VAL_SS1BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(PROGRAM_MEMORY_KEY_SS2BSP))
                {
                    QString temp;

                    settings.setValue(PROGRAM_MEMORY_KEY_SS2BSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_MEMORY_VAL_SS2BSP));

                    QDir().mkpath(temp);
                }
            }
            settings.endGroup();

            settings.beginGroup(STORAGE_MEMORY);
            {
                if(!settings.contains(STORAGE_MEMORY_KEY_UBSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_UBSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    STORAGE_MEMORY_VAL_UBSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(STORAGE_MEMORY_KEY_US1BSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_US1BSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    STORAGE_MEMORY_VAL_US1BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(STORAGE_MEMORY_KEY_US2BSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_US2BSP,
                    temp = QDir::cleanPath(workspaceDirPath +
                    STORAGE_MEMORY_VAL_US2BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(STORAGE_MEMORY_KEY_SBSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_SBSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    STORAGE_MEMORY_VAL_SBSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(STORAGE_MEMORY_KEY_SS1BSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_SS1BSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    STORAGE_MEMORY_VAL_SS1BSP));

                    QDir().mkpath(temp);
                }

                if(!settings.contains(STORAGE_MEMORY_KEY_SS2BSP))
                {
                    QString temp;

                    settings.setValue(STORAGE_MEMORY_KEY_SS2BSP,
                    temp = QDir::cleanPath(QApplication::applicationDirPath() +
                    STORAGE_MEMORY_VAL_SS2BSP));

                    QDir().mkpath(temp);
                }
            }
            settings.endGroup();

            settings.beginGroup(BOARD_SETTINGS);
            {
                if(!settings.contains(BOARD_SETTINGS_KEY_OPEN_USER_FILE))
                {
                    settings.setValue(BOARD_SETTINGS_KEY_OPEN_USER_FILE,
                    QDir::cleanPath(workspaceDirPath +
                    BOARD_SETTINGS_VAL_OPEN_USER_FILE));
                }

                if(!settings.contains(BOARD_SETTINGS_KEY_OPEN_SYSTEM_FILE))
                {
                    settings.setValue(BOARD_SETTINGS_KEY_OPEN_SYSTEM_FILE,
                    QDir::cleanPath(QApplication::applicationDirPath() +
                    BOARD_SETTINGS_VAL_OPEN_SYSTEM_FILE));
                }
            }
            settings.endGroup();

            settings.beginGroup(PROGRAM_LOADER);
            {
                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_USER_F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_F,
                    QDir::cleanPath(workspaceDirPath +
                    PROGRAM_LOADER_VAL_OPEN_USER_F));
                }

                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_USER_S1F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_S1F,
                    QDir::cleanPath(workspaceDirPath +
                    PROGRAM_LOADER_VAL_OPEN_USER_S1F));
                }

                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_USER_S2F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_USER_S2F,
                    QDir::cleanPath(workspaceDirPath +
                    PROGRAM_LOADER_VAL_OPEN_USER_S2F));
                }

                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_SYSTEM_F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_F,
                    QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_LOADER_VAL_OPEN_SYSTEM_F));
                }

                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S1F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S1F,
                    QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_LOADER_VAL_OPEN_SYSTEM_S1F));
                }

                if(!settings.contains(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S2F))
                {
                    settings.setValue(PROGRAM_LOADER_KEY_OPEN_SYSTEM_S2F,
                    QDir::cleanPath(QApplication::applicationDirPath() +
                    PROGRAM_LOADER_VAL_OPEN_SYSTEM_S2F));
                }
            }
            settings.endGroup();
        }
        settings.endGroup();
    }

    SerialOscilloscope::initFttw(Core::ICore::settings(),
                                 Core::ICore::mainWindow());

    // Default Port ///////////////////////////////////////////////////////////
    {
        QList<QFtdiInfo> devs; QString errorString;

        if(QFtdi::getDevices(devs, Core::ICore::settings(), errorString))
        {
            QSettings settings(Core::ICore::settings()->fileName(),
                               Core::ICore::settings()->format());

            settings.beginGroup(PROPINO_PLUGIN_KEY_GROUP);

            if(settings.contains(PROPINO_PLUGIN_KEY_SBSN))
            {
                QString serialNumber =
                        settings.value(PROPINO_PLUGIN_KEY_SBSN).toString();

                if(!serialNumber.isEmpty())
                {
                    foreach(QFtdiInfo info, devs)
                    {
                        if(info.node.SerialNumber == serialNumber)
                        {
                            m_ftdi->setFtdiInfo(info); openFtdi(); break;
                        }
                    }
                }
            }
            else if(devs.size() > 0)
            {
                QMessageBox::StandardButton button;

                if(devs.size() > 1)
                {
                    button = QMessageBox::question(Core::ICore::mainWindow(),
                    tr("Open Port"), tr("Propino found microcontroller boards "
                    "attached to your computer. Would you like to connect to "
                    "one?"), QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Ok);

                    if(button == QMessageBox::Ok)
                    {
                        QMap<QString, QVariant> data;

                        foreach(QFtdiInfo io, devs)
                        {
                            data.insertMulti(io.name, qVariantFromValue(io));
                        }

                        UtilItemPicker ip(tr("Open Port"), tr("Please select "
                        "a board"), data, Core::ICore::mainWindow());

                        if(ip.exec() == QDialog::Accepted)
                        {
                            m_ftdi->setFtdiInfo(qVariantValue<QFtdiInfo>
                                                (ip.getSelectedItem()));

                            openFtdi();
                        }
                    }
                }
                else
                {
                    button = QMessageBox::question(Core::ICore::mainWindow(),
                    tr("Open Port"), tr("Propino found a microcontroller "
                    "board attached to your computer. Would you like to "
                    "connect to it?"), QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Ok);

                    if(button == QMessageBox::Ok)
                    {
                        m_ftdi->setFtdiInfo(devs.first()); openFtdi();
                    }
                }
            }
        }
    }

    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag PropinoPlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void PropinoPlugin::editBoardSettings()
{
    BoardSettingsDialog dialog(m_ftdi,
                               Core::ICore::settings(),
                               Core::ICore::mainWindow()); dialog.exec();

    updateSelectedBoardName(m_ftdi->isOpen());
}

void PropinoPlugin::freeSelectedBoard()
{
    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PROPINO_PLUGIN_KEY_GROUP);
    settings.setValue(PROPINO_PLUGIN_KEY_SBSN, ""); m_ftdi->close();
}

void PropinoPlugin::changedSelectedBoardTriggered(QAction *action)
{
    m_ftdi->setFtdiInfo(qVariantValue<QFtdiInfo>(action->data()));

    openFtdi();
}

void PropinoPlugin::editProgramMemory()
{
    // Load Settings //////////////////////////////////////////////////////////

    m_escape->setEnabled(false);

    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(PROGRAM_MEMORY_KEY_GROUP);

    // Initial Setup //////////////////////////////////////////////////////////

    if(!settings.contains(PROGRAM_MEMORY_KEY_LP))
    {
        QMessageBox box(QMessageBox::Information,
        tr("View/Edit Program Memory"),
        tr("Please select the loader protocol and binaries to use."),
        QMessageBox::Ok | QMessageBox::Cancel, Core::ICore::mainWindow());
        box.setDefaultButton(QMessageBox::Ok);
        connect(m_ftdi, SIGNAL(aboutToClose()), &box, SLOT(reject()));

        if(box.exec() != QMessageBox::Ok)
        {
            m_escape->setEnabled(true); return;
        }

        ProgramLoaderDialog dialog(m_ftdi, Core::ICore::settings(),
        Core::ICore::mainWindow(), PROGRAM_MEMORY);
        dialog.setLoadButtonVisibility(false);

        if(dialog.exec() == QDialog::Rejected)
        {
            m_escape->setEnabled(true); return;
        }
    }

    // Default Setup //////////////////////////////////////////////////////////

    forever
    {
        QMessageBox box(QMessageBox::Question, tr("View/Edit Program Memory"),
        tr("Load the program memory editor?"), QMessageBox::Ok |
        QMessageBox::Cancel, Core::ICore::mainWindow());

        box.setDefaultButton(QMessageBox::Ok); QPushButton *settingsButton =
        box.addButton(tr("Settings"), QMessageBox::ActionRole);
        connect(m_ftdi, SIGNAL(aboutToClose()), &box, SLOT(reject()));

        int result = box.exec();

        if(box.clickedButton() == settingsButton)
        {
            ProgramLoaderDialog dialog(m_ftdi, Core::ICore::settings(),
            Core::ICore::mainWindow(), PROGRAM_MEMORY);
            dialog.setLoadButtonVisibility(false);

            if(dialog.exec() == QDialog::Rejected)
            {
                m_escape->setEnabled(true); return;
            }

            continue;
        }
        else if(result != QMessageBox::Ok)
        {
           m_escape->setEnabled(true); return;
        }

        break;
    }

    // Load Memory ////////////////////////////////////////////////////////////

    ProgramLoaderDialog load(m_ftdi, Core::ICore::settings(),
    Core::ICore::mainWindow(), PROGRAM_MEMORY);

    QProgressDialog dialog(Core::ICore::mainWindow());
    SerialLoader loader(m_ftdi, &dialog);

    load.setResult(QDialog::Rejected);
    load.loadBinary(&dialog, &loader);

    if(load.result() == QDialog::Accepted)
    {
        BinaryEditor edit(tr("Program Memory"), Core::ICore::settings(),
        Core::ICore::mainWindow(), PROGRAM_MEMORY);

        edit.setWindowModality(Qt::ApplicationModal);

        if(edit.memoryInit(m_ftdi, &dialog))
        {
            QEventLoop loop;

            connect(&edit, SIGNAL(closed()), &loop, SLOT(quit()));
            dialog.hide(); edit.show(); edit.warn(); loop.exec();

            if(m_ftdi->isOpen())
            {
                edit.runCommand(BinaryEditor::boot_cmd);
            }
        }
    }

    m_escape->setEnabled(true);
}

void PropinoPlugin::editStorageMemory()
{
    // Load Settings //////////////////////////////////////////////////////////

    m_escape->setEnabled(false);

    QSettings settings(Core::ICore::settings()->fileName(),
                       Core::ICore::settings()->format());

    settings.beginGroup(STORAGE_MEMORY_KEY_GROUP);

    // Initial Setup //////////////////////////////////////////////////////////

    if(!settings.contains(STORAGE_MEMORY_KEY_LP))
    {
        QMessageBox box(QMessageBox::Information,
        tr("View/Edit Storage Memory"),
        tr("Please select the loader protocol and binaries to use."),
        QMessageBox::Ok | QMessageBox::Cancel, Core::ICore::mainWindow());
        box.setDefaultButton(QMessageBox::Ok);
        connect(m_ftdi, SIGNAL(aboutToClose()), &box, SLOT(reject()));

        if(box.exec() != QMessageBox::Ok)
        {
            m_escape->setEnabled(true); return;
        }

        ProgramLoaderDialog dialog(m_ftdi, Core::ICore::settings(),
        Core::ICore::mainWindow(), STORAGE_MEMORY);
        dialog.setLoadButtonVisibility(false);

        if(dialog.exec() == QDialog::Rejected)
        {
            m_escape->setEnabled(true); return;
        }
    }

    // Default Setup //////////////////////////////////////////////////////////

    forever
    {
        QMessageBox box(QMessageBox::Question, tr("View/Edit Storage Memory"),
        tr("Load the storage memory editor?"), QMessageBox::Ok |
        QMessageBox::Cancel, Core::ICore::mainWindow());

        box.setDefaultButton(QMessageBox::Ok); QPushButton *settingsButton =
        box.addButton(tr("Settings"), QMessageBox::ActionRole);
        connect(m_ftdi, SIGNAL(aboutToClose()), &box, SLOT(reject()));

        int result = box.exec();

        if(box.clickedButton() == settingsButton)
        {
            ProgramLoaderDialog dialog(m_ftdi, Core::ICore::settings(),
            Core::ICore::mainWindow(), STORAGE_MEMORY);
            dialog.setLoadButtonVisibility(false);

            if(dialog.exec() == QDialog::Rejected)
            {
                m_escape->setEnabled(true); return;
            }

            continue;
        }
        else if(result != QMessageBox::Ok)
        {
           m_escape->setEnabled(true); return;
        }

        break;
    }

    // Load Memory ////////////////////////////////////////////////////////////

    ProgramLoaderDialog load(m_ftdi, Core::ICore::settings(),
    Core::ICore::mainWindow(), STORAGE_MEMORY);

    QProgressDialog dialog(Core::ICore::mainWindow());
    SerialLoader loader(m_ftdi, &dialog);

    load.setResult(QDialog::Rejected);
    load.loadBinary(&dialog, &loader);

    if(load.result() == QDialog::Accepted)
    {
        BinaryEditor edit(tr("Storage Memory"), Core::ICore::settings(),
        Core::ICore::mainWindow(), STORAGE_MEMORY);

        edit.setWindowModality(Qt::ApplicationModal);

        if(edit.memoryInit(m_ftdi, &dialog))
        {
            QEventLoop loop;

            connect(&edit, SIGNAL(closed()), &loop, SLOT(quit()));
            dialog.hide(); edit.show(); edit.warn(); loop.exec();

            if(m_ftdi->isOpen())
            {
                edit.runCommand(BinaryEditor::boot_cmd);
            }
        }
    }

    m_escape->setEnabled(true);
}

void PropinoPlugin::binaryLoader()
{
    m_escape->setEnabled(false);

    ProgramLoaderDialog dialog(m_ftdi,
                               Core::ICore::settings(),
                               Core::ICore::mainWindow()); dialog.exec();

    updateSelectedBoardName(m_ftdi->isOpen());

    m_escape->setEnabled(true);
}

void PropinoPlugin::widgetsAboutToShow()
{
    {
        QMap<QString, QAction*> map;

        foreach(SerialOscilloscope *oscilloscope,
                m_escape->getOscilloscopes().values())
        {
            QAction *action = new QAction(oscilloscope->windowTitle(),
                                          m_widgetsMenu->menu());

            action->setCheckable(true);
            action->setChecked(oscilloscope->isVisible());
            action->setEnabled(m_ftdi->isOpen());
            action->setData(qVariantFromValue(oscilloscope));

            connect(action, SIGNAL(triggered()),
                    this, SLOT(widgetsTriggered()));

            map.insertMulti(oscilloscope->windowTitle(), action);
        }

        foreach(SerialTerminal *terminal,
                m_escape->getTerminals().values())
        {
            QAction *action = new QAction(terminal->windowTitle(),
                                          m_widgetsMenu->menu());

            action->setCheckable(true);
            action->setChecked(terminal->isVisible());
            action->setEnabled(m_ftdi->isOpen());
            action->setData(qVariantFromValue(terminal));

            connect(action, SIGNAL(triggered()),
                    this, SLOT(widgetsTriggered()));

            map.insertMulti(terminal->windowTitle(), action);
        }

        m_widgetsMenu->menu()->clear();
        m_widgetsMenu->menu()->addActions(map.values());

        m_widgetsMenu->menu()->addSeparator();
        m_widgetsMenu->menu()->addMenu(m_removeWidgetMenu->menu());
    }

    {
        QMap<QString, QAction*> map;

        foreach(SerialOscilloscope *oscilloscope,
                m_escape->getOscilloscopes().values())
        {
            // Don't delete default oscilloscope

            if(oscilloscope
            != m_escape->getOscilloscopes().constBegin().value())
            {
                QAction *action = new QAction(oscilloscope->windowTitle(),
                                              m_removeWidgetMenu->menu());

                action->setData(qVariantFromValue(oscilloscope));

                connect(action, SIGNAL(triggered()),
                        this, SLOT(removeWidgetTriggered()));

                map.insertMulti(oscilloscope->windowTitle(), action);
            }
        }

        foreach(SerialTerminal *terminal,
                m_escape->getTerminals().values())
        {
            // Don't delete default terminal

            if(terminal
            != m_escape->getTerminals().constBegin().value())
            {
                QAction *action = new QAction(terminal->windowTitle(),
                                              m_removeWidgetMenu->menu());

                action->setData(qVariantFromValue(terminal));

                connect(action, SIGNAL(triggered()),
                        this, SLOT(removeWidgetTriggered()));

                map.insertMulti(terminal->windowTitle(), action);
            }
        }

        m_removeWidgetMenu->menu()->clear();
        m_removeWidgetMenu->menu()->addActions(map.values());

        m_removeWidgetMenu->menu()->setDisabled(map.isEmpty());

        m_removeWidgetMenu->menu()->addSeparator();
        m_removeWidgetMenu->menu()->addAction(m_removeAllWidgets->action());

        m_removeAllWidgets->action()->setDisabled(map.isEmpty());
    }
}

void PropinoPlugin::widgetsTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QWidget *widget = qVariantValue<QWidget*>(action->data());

    widget->setVisible(!widget->isVisible());
}

void PropinoPlugin::removeWidgetTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QWidget *widget = qVariantValue<QWidget*>(action->data());

    widget->setAttribute(Qt::WA_DeleteOnClose, true); widget->close();
}

void PropinoPlugin::removeAllWidgets()
{
    foreach(SerialOscilloscope *oscilloscope,
            m_escape->getOscilloscopes().values())
    {
        // Don't delete default oscilloscope

        if(oscilloscope != m_escape->getOscilloscopes().constBegin().value())
        {
            m_escape->deleteOscilloscope(oscilloscope);
        }
    }

    foreach(SerialTerminal *terminal,
            m_escape->getTerminals().values())
    {
        // Don't delete default terminal

        if(terminal != m_escape->getTerminals().constBegin().value())
        {
            m_escape->deleteTerminal(terminal);
        }
    }
}

void PropinoPlugin::generalHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Open General Help Error"),
        tr("Unable to open the URL to the Propino General Help page"));
    }
}

void PropinoPlugin::editorHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    "http://www.propino.org/")))
    {
        QMessageBox::critical(Core::ICore::mainWindow(),
        tr("Open Editor Help Error"),
        tr("Unable to open the URL to the Propino Editor Help page"));
    }
}

void PropinoPlugin::about()
{
    QMessageBox::about(Core::ICore::mainWindow(), tr("About Propino %1").
    arg(QApplication::applicationVersion()),
    tr("<h3>About Propino %1</h3>"
    ""
    "<p>Copyright 2013 by Kwabena W. Agyeman. All rights reserved.</p>"
    ""
    "<h4>Acknowledgments</h4>"
    ""
    "<p>GUI powered by <a href=\"http://www.qt-project.org/\">Qt</a>.</p>"
    ""
    "<p>Editor powered by "
    "<a href=\"http://www.qt-project.org/\">Qt Creator</a>.</p>"
    ""
    "<p>Serial port library powered by "
    "<a href=\"http://www.ftdichip.com/\">FTDI</a>.</p>"
    ""
    "<p>Qt plotting widget powered by "
    "<a href=\"http://www.qcustomplot.com/\">QCustomPlot</a>.</p>"
    ""
    "<p>FFT subroutine library powered by "
    "<a href=\"http://www.fftw.org/\">FFTW</a>.</p>"
    ""
    "<p>Build system powered by "
    "<a href=\"http://www.cmake.org/\">CMake</a>.</p>"
    ""
    "<p>Propeller compiler powered by "
    "<a href=\"https://code.google.com/p/propgcc/\">PropGCC</a>.</p>"
    ""
    "<p>Installer powered by "
    "<a href=\"http://installbuilder.bitrock.com/\">BitRock</a>.</p>"
    ""
    "<h4>Questions or Comments?</h4>"
    ""
    "<p>Contact us at "
    "<a href=\"mailto:propino@propino.org\">propino@propino.org</a>.</p>").
    arg(QApplication::applicationVersion()));
}

void PropinoPlugin::startGetDevices()
{
    if(m_getDevicesLock.tryLock())
    {
        GetDevices *getDevices = new GetDevices; QThread *thread = new QThread;

        getDevices->moveToThread(thread);

        connect(thread, SIGNAL(started()), getDevices, SLOT(start()));
        connect(getDevices, SIGNAL(finished()), thread, SLOT(quit()));
        connect(thread, SIGNAL(finished()), getDevices, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        connect(getDevices, SIGNAL(devices(bool,QList<QFtdiInfo>,QString)),
                this, SLOT(endGetDevices(bool,QList<QFtdiInfo>,QString)));

        thread->start();

        if(sender() == m_ftdi)
        {
            QTimer::singleShot(RESCAN_WAIT, this, SLOT(startGetDevices()));

            m_getDevicesProgress = QFutureInterface<void>();
            m_getDevicesProgress.setProgressRange(0, 2);

            Core::ICore::progressManager()->addTask(m_getDevicesProgress.
            future(), tr("Rescanning"), Constants::GET_DEVICES_ID);

            m_getDevicesProgress.reportStarted();
            m_getDevicesProgress.setProgressValue(1);
        }
    }
}

void PropinoPlugin::endGetDevices(bool result,
                                  const QList<QFtdiInfo> &devs,
                                  const QString &errorString)
{
    // Regular Code ///////////////////////////////////////////////////////////
    {
        QMap<QString, QAction*> map;

        if(result)
        {
            foreach(QFtdiInfo info, devs)
            {
                QAction *action = new QAction(info.name,
                m_changeSelectedBoardMenu->menu());

                action->setCheckable(false);
                action->setChecked(false);
                action->setDisabled(false);
                action->setData(qVariantFromValue(info));

                map.insertMulti(info.name, action);
            }

            if(m_ftdi->isOpen())
            {
                FT_DEVICE type; DWORD ID; QString number; QString description;

                if(m_ftdi->getDeviceInfo(&type, &ID, number, description))
                {
                    // Ping Success

                    QAction *action = new QAction(m_ftdi->getFtdiInfo().name,
                    m_changeSelectedBoardMenu->menu());

                    action->setCheckable(true);
                    action->setChecked(true);
                    action->setDisabled(true);
                    action->setData(qVariantFromValue(m_ftdi->getFtdiInfo()));

                    map.insertMulti(m_ftdi->getFtdiInfo().name, action);
                }
                else
                {
                    // Ping Failure

                    m_ftdi->close();
                }
            }
        }
        else
        {
            QAction *action = new QAction(errorString,
            m_changeSelectedBoardMenu->menu());

            action->setCheckable(false);
            action->setChecked(false);
            action->setEnabled(false);
            action->setData(qVariantFromValue(QFtdiInfo()));

            map.insertMulti(errorString, action);
        }

        m_changeSelectedBoardMenu->menu()->setUpdatesEnabled(false);

        m_changeSelectedBoardMenu->menu()->clear();
        m_changeSelectedBoardMenu->menu()->addActions(map.values());
        m_changeSelectedBoardMenu->menu()->setDisabled(map.isEmpty());

        m_changeSelectedBoardMenu->menu()->setUpdatesEnabled(true);

        m_getDevicesProgress.setProgressValue(2);
        m_getDevicesProgress.reportFinished();

        m_getDevicesLock.unlock();
    }

    // Extra Code /////////////////////////////////////////////////////////////
    {
        QSettings settings(Core::ICore::settings()->fileName(),
                           Core::ICore::settings()->format());

        settings.beginGroup(PROPINO_PLUGIN_KEY_GROUP);

        if(settings.contains(PROPINO_PLUGIN_KEY_SBSN))
        {
            QString serialNumber =
                    settings.value(PROPINO_PLUGIN_KEY_SBSN).toString();

            if(!serialNumber.isEmpty())
            {
                foreach(QFtdiInfo info, devs)
                {
                    if(info.node.SerialNumber == serialNumber)
                    {
                        m_ftdi->setFtdiInfo(info); openFtdi(); break;
                    }
                }
            }
        }
        else if(devs.size() > 0)
        {
            QMessageBox::StandardButton button;

            if(devs.size() > 1)
            {
                button = QMessageBox::question(Core::ICore::mainWindow(),
                tr("Open Port"), tr("Propino found microcontroller boards "
                "attached to your computer. Would you like to connect to "
                "one?"), QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Ok);

                if(button == QMessageBox::Ok)
                {
                    QMap<QString, QVariant> data;

                    foreach(QFtdiInfo io, devs)
                    {
                        data.insertMulti(io.name, qVariantFromValue(io));
                    }

                    UtilItemPicker ip(tr("Open Port"), tr("Please select "
                    "a board"), data, Core::ICore::mainWindow());

                    if(ip.exec() == QDialog::Accepted)
                    {
                        m_ftdi->setFtdiInfo(qVariantValue<QFtdiInfo>
                                            (ip.getSelectedItem()));

                        openFtdi();
                    }
                }
            }
            else
            {
                button = QMessageBox::question(Core::ICore::mainWindow(),
                tr("Open Port"), tr("Propino found a microcontroller "
                "board attached to your computer. Would you like to "
                "connect to it?"), QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Ok);

                if(button == QMessageBox::Ok)
                {
                    m_ftdi->setFtdiInfo(devs.first()); openFtdi();
                }
            }
        }
    }
}

void PropinoPlugin::pingDevice()
{
    if(m_ftdi->isOpen())
    {
        FT_DEVICE type; DWORD ID; QString number; QString description;

        if(!m_ftdi->getDeviceInfo(&type, &ID, number, description))
        {
            m_ftdi->close();
        }
    }
}

void PropinoPlugin::aboutToOpenCloseFdti(bool open)
{
      m_editBoardSettings->action()->setEnabled(open);
      m_freeSelectedBoard->action()->setEnabled(open);
      m_openSerialTerminal->action()->setEnabled(open);
      m_editProgramMemory->action()->setEnabled(open);
      m_editStorageMemory->action()->setEnabled(open);
      m_binaryLoader->action()->setEnabled(open);

      updateSelectedBoardName(open);
}

void PropinoPlugin::printError(const QString &errorMessage)
{
    Core::ICore::messageManager()->printToOutputPane(errorMessage,
    Core::MessageManager::PrintToOutputPaneFlags(
    Core::MessageManager::ModeSwitch |
    Core::MessageManager::WithFocus |
    Core::MessageManager::EnsureSizeHint));
}

bool PropinoPlugin::eventFilter(QObject *object, QEvent *event)
{
    if((object == Core::ICore::mainWindow())
    && (event->type() == QEvent::Close))
    {
        m_ftdi->close();

        removeObject(m_escape); delete m_escape;
        removeObject(m_ftdi); delete m_ftdi;

        SerialOscilloscope::finiFttw();
    }

    return false;
}

void PropinoPlugin::openFtdi()
{
    DWORD time = calcUSBRWTimeouts(FT_BAUD_115200);
    DWORD size = calcUSBParameters(FT_BAUD_115200);

    if(m_ftdi->open(QIODevice::ReadWrite)
    && m_ftdi->setDataCharacteristics(FT_BITS_8,
                                      FT_STOP_BITS_1,
                                      FT_PARITY_NONE)
    && m_ftdi->setFlowControl(FT_FLOW_NONE, XON_CHAR, XOFF_CHAR)
    && m_ftdi->setBaudRate(FT_BAUD_115200)
    && m_ftdi->setTimeouts(time, time)
    && m_ftdi->setUSBParameters(size, size)
    && m_ftdi->setDtr(true)
    && m_ftdi->setRts(true)
    && m_ftdi->setInTask(false, 10))
    {
        QThread::msleep(100);

        if((!m_ftdi->setRts(false))
        || (!m_ftdi->setDtr(false)))
        {
            QString errorString = m_ftdi->errorString(); m_ftdi->close();
            QMessageBox::critical(Core::ICore::mainWindow(), tr("Open Port"),
            errorString); return;
        }

        QThread::msleep(100);

        if((!m_ftdi->purge(FT_PURGE_RX | FT_PURGE_TX))
        || (!m_ftdi->setInTask(true, 10)))
        {
            QString errorString = m_ftdi->errorString(); m_ftdi->close();
            QMessageBox::critical(Core::ICore::mainWindow(), tr("Open Port"),
            errorString); return;
        }

        aboutToOpenCloseFdti(true);

        QSettings settings(Core::ICore::settings()->fileName(),
                           Core::ICore::settings()->format());

        settings.beginGroup(PROPINO_PLUGIN_KEY_GROUP);
        settings.setValue(PROPINO_PLUGIN_KEY_SBSN,
        m_ftdi->getFtdiInfo().node.SerialNumber);
        settings.endGroup();

        settings.beginGroup(PROPINO_FTDI_KEY_GROUP);
        settings.beginGroup(m_ftdi->getFtdiInfo().node.SerialNumber);

        if(!settings.contains(PROPINO_FTDI_KEY_NAME))
        {
            QMessageBox messageBox(QMessageBox::Question, tr("Open Port"),
            tr("Would you like to name this new board?"), QMessageBox::Yes |
            QMessageBox::No, Core::ICore::mainWindow());
            messageBox.setDefaultButton(QMessageBox::Yes);

            connect(m_ftdi, SIGNAL(aboutToClose()),
                    &messageBox, SLOT(reject()));

            if(messageBox.exec() == QMessageBox::Yes)
            {
                editBoardSettings();
            }
        }
    }
    else
    {
        QString errorString = m_ftdi->errorString(); m_ftdi->close();
        QMessageBox::critical(Core::ICore::mainWindow(), tr("Open Port"),
        errorString);
    }
}

void PropinoPlugin::updateSelectedBoardName(bool open)
{
    if(open)
    {
        m_selectedBoardName->setText(
        tr("Selected Board: ") + m_ftdi->getFtdiInfo().name + " ");
    }
    else
    {
        m_selectedBoardName->setText(
        tr("Selected Board: No Available Board Selected "));
    }
}

Q_EXPORT_PLUGIN2(Propino, PropinoPlugin)

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
