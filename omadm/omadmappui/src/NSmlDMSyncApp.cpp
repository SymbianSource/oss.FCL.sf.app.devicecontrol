/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for CNSmlDMSyncApp
*
*/


// INCLUDE FILES
#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncDebug.h"
#include "NSmlDMSyncAppServer.h"
#include <eikstart.h>
#include <e32property.h>
#include "nsmldmsyncinternalpskeys.h"

// ================= OTHER EXPORTED FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CNSmlDMSyncApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CNSmlDMSyncApp::AppDllUid() const
    {
    FLOG( "[OMADM] AppDllUid:" );
    
    return KUidSmlSyncApp;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncApp::CreateDocumentL()
// Creates CNSmlDMSyncDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CNSmlDMSyncApp::CreateDocumentL()
    {
    FLOG( "[OMADM] CNSmlDMSyncApp::CreateDocumentL:" );
    
    CNSmlDMSyncDocument* document = CNSmlDMSyncDocument::NewL( *this );
    TInt Value = -1;
    static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
   	static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
    TInt r1=RProperty::Define(KUidSmlSyncApp,KNSmlDMSyncUiLaunchKey,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
    TInt r=RProperty::Get(KUidSmlSyncApp,KNSmlDMSyncUiLaunchKey,Value);
    if( r != KErrNone)
    {
    	User::LeaveIfError(r);
    	
    }
    
    if ( document )
        {
        //Only independent launch	
        if ( !document->IsServerAlertSync() )
            {
            document->StartServerL();
            }
        //Not an independent launch,server alert launch with SAN support    
        else if ( document->SANSupport() )     
            {
            document->StartServerL();	
            }	

        else if ( Value == 2)    //DM UI launch from CP
        {
        document->StartServerL();        
        }

        }
    return document;
    }

// -----------------------------------------------------------------------------
// NewApplication() 
// Constructs CNSmlDMSyncApp
// Returns: CApaDocument*: created application object
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    FLOG( "[OMADM] CApaApplication* NewApplication:" );
    
    return new CNSmlDMSyncApp;
    }

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    FLOG( "[OMADM] E32Main:" );
    
    return EikStart::RunApplication( NewApplication );
    }

// -----------------------------------------------------------------------------
// NewAppServerL() 
// 
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncApp::NewAppServerL( CApaAppServer*& aAppServer )
	{
	FLOG( "[OMADM] CNSmlDMSyncApp::NewAppServerL()" );
	
	aAppServer = new (ELeave) CNSmlDMSyncAppServer;
	}

// End of File
