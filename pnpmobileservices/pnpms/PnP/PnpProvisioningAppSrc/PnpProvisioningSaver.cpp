/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper class for saving Provisioning settings. 
*                Provides a progress note.
*
*/



// INCLUDE FILES
#include <AknQueryDialog.h>
#include <eikprogi.h>
#include <PnpProvisioning.rsg>
#include <commdb.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>
#include <activefavouritesdbnotifier.h>

#include "PnpProvisioningSaver.h"
#include "PnpLogger.h"
#include "cwaitdialogmonitor.h"

// CONSTANTS
const TInt KMaxWaitTime = 1000000;

// CLASS DECLARATION

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor.
CPnpProvisioningSaver::CPnpProvisioningSaver( CWPEngine& aEngine, TBool aSetAsDefault, TBool aWaitNote )
: CActive( EPriorityStandard ), iEngine( aEngine ), 
  iSetAsDefault( aSetAsDefault ),
  iCurrentItem( 0 ), iResult( KErrNone ),iEndKeyPressed(EFalse), iShowWaitNote(aWaitNote)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::ExecuteLD
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::PrepareLC()
    {
    // Assume ownership of this.
    CleanupStack::PushL( this );

    iApDbNotifier = CActiveApDb::NewL( EDatabaseTypeIAP );
    iApDbNotifier->AddObserverL( this );

    iRetryTimer = CPeriodic::NewL( EPriorityStandard );

    LOGSTRING("Constructing dialog");
    
    // Set up the dialog and callback mechanism.
    if(iSetAsDefault && iShowWaitNote)
    ShowWaitNoteL();
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::ExecuteLD
// ---------------------------------------------------------
//
TInt CPnpProvisioningSaver::ExecuteLD( TInt& aNumSaved )
    {
    LOGSTRING( "ExecuteLD" );
    PrepareLC();

    // make sure RunL() gets called.
    CompleteRequest();
    iWait.Start();

    LOGSTRING("iWait.Start done");
    // Progress note has been finished/cancelled. Cache the result
    // and delete this.
    TInt result( iResult );
    aNumSaved = iCurrentItem;
    CleanupStack::PopAndDestroy( this ); // this
    LOGSTRING( "PopAndDestroy( this ), done" );

    return result;
    }

// Destructor
// ---------------------------------------------------------
// CPnpProvisioningSaver::~CPnpProvisioningSaver
// ---------------------------------------------------------
//
CPnpProvisioningSaver::~CPnpProvisioningSaver()
    {
    LOGSTRING( "~CPnpProvisioningSaver" );

    if( IsActive() )
        {
        Cancel();
        }

    if( iApDbNotifier )
        {
        iApDbNotifier->RemoveObserver( this );
        delete iApDbNotifier;
        }

    if( iRetryTimer )
        {
        if( iRetryTimer->IsActive() )
            {
            iRetryTimer->Cancel();
            }
        delete iRetryTimer;
        }
        
    
    if( iGlobalWaitNote )
        {
        delete iGlobalWaitNote;
        }
        
    if( iWaitDialogMonitor )
        {
        iWaitDialogMonitor->Cancel();
        delete iWaitDialogMonitor;
        }
        
    LOGSTRING( "~CPnpProvisioningSaver - done" );
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::DoCancel
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::DoCancel()
    {
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::RunL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::RunL()
    {
    // Choose whether to save or set as default
    TInt err( KErrNone );
    if( iSetAsDefault )
        {
        TRAP( err, 
            if( iEngine.CanSetAsDefault( iCurrentItem ) )
                {
                iEngine.SetAsDefaultL( iCurrentItem );
                } );
        }
    else
        {

        LOGSTRING2("Saving item: %i", iCurrentItem );
        TRAP( err, iEngine.SaveL( iCurrentItem ) );
        
        }

    // If CommsDB or BookmarkDB are locked, schedule a retry
    if( err == EWPCommsDBLocked )
        {
        LOGSTRING( "CPnpProvisioningSaver: EWPCommsDBLocked" );
        iWaitCommsDb = ETrue;
        DelayedCompleteRequest();
        return;
        }
    else if( err == EWPBookmarksLocked )
        {
        LOGSTRING( "CPnpProvisioningSaver: EWPBookmarksLocked" );
        iWaitFavourites = ETrue;
        DelayedCompleteRequest();
        return;
        }
    else if( err == KErrLocked )
        {
        LOGSTRING( "CPnpProvisioningSaver: KErrLocked" );
        // Assume that commsdb caused the error
        iWaitCommsDb = ETrue;
        DelayedCompleteRequest();
        return;
        }
    // For all other errors, pass them through.
    User::LeaveIfError( err );

    // Normal progress
    if( iCurrentItem == iEngine.ItemCount()-1 )
        {
        LOGSTRING("All saved");
        //LOGSTRING2( "Saver RunL iWaitDialogMonitor->iStatus %i", iWaitDialogMonitor->iStatus.Int() );        
        ProcessFinishedL();
        }
    else
        {
        //LOGSTRING2( "Saver RunL 2nd iWaitDialogMonitor->iStatus %i", iWaitDialogMonitor->iStatus.Int() );
        iCurrentItem++;
        CompleteRequest();
        }
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::ProcessFinishedL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::ProcessFinishedL()
    {
    //LOGSTRING2( "Saver iWaitDialogMonitor->iStatus %i", iWaitDialogMonitor->iStatus.Int() );
    iWait.AsyncStop();
    if(iWaitDialogMonitor)
    {
    //End key or cancel pressed after saving settings
    if(iWaitDialogMonitor->iStatus == KErrCancel)
    	{
    	iEndKeyPressed = ETrue;
    	}
    if( iGlobalWaitNote )
        {
        delete iGlobalWaitNote;
        iGlobalWaitNote = NULL;
        }
     }
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::RunError
// ---------------------------------------------------------
//
TInt CPnpProvisioningSaver::RunError( TInt aError )
    {
    LOGSTRING2( "RunError: %i", aError );
    // There was a leave in RunL(). Store the error and
    // stop the dialog.
    iResult = aError;
    TInt err(KErrNone);
    TRAP( err, ProcessFinishedL() );

    return KErrNone;
    }
    
// ---------------------------------------------------------
// CPnpProvisioningSaver::ShowWaitNoteL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::ShowWaitNoteL()
    {
    HBufC* msgText = CEikonEnv::Static()->
        AllocReadResourceLC( R_TEXT_WAIT_SAVING ); 

    if (iWaitDialogMonitor)
        {
        iWaitDialogMonitor->Cancel();
        delete iWaitDialogMonitor;
        iWaitDialogMonitor = NULL;
        }
    // instantiate the active object CGlobalConfirmationObserver
    iWaitDialogMonitor = CWaitDialogMonitor::NewL( *this );
    
    // SetActive
    iWaitDialogMonitor->Start();
    
    if (!iGlobalWaitNote)
        {
        iGlobalWaitNote = CAknGlobalNote::NewL();
        }
        
    iGlobalWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY);

    iGlobalWaitNote->ShowNoteL(
        iWaitDialogMonitor->iStatus,
        EAknGlobalWaitNote,
        *msgText );
    
    CleanupStack::PopAndDestroy( msgText );
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::WaitDialogDismissedL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::WaitDialogDismissedL( const TInt aStatusCode )
    {
    LOGSTRING( "DialogDismissedL" );
    if( aStatusCode == EAknSoftkeyCancel || aStatusCode == EAknSoftkeyExit )
        {
        LOGSTRING( "setting to KErrCancel" );  
        iResult = KErrCancel;
        }
    //pressed end key and not all settings saved
    if( aStatusCode== KErrCancel && iEndKeyPressed )
    	{
    	LOGSTRING( "End key pressed" );    	
        iResult = KErrCancel;
        }

    iWait.AsyncStop();
    }
    

// ---------------------------------------------------------
// CPnpProvisioningSaver::CompleteRequest
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::CompleteRequest()
    {
    // Schedule an immediate complete. Make sure that there
    // is no timer alive first
    if( IsActive() )
        {
        Cancel();
        }
    if( iRetryTimer->IsActive() )
        {
        iRetryTimer->Cancel();
        }
    SetActive();
    TRequestStatus* sp = &iStatus;
    User::RequestComplete( sp, KErrNone );
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::DelayedCompleteRequest
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::DelayedCompleteRequest()
    {
    // Schedule a delayed complete. Cancel first in case
    // an immediate request was scheduled.
    if( IsActive() )
        {
        Cancel();
        }
    if( iRetryTimer->IsActive() )
        {
        iRetryTimer->Cancel();
        }

    iRetryTimer->Start( KMaxWaitTime, 0, TCallBack( Timeout, this ) );
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::Retry
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::Retry()
    {
    // Immediate retry. Mark that we're not waiting
    // for an event and complete request.
    iWaitCommsDb = EFalse;
    iWaitFavourites = EFalse;
    CompleteRequest();
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::Timeout
// ---------------------------------------------------------
//
TInt CPnpProvisioningSaver::Timeout(TAny* aSelf)
    {
    // There was a time-out. Retry saving even though we
    // didn't get a notify from database.
    CPnpProvisioningSaver* self = STATIC_CAST(CPnpProvisioningSaver*, aSelf);
    self->Retry();

    return KErrNone;
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::HandleApDbEventL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::HandleApDbEventL( TEvent aEvent )
    {
    // We received an event from CommsDB. Retry if we're
    // waiting for it.
    if( iWaitCommsDb && aEvent == EDbAvailable )
        {
        Retry();
        }
    }

// ---------------------------------------------------------
// CPnpProvisioningSaver::HandleFavouritesDbEventL
// ---------------------------------------------------------
//
void CPnpProvisioningSaver::HandleFavouritesDbEventL( RDbNotifier::TEvent /*aEvent*/ )
    {
    // We received an event from BookmarkDB. Retry if we're
    // waiting for it.
    if( iWaitFavourites )
        {
        Retry();
        }
    }

//  End of File

