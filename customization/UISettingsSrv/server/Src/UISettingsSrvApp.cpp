/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*        UISettingsSrv Application
*
*/


// INCLUDE FILES
#include    <eikstart.h>
#include    "UISettingsSrvApp.h"
#include    "UISettingsSrvDocument.h"

#include    "UISettingsSrv.h"
#include    "UISettingsSrvConstants.h"
#include	"debug.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CUISettingsSrvApp::CUISettingsSrvApp()
//
// ---------------------------------------------------------------------------
CUISettingsSrvApp::CUISettingsSrvApp( CUISettingsSrvServer* aServer )
	: iServer( aServer )
    {
    }

// ---------------------------------------------------------------------------
// CUISettingsSrvApp::~CUISettingsSrvApp()
//
// ---------------------------------------------------------------------------
CUISettingsSrvApp::~CUISettingsSrvApp()
    {
    delete iServer;
    }

// ---------------------------------------------------------------------------
// CUISettingsSrvApp::AppDllUid()
// Returns application UID
//
// ---------------------------------------------------------------------------
TUid CUISettingsSrvApp::AppDllUid() const
    {
    return KUidUISettingsSrv;
    }

// ---------------------------------------------------------------------------
// CUISettingsSrvApp::CreateDocumentL()
// Creates CGSDocument object
//
// ---------------------------------------------------------------------------
CApaDocument* CUISettingsSrvApp::CreateDocumentL()
    {
    return CUISettingsSrvDocument::NewL(*this);
    }

// ===================== OTHER EXPORTED FUNCTIONS ============================
//

// ---------------------------------------------------------------------------
// NewApplication() 
// constructs CUISettingsSrvApp
// Returns: CApaDocument*: created application object
//
// ---------------------------------------------------------------------------
LOCAL_C CApaApplication* NewApplication()
    {

	// create the server
	CUISettingsSrvServer* server = NULL;
	
	TInt err = KErrNone;
	TRAP( err, server = CUISettingsSrvServer::NewL() );

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

//sf-	// Ready to run
	RDEBUG("UISettingsSrv about to run");

	//
    // Continue with normal UI startup
    //
    return new CUISettingsSrvApp( server );
    }

// ---------------------------------------------------------------------------
// E32Main()
//
// ---------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

