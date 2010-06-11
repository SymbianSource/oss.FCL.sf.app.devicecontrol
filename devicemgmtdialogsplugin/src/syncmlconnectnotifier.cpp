/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/
#include <hbaction.h>
#include "devicemanagementnotifierutils.h"
#include "devicemanagementnotifierwidget_p.h"

syncmlConnectNotifier::syncmlConnectNotifier(devicemanagementnotifierwidget* ptr)
    {
    iPtr = ptr;
    qDebug("devicemanagementnotifierutils syncmlConnectNotifier");
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_en.qm", path);
    if (fine)
        qApp->installTranslator(translator);

    QTranslator *commontranslator = new QTranslator();

    fine = commontranslator->load("common_" + lang, path);
    if (fine)
        qApp->installTranslator(commontranslator);   
    pDialog = NULL;
    }

void syncmlConnectNotifier::launchDialog(const QVariantMap &parameters)
    {    
    qDebug("devicemanagementnotifierutils syncmlConnectNotifier launchDialog");        
    pDialog = new HbProgressDialog(HbProgressDialog::ProgressDialog);
       HbAction *action = new HbAction(pDialog->tr("Cancel"), pDialog);
       pDialog->setText("Connecting");
       pDialog->actions().at(0)->setText(pDialog->tr("Hide"));
       pDialog->setMaximum(0);
       pDialog->setMinimum(0);    
       pDialog->addAction(action);
       pDialog->open(this, SLOT(buttonClicked(HbAction*)));
    qDebug("devicemanagementnotifierutils syncmlConnectNotifier launchDialog end");
    }

void syncmlConnectNotifier::buttonClicked(HbAction* action)
{
    // Figure out, what user pressed 
    qDebug("syncmlConnectNotifier::buttonClicked");    
        if (pDialog->actions().at(0) == action ) {
               qDebug("Hide was selected");              
               iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EConnectingNote,0);
           } 
        else if ( pDialog->actions().at(1) == action  ) {

           qDebug("OMADM Cancel was selected");           
           qDebug("syncmlConnectNotifier::ultimateDialogSlot 1");
           iPtr->dmDevdialogDismissed(devicemanagementnotifierwidget::EConnectingNote,-3);                  
           }
    }


syncmlConnectNotifier::~syncmlConnectNotifier()
    {
    qDebug("syncmlConnectNotifier::~syncmlConnectNotifier");
   if(pDialog)
   	{
       delete pDialog;
       pDialog = NULL;
     }
    qDebug("syncmlConnectNotifier::~syncmlConnectNotifier end");
    }

