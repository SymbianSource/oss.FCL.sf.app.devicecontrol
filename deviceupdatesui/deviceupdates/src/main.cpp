/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Application entry. 
*
*/

#include <hbinstance.h>
#include <dmfotaview.h>
#include <hbapplication.h>
#include <QDebug.h>
#include "nsmldmsyncprivatecrkeys.h"

int main(int argc, char *argv[])
{

    HbApplication app(argc, argv);
    //HbView* newview= new HbView();

    QTranslator *translator = new QTranslator();
   QString lang = QLocale::system().name();
   qDebug("OMADM Loading qm file");
   qDebug()<<lang;
   QString path = "z:/resource/qt/translations/";
    // bool fine = translator->load("deviceupdates_" + lang, path);/*finally required once localisation available*/
    bool fine = translator->load("deviceupdates_en.qm", path);
	if(fine)
   qApp->installTranslator(translator);
   else
   qDebug("omadm translator loading failed");
   
    QTranslator *commontranslator = new QTranslator();
   
   	fine = commontranslator->load("common_" + lang + ".qm", path);/*finally required once localisation available*/
    //fine = commontranslator->load("common_en" , path);
   	if(fine)
   qApp->installTranslator(translator);
   else
   qDebug("omadm common translator loading failed");
   
    app.setApplicationName(hbTrId("txt_device_update_subhead_device_updates"));
    
    CRepository *cenrep = CRepository::NewL(KUidSmlSyncApp);
    TInt val(0);
    if(cenrep)
        {        
        cenrep->Get(KNsmlDmUILaunch, val);
            {
            if(val==1)
                {
                QTranslator *tempTranslator = new QTranslator();
    			QString lang = QLocale::system().name();
    			tempTranslator->load("control_panel_" + lang, path);
    			qApp->installTranslator(tempTranslator);
    			app.setApplicationName(hbTrId("txt_applib_list_control_panel"));
    			val=0;
    			cenrep->Set(KNsmlDmUILaunch, val);
    			delete cenrep;
    			cenrep = NULL;
                }
            }
        }
    HbMainWindow window;

    DMFotaView* view = new DMFotaView(&window);
    bool loadingok = view->addFotaView();

    window.show();
    return app.exec();
}

