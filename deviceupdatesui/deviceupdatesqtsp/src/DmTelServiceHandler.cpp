/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Service Handler for dialer launch of DM
 *
 */

#include <hbapplication.h>
#include <coemain.h>
#include <apgtask.h>
#include <e32property.h>
#include <qdebug.h>
#include "DmTelServiceHandler.h"

DmTelServiceHandler::DmTelServiceHandler(QObject *parent):
    XQServiceProvider("com.nokia.services.devicemanager", parent)
{
    publishAll();
}

DmTelServiceHandler::~DmTelServiceHandler()
{
}

void DmTelServiceHandler::showVersionNumber()
    {
    qDebug("DmTelServiceHandler::showVersionNumber >> ");
    // Create DM UI Process
    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    CleanupClosePushL(ws);
    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp(KPSUidNSmlDMSyncApp);
    TInt dmLaunch=NULL;
    TInt err = NULL;
    RProperty::Get(KPSUidNSmlDMSyncApp, KDMLaunched, dmLaunch);
    if (task.Exists())
        {
        if (dmLaunch == EDMLaunchedFromDialer)
            {
            task.BringToForeground();
            }
        else if(dmLaunch == EDMLaunchedFromCP)
            {
            TApaTask task1 = taskList.FindApp(KControlPanelAppUid);
            if (task1.Exists())
                {
                task1.BringToForeground();
                }
            }
        }
    else
        {
        RProcess rp;
        err = rp.Create(KDMUIProcess, KNullDesC);
        qDebug()<<"process creation error "<<err;
        rp.SetPriority(EPriorityForeground);
        rp.Resume(); // logon OK - start the server
        // quit the service provider app.
        rp.Close();
        }
    int requestId = setCurrentRequestAsync();
    CleanupStack::PopAndDestroy(); //for ws

    completeRequest(requestId,0);
    connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    //QApplication::exit(0);
	qDebug("DmTelServiceHandler::showVersionNumber >> end");
}
