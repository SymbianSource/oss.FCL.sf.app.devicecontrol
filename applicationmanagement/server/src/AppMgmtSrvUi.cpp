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


// INCLUDES 
#include <apgwgnam.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <textresolver.h>
#include "AppMgmtSrvUI.h"
#include "AppMgmtSrvApp.h"
#include "debug.h"

// ============================ MEMBER FUNCTIONS =============================
//

// ---------------------------------------------------------------------------
// CAppMgmtSrvUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvUi::ConstructL()
    {

    BaseConstructL(EAknEnableSkin | EAknEnableMSK);

    CAppMgmtSrvApp* app = (CAppMgmtSrvApp*) Application();
    CApplicationManagementServer* server = app->Server();
    server->StartShutDownTimerL();
    
    server->SendServerToBackground();

    this->StatusPane()->MakeVisible(EFalse);

    HideApplicationFromFSW(ETrue);

    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAppMgmtSrvUi::~CAppMgmtSrvUi()
    {

    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvUi::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvUi::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
    {

    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMgmtSrvUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {

    if (aType == EEventKey && aKeyEvent.iCode == EKeyEscape)
        {

        //Handle not required. Since UI Server goes to background and
        // brought to foreground at the time of displaying dialogs

        }

    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvUi::HandleCommandL(TInt aCommand)
    {

    switch (aCommand)
        {
        case EAknCmdExit:
        case EEikCmdExit:
            break;

        default:
            break;
        }

    }

// ---------------------------------------------------------------------------
// CAppMgmtAppUi::OpenFileL
// This is called by framework when application is already open in background
// and user open other file in eg. FileBrowse.
// New file to been shown is passed via aFileName.
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvUi::OpenFileL(const TDesC& aFileName)
    {

    }

// ---------------------------------------------------------------------------
// CAppMgmtAppUi::PrepareToExit
// This is called by framework when application is about to exit
// and server can prepare to stop any active downloads
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvUi::PrepareToExit()
    {

    }

void CAppMgmtSrvUi::HandleForegroundEventL(TBool aValue)
    {
    if (aValue == EFalse)
        RDEBUG( "CAppMgmtSrvUi::HandleForegroundEventL aValue::EFalse" );
    else
        RDEBUG( "CAppMgmtSrvUi::HandleForegroundEventL aValue::ETrue" );

    CAknAppUi::HandleForegroundEventL(aValue);
    // No Handle required    
    }
