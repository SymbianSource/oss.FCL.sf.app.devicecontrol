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
#include <apgwgnam.h>
#include <apgcli.h>
#include "AppMgmtSrvApp.h"
#include "AppMgmtSrvDocument.h"
#include "ApplicationManagementCommon.h"

#include <apgtask.h>

// ---------------------------------------------------------------------------
// CAppMgmtSrvApp::CAppMgmtSrvApp
// ---------------------------------------------------------------------------
CAppMgmtSrvApp::CAppMgmtSrvApp()
    {
    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------------------------
//
TUid CAppMgmtSrvApp::AppDllUid() const
    {
    TUid AppMgmtuid;
    AppMgmtuid.iUid = KAppMgmtServerUid;
    return AppMgmtuid;
    }

// --------------------------------------------------------------------------
// CAppMgmtSrvApp::SetUIVisibleL
// Turns UI visible or hidden
// --------------------------------------------------------------------------
//
void CAppMgmtSrvApp::SetUIVisibleL()
    {
    /*CAppMgmtSrvUi*  ui = (CAppMgmtSrvUi*)iDocument->AppUi();
     
     TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
     TApaTask task = taskList.FindApp( TUid::Uid(KAppMgmtServerUid ));
     
     if(task.Exists())
     {
     if(!aVisible)
     {
     
     ui->HideApplicationFromFSW(ETrue);
     
     }
     else
     {
     
     task.BringToForeground();
     }*/

    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvApp::~CAppMgmtSrvApp()
// ---------------------------------------------------------------------------
//
CAppMgmtSrvApp::~CAppMgmtSrvApp()
    {
    }

// ---------------------------------------------------------------------------
// CAppMgmtSrvApp::CreateDocumentL()
// Creates AppMgmtSrvDocument object
// ---------------------------------------------------------------------------
//
CApaDocument* CAppMgmtSrvApp::CreateDocumentL()
    {

    iDocument = CAppMgmtSrvDocument::NewL( *this);
    return iDocument;
    }

#include <eikstart.h>

// ---------------------------------------------------------------------------
// NewApplication() 
// Constructs CAppMgmtSrvApp
// Returns: CApaDocument*: created application object
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {

    return new CAppMgmtSrvApp;
    }

// ---------------------------------------------------------------------------
// ServerL() 
// Getter for server
// ---------------------------------------------------------------------------
//
CApplicationManagementServer* CAppMgmtSrvApp::Server()
    {
    return iAppMgmtServer;
    }

// ---------------------------------------------------------------------------
// NewAppServerL() 
// Called by framwork
// ---------------------------------------------------------------------------
//
void CAppMgmtSrvApp::NewAppServerL(CApaAppServer*& aAppServer)
    {

    iAppMgmtServer = CApplicationManagementServer::NewL();
    iAppMgmtServer->iParentApp = this;
    aAppServer = iAppMgmtServer;
    }

// ---------------------------------------------------------------------------
// E32Main
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt err;
    RProcess pr;
    TFullName fn = pr.FullName();

    RDebug::Print(_L("[AppMgmtServer] E32Main:    >> called by %S"), &fn);

    err = EikStart::RunApplication(NewApplication);

    return err;
    }

// End of File
