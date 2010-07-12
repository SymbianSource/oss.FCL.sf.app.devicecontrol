/*
* ============================================================================
*  Name        : CWPSaver.cpp
*  Part of     : Provisioning / ProvisioningBC
*  Description : Helper class for saving Provisioning settings. Provides a progress note.
*  Version     : %version: 1 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2002-2006 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

// INCLUDE FILES
#include <aknquerydialog.h>
#include <eikprogi.h>
//#include <ProvisioningBC.rsg>
#include <commdb.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>
#include <ActiveFavouritesDbNotifier.h>
#include "CWPSaver.h"
#include <HbProgressDialog>
#include <HbAction>

// CONSTANTS
const TInt KMaxWaitTime = 2000000;
const TInt KRetryCount = 5;

// CLASS DECLARATION

// ========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// C++ default constructor.
// ----------------------------------------------------------------------------
CWPSaver::CWPSaver( CWPEngine& aEngine, TBool aSetAsDefault )
                  : CActive( EPriorityStandard ),
                    iEngine( aEngine ), 
                    iSetAsDefault( aSetAsDefault ),
                    iCurrentItem( 0 ),
                    iResult( KErrNone )
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::PrepareLC
// ----------------------------------------------------------------------------
//
void CWPSaver::PrepareLC()
    {
    // Assume ownership of this.
    CleanupStack::PushL( this );

    iApDbNotifier = CActiveApDb::NewL( EDatabaseTypeIAP );
    iApDbNotifier->AddObserverL( this );
    
    User::LeaveIfError( iSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iSession, KBrowserBookmarks ) );
    iFavouritesNotifier = 
                new(ELeave) CActiveFavouritesDbNotifier( iBookmarkDb, *this );  

    iFavouritesNotifier->Start();

    iRetryTimer = CPeriodic::NewL( EPriorityStandard );
    iProgress = new HbProgressDialog(HbProgressDialog::WaitDialog);
    iProgress->setText(hbTrId("txt_device_update_setlabel_saving_settings"));
    //iProgress->setTextAlignment(Qt::AlignCenter);
    iProgress->clearActions();   
    //iProgress->setPrimaryAction(new HbAction(""));
    //iProgress->setSecondaryAction(new HbAction(""));
    iProgress->show();

 }

// ----------------------------------------------------------------------------
// CWPSaver::ExecuteLD
// ----------------------------------------------------------------------------
//
TInt CWPSaver::ExecuteLD( TInt& aNumSaved, TBufC<256>& Value )
    {
    PrepareLC();

    // Add us to active scheduler and make sure RunL() gets called.
    CActiveScheduler::Add( this );
    CompleteRequest();
    iWait.Start();

    // Progress note has been finished/cancelled. Cache the result
    // and delete this.
    TInt result( iResult );
    aNumSaved = iCurrentItem;
    Value = iValue;
    CleanupStack::PopAndDestroy(); // this

    return result;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CWPSaver::~CWPSaver()
    {
    Cancel();

    delete iApDbNotifier;

    if( iFavouritesNotifier )
        {
        iFavouritesNotifier->Cancel();
        delete iFavouritesNotifier;
        }

    iBookmarkDb.Close();
    iSession.Close();
    delete iRetryTimer;
    }

// ----------------------------------------------------------------------------
// CWPSaver::DoCancel
// ----------------------------------------------------------------------------
//
void CWPSaver::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunL
// ----------------------------------------------------------------------------
//
void CWPSaver::RunL()
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
        TRAP(err, QT_TRYCATCH_LEAVING( iEngine.SaveL( iCurrentItem ) )); 
        }
    // If CommsDB or BookmarkDB are locked, schedule a retry
    if( err == EWPCommsDBLocked || err == KErrLocked)
        {
        iWaitCommsDb = ETrue;
        DelayedCompleteRequestL();
        return;
        }
    else if( err == EWPBookmarksLocked )
        {
        iWaitFavourites = ETrue;
        DelayedCompleteRequestL();
        return;
        }
    else if( err != KErrNone )
        {
        // For all other errors, pass them through.
        iValue = iEngine.SummaryText(iCurrentItem);
        delete iProgress;
        iProgress = NULL;
        User::LeaveIfError( err );
        }
    // Succesful save, so reset retry count
    iRetryCount = 0;

    // Normal progress
    if( iCurrentItem == iEngine.ItemCount()-1 )
        {
        //iProgress->cancel();
        //iProgress->close();
        delete iProgress;
        iProgress = NULL;
        iWait.AsyncStop();       
        }
    else
        {
        //CEikProgressInfo* progressInfo = iDialog->GetProgressInfoL();
        iCurrentItem++;
        //progressInfo->SetAndDraw(iCurrentItem);
        CompleteRequest();
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::RunError
// ----------------------------------------------------------------------------
//
TInt CWPSaver::RunError( TInt aError )
    {
    // There was a leave in RunL(). Store the error and
    // stop the dialog.
    iResult = aError;
    iWait.AsyncStop();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPSaver::DialogDismissedL
// ----------------------------------------------------------------------------
//
void CWPSaver::DialogDismissedL( TInt aButtonId )
    {
    if( aButtonId < 0 )
        {
        iResult = KErrCancel;
        }

    iWait.AsyncStop();
    }

// ----------------------------------------------------------------------------
// CWPSaver::CompleteRequest
// ----------------------------------------------------------------------------
//
void CWPSaver::CompleteRequest()
    {
    // Schedule an immediate complete. Make sure that there
    // is no timer alive first
    Cancel();
    iRetryTimer->Cancel();

    SetActive();
    TRequestStatus* sp = &iStatus;
    User::RequestComplete( sp, KErrNone );
    }

// ----------------------------------------------------------------------------
// CWPSaver::DelayedCompleteRequestL
// ----------------------------------------------------------------------------
//
void CWPSaver::DelayedCompleteRequestL()
    {
    if( iRetryCount < KRetryCount )
        {
        // Schedule a delayed complete. Cancel first in case
        // an immediate request was scheduled.
        iRetryTimer->Cancel();
        iRetryTimer->Start( KMaxWaitTime, KMaxTInt32, TCallBack( Timeout, this ) );
        iRetryCount++;
        }
    else
        {
        User::Leave( KErrTimedOut );
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::Retry
// ----------------------------------------------------------------------------
//
void CWPSaver::Retry()
    {
    // Immediate retry. Mark that we're not waiting
    // for an event and complete request.
    iWaitCommsDb = EFalse;
    iWaitFavourites = EFalse;
    CompleteRequest();
    }

// ----------------------------------------------------------------------------
// CWPSaver::Timeout
// ----------------------------------------------------------------------------
//
TInt CWPSaver::Timeout(TAny* aSelf)
    {
    // There was a time-out. Retry saving even though we
    // didn't get a notify from database.
    CWPSaver* self = static_cast<CWPSaver*>( aSelf );
    self->Retry();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CWPSaver::HandleApDbEventL
// ----------------------------------------------------------------------------
//
void CWPSaver::HandleApDbEventL( TEvent aEvent )
    {
    // We received an event from CommsDB. Retry if we're
    // waiting for it.
    if( iWaitCommsDb && aEvent == EDbAvailable )
        {
        Retry();
        }
    }

// ----------------------------------------------------------------------------
// CWPSaver::HandleFavouritesDbEventL
// ----------------------------------------------------------------------------
//
void CWPSaver::HandleFavouritesDbEventL( RDbNotifier::TEvent /*aEvent*/ )
    {
    // We received an event from BookmarkDB. Retry if we're
    // waiting for it.
    if( iWaitFavourites )
        {
        Retry();
        }
    }

//  End of File
