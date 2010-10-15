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
 
#ifndef DMTELSERVICEHANDLER_H
#define DMTELSERVICEHANDLER_H

#include <xqserviceprovider.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>

_LIT( KDMUIProcess, "\\deviceupdates.exe" );

enum  {
    EDMLaunchedFromDialer=0,
    EDMLaunchedFromCP 
    };

const TUid KControlPanelAppUid ={0x20025fd9};
const TUid KPSUidNSmlDMSyncApp = {0x101f6de5};
const TUint32 KDMLaunched = {0x0000000E};

class DmTelServiceHandler : public XQServiceProvider
{
    Q_OBJECT
public:
    DmTelServiceHandler(QObject *parent = NULL);
    ~DmTelServiceHandler();

public slots:
    void showVersionNumber();

};

#endif /* DMTELSERVICEHANDLER_H */
