//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
//============================================================================

#include <string>
#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStyleFactory>
#include <QPixmap>

#include "File.hpp"
#include "DiagData.hpp"
#include "MainWindow.hpp"
#include "ServerHandler.hpp"
#include "MenuHandler.hpp"
#include "InfoPanelHandler.hpp"
#include "InputEventLog.hpp"
#include "DirectoryHandler.hpp"
#include "Highlighter.hpp"
#include "NodeQueryHandler.hpp"
#include "CustomCommandHandler.hpp"
#include "Palette.hpp"
#include "ServerList.hpp"
#include "VConfig.hpp"
#include "VIcon.hpp"
#include "VServerSettings.hpp"
#include "VSettingsLoader.hpp"
#include "SessionHandler.hpp"
#include "SessionDialog.hpp"
#include "UiLog.hpp"

int main(int argc, char **argv)
{
    //if (argc != 3)
    //{
    //  std::cout << "Usage:" << std::endl;
     //   std::cout << argv[0] << " <host> <port>" << std::endl;
    //    return 1;
    //

    //Init qt
    QApplication app(argc, argv);

    app.setWindowIcon(QPixmap(":/viewer/logo_small.png"));

    QStringList styleLst=QStyleFactory::keys();

    //Set the style
    QString style="Plastique";
    if(styleLst.contains(style))
    {
    	app.setStyle(style);
    }
    else
    {
    	style="Fusion";
    	if(styleLst.contains(style))
        {    		
            app.setStyle(style);
        }
    }

    //Set font size for application
    //QFont font=app.font();
    //font.setPointSize(9);
    //app.setFont(font);

    //Initialise the config and other paths
    std::string exe(argv[0]);
    DirectoryHandler::init(exe);  // we need to tell the Directory class where we started from

    //Set the stylesheet
    std::string styleSheetFileName="viewer.qss";
    std::string styleSheetPath=DirectoryHandler::concatenate(DirectoryHandler::etcDir(),styleSheetFileName);

    QFile shFile(QString::fromStdString(styleSheetPath));
    if(shFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    	 app.setStyleSheet(shFile.readAll());
    }
    shFile.close();

    //Set fontsize if defined in env var
    if(const char* fontSizeCh=getenv("ECFLOWUI_FONT_SIZE"))
    {
        int fontSize=atoi(fontSizeCh);
        if(fontSize < 8)
            fontSize=8;
        else if(fontSize > 32)
            fontSize=32;
        QFont f=app.font();
        f.setPointSize(fontSize);
        app.setFont(f);
    }

    //Load the configurable menu items
    std::string menuFilename("ecflowview_menus.json");
    std::string menuPath = DirectoryHandler::concatenate(DirectoryHandler::etcDir(), menuFilename);
    MenuHandler::readMenuConfigFile(menuPath);

    //Load the custom context menu commands
    CustomCommandHistoryHandler::instance()->init();
    CustomSavedCommandHandler::instance()->init();
    MenuHandler::refreshCustomMenuCommands();

    //Load the info panel definition
    std::string panelFile = DirectoryHandler::concatenate(DirectoryHandler::etcDir(), "ecflowview_panels.json");
    InfoPanelHandler::instance()->init(panelFile);

    //Load the queries
    std::string queryDir = DirectoryHandler::concatenate(DirectoryHandler::configDir(), "query");
    NodeQueryHandler::instance()->init(queryDir);

    //Initialise the server list. This will update the server list
    //from the central the system server list
    ServerList::instance()->init();

    // startup - via the session manager, or straight to the main window?
    bool startMainWindow = true;

    //Initialise the session. We have to call this before VConfig::init() because
    //some settings VConfig loads are session-dependent.
    if (SessionHandler::requestStartupViaSessionManager())
    {
        SessionDialog sessionDialog;
        if (sessionDialog.exec() != QDialog::Accepted)
            startMainWindow = false;
    }
    else
    {
        SessionHandler::setTemporarySessionIfReqested(); // user starts with -ts command-line switch?
    }

    //Load the global configurations
    VConfig::instance()->init(DirectoryHandler::etcDir());
    
    //Import server settings from the previous viewer
    if(DirectoryHandler::isFirstStartUp())
    {
    	VConfig::instance()->importSettings();
    	VServerSettings::importRcFiles();
    }

    //Update objects with saved user settings (these are now stored in VConfig!!)
    VSettingsLoader::process();

    //Initialise highlighter
    Highlighter::init(DirectoryHandler::concatenate(DirectoryHandler::etcDir(),
    		      "ecflowview_highlighter.json"));

    //Initialise the system palette
    Palette::load(DirectoryHandler::concatenate(DirectoryHandler::etcDir(),
		      "ecflowview_palette.json")); 

    //Initialise the list containing all the icon names existed on last exit
    VIcon::initLastNames();

    //Start the GUI
	if (startMainWindow)
	{
        //Build the GUI
		MainWindow::init();

		//Show all the windows
		MainWindow::showWindows();

        //Start input event logging
        InputEventLog::instance()->start();

        //Enable (daily) truncation for ui log
        UiLog::enableTruncation();

        //Load extra diagnostic data
        DiagData::instance()->load();

		return app.exec();
	}
	else
	{
		return 0;  // user quit from within the session manager
	}
}
