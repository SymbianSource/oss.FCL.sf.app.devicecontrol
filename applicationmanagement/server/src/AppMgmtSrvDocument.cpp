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
 * Description:  Implementation of applicationmanagement components
 *
*/


// INCLUDE FILES
#include <StringLoader.h>
#include "AppMgmtSrvDocument.h"
#include "AppMgmtSrvUI.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Desctructor
// ---------------------------------------------------------------------------
//
CAppMgmtSrvDocument::~CAppMgmtSrvDocument()
    {
    }

void CAppMgmtSrvDocument::ConstructL()
    {

    }

CAppMgmtSrvDocument* CAppMgmtSrvDocument::NewL(CAknApplication& aApp)
    {

    CAppMgmtSrvDocument* self = new (ELeave) CAppMgmtSrvDocument( aApp );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

CEikAppUi* CAppMgmtSrvDocument::CreateAppUiL()
    {

    return new (ELeave) CAppMgmtSrvUi;
    }

// End of File
