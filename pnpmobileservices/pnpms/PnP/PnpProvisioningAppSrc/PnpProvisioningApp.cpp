/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


// INCLUDE FILES
#include    <eikstart.h>

#include    "PnpProvisioningApp.h"
#include    "PnpProvisioningDocument.h"
#include    "PnpLogger.h"


LOCAL_C CApaApplication* NewApplication()
    {
    LOGSTRING( "NewApplication" );
    return new CPnpProvisioningApp;
    }

GLDEF_C TInt E32Main()
    {
    LOGSTRING( "E32Main" );
    return EikStart::RunApplication(NewApplication);
    }
    

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPnpProvisioningApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CPnpProvisioningApp::AppDllUid() const
    {
    LOGSTRING( "AppDllUid" );
    return KUidPnpProvisioning;
    }

// ---------------------------------------------------------
// CPnpProvisioningApp::CreateDocumentL()
// Creates CPnpProvisioningDocument object
// ---------------------------------------------------------
//
CApaDocument* CPnpProvisioningApp::CreateDocumentL()
    {
    LOGSTRING( "CreateDocumentL" );
    return CPnpProvisioningDocument::NewL( *this );
    }

// End of File  

