/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Service Activation this feature provided if service 
*                to be enabled from PNPMS application
*
*/


#include <PnpProvUtil.h>
#include "PnpServiceActivation.h"
#include <apacmdln.h> 
#include <apgcli.h>
#include <w32std.h>
#include <apgtask.h>
#include <w32std.h>
#include <StringLoader.h>
#include <PnpProvisioning.rsg>
#include <AknGlobalNote.h>


const TUint32 KEmailApplicationUID[] = {0x100058C5};

// Browser UID
const TUid KWAPBrowserUID = { 0x10008d39 };

// POP3 and IMAP4 provisioning settings application ID
_LIT(KEmailProvAppPOP3ID, "110");
_LIT(KEmailProvAppIMAP4ID, "143"); 

// Literal constants
_LIT( KMessagingAppName, "z:\\sys\\bin\\mce.exe" );
_LIT( KEmptyDoc, "");
	

// INCLUDE FILES

// -----------------------------------------------------------------------------
// CPnpServiceActivation::CPnpServiceActivation
// C++ default constructor can NOT contain any code, that
//  -----------------------------------------------------------------------------
//
CPnpServiceActivation::CPnpServiceActivation()
    {
    }

// -----------------------------------------------------------------------------
// CPnpProvUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::ConstructL()
    {
     //No values to assign	
    }  
 
// -----------------------------------------------------------------------------
// CPnpProvUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPnpServiceActivation* CPnpServiceActivation::NewL()
    {
    CPnpServiceActivation* self = NewLC();
    CleanupStack::Pop();
    
    return self; 
    }

// -----------------------------------------------------------------------------
// CPnpServiceActivation::NewLC
// -----------------------------------------------------------------------------
//
CPnpServiceActivation* CPnpServiceActivation::NewLC()
    {
    CPnpServiceActivation* self = new( ELeave ) CPnpServiceActivation;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self; 
    } 

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CPnpServiceActivation::~CPnpServiceActivation()
    {   
    }
// -----------------------------------------------------------------------------
// CPnpServiceActivation::LaunchApplicationL
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::LaunchApplicationL()
    {
    
    // For email service activation Messaging launcher is activated
    TBool validApp = ValidateAppUidWithProvappIDL();
    
    if(validApp) 
    {
    	
    
     CApaCommandLine* cmd = CApaCommandLine::NewLC();
     
     cmd->SetExecutableNameL( KMessagingAppName );
     
     cmd->SetDocumentNameL( KEmptyDoc );
     cmd->SetCommandL( EApaCommandOpen ); 
     RApaLsSession appArcSession;
	
     User::LeaveIfError( appArcSession.Connect() );
     appArcSession.StartApp( *cmd ); 
     appArcSession.Close();
     CleanupStack::PopAndDestroy( cmd ); 
    }

    }


// -----------------------------------------------------------------------------
// CPnpServiceActivation::ValidateAppUidWithProvappIDL()
// -----------------------------------------------------------------------------
//
TBool CPnpServiceActivation::ValidateAppUidWithProvappIDL()
    {
    
    // Get application ID and Provisioning settings application
    // ID and validate the application launcher	
    TBool flag (EFalse);
    RPointerArray<HBufC> appID;
	
    CPnpProvUtil *provutil = CPnpProvUtil::NewLC();
    
    TUint32 val = provutil->GetApplicationUidL();
    if(val == KEmailApplicationUID[0])
    {
	provutil->GetProvAdapterAppIdsL(appID);
    
    	for(TInt i=0; i<appID.Count(); i++)
    	{
		TPtrC ptr(*appID[i]);
		if(ptr.Compare(KEmailProvAppPOP3ID) ==0||
		   ptr.Compare(KEmailProvAppIMAP4ID) ==0 )
		flag = ETrue;
	
    	}
     }
    
    
    CleanupStack::PopAndDestroy();
     
    appID.ResetAndDestroy();
    appID.Close();
    return flag; 
    }


// -----------------------------------------------------------------------------
// CPnpServiceActivation::KillBrowserL()
// -----------------------------------------------------------------------------
//
void CPnpServiceActivation::KillBrowserL()
    {
     
    // Get the correct application data 
    RWsSession ws;
    User::LeaveIfError(ws.Connect()); 
    CleanupClosePushL(ws);

    // Find the task with uid of browser. If task exists then 
    // kill browser
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KWAPBrowserUID );
    if(task.Exists())
    task.EndTask();

    CleanupStack::PopAndDestroy(); // ws, lsSession

    
    }
    
// -----------------------------------------------------------------------------
// CPnpServiceActivation::DisplayAppSpecNote()
// -----------------------------------------------------------------------------
//

void CPnpServiceActivation::DisplayAppSpecNoteL()
{
    HBufC* text = StringLoader::LoadLC(R_TEXT_EMAIL_SERVICE_NOTE);

    CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
    CleanupStack::PushL( globalNote );
    globalNote->ShowNoteL( EAknGlobalInformationNote , *text );
    CleanupStack::PopAndDestroy( 2 );
}

//  End of File

