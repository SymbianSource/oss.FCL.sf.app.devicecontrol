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
* Description:  Qt Highway Service Provider for control panel
*
*/
#include <qtranslator.h>
#include "dmserviceprovider.h"
#include "nsmldmsyncinternalpskeys.h"
#include <hbapplication.h>
#include <qlocale.h>
#include <nsmldmdef.h>
#include <e32property.h>
#include <qdebug.h>
// -----------------------------------------------------------------------------
// DMServiceProvider::DMServiceProvider
// -----------------------------------------------------------------------------
//
DMServiceProvider::DMServiceProvider(QObject* parent)
: XQServiceProvider("com.nokia.services.MDM.devicemanager",parent)
{
    publishAll();
}

// -----------------------------------------------------------------------------
// DMServiceProvider::~DMServiceProvider
// -----------------------------------------------------------------------------
//
DMServiceProvider::~DMServiceProvider()
{
}

// -----------------------------------------------------------------------------
// DMServiceProvider::complete
// -----------------------------------------------------------------------------
//
void DMServiceProvider::complete()
{
    TInt err = RProperty::Delete(KPSUidNSmlDMSyncApp, KDMLaunched);
    qDebug()<<"dmsrvice error "<<err;
    completeRequest(requestId,0);
}

// -----------------------------------------------------------------------------
// DMServiceProvider::launchDM
// -----------------------------------------------------------------------------
//
void DMServiceProvider::launchDM()
{
    TInt err = RProperty::Define(KPSUidNSmlDMSyncApp, KDMLaunched, RProperty::EInt);
    TInt launched=EDMLaunchedFromCP;
    RProperty::Set(KPSUidNSmlDMSyncApp, KDMLaunched, launched);
    requestId = setCurrentRequestAsync();
}
