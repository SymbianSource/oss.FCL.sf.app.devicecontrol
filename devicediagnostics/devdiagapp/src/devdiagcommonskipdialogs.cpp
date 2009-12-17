/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the implementation of the common dialogs, which are
*                used by both the Device Diagnostics Application and plugins.
*
*/


// System Include Files
#include <DiagFrameworkDebug.h>     // Debugging Macros
#include <devdiagapp.rsg>        // Resource defintions

// User Include Files
#include "devdiagcommonskipdialogs.h"        // CDevDiagCommonSkipDialogs
#include "devdiagengine.h"            // CDevDiagEngine


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CDevDiagCommonSkipDialogs* CDevDiagCommonSkipDialogs::NewLC(
    CDevDiagEngine& aEngine,
    TBool aSuspendTestsOnExecute )
    {
    LOGSTRING2( "CDevDiagCommonSkipDialogs::NewLC( %d )",
                aSuspendTestsOnExecute )

    CDevDiagCommonSkipDialogs* self =
        new ( ELeave ) CDevDiagCommonSkipDialogs( aEngine,
                                                 aSuspendTestsOnExecute );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// From CAknQueryDialog.
// Runs the dialog, and returns the ID of the button used to dismiss it.
// ---------------------------------------------------------------------------
//
TInt CDevDiagCommonSkipDialogs::RunLD()
    {
    LOGSTRING( "CDevDiagCommonSkipDialogs::RunLD()" )

    // Tell the engine to suspend execution while we're asking the user if
    // they really want to cancel tests.
    if ( iSuspendTestsOnExecute )
        {
        iEngine.ExecutionStopL( CDevDiagEngine::EStopModeSuspend );
        }

    // Only stop the watchdog timer.
    else
        {
        iEngine.ExecutionStopL( CDevDiagEngine::EStopModeWatchdog );
        }

    // Ask the user if they really want to cancel all.  Note: We cannot cancel
    // or resume after this call, because it deletes the dialog, so that
    // handling is done in OkToExitL.
    return CAknListQueryDialog::RunLD();
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDevDiagCommonSkipDialogs::CDevDiagCommonSkipDialogs(
    CDevDiagEngine& aEngine,
    TBool aSuspendTestsOnExecute )
:   CAknListQueryDialog( &iIndex ),
    iEngine( aEngine ),
    iSuspendTestsOnExecute( aSuspendTestsOnExecute )
    {
    LOGSTRING( "CDevDiagCommonSkipDialogs::CDevDiagCommonSkipDialogs()" )

    // Nothing to do.
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CDevDiagCommonSkipDialogs::ConstructL()
    {
    LOGSTRING( "CDevDiagCommonSkipDialogs::ConstructL()" )

    PrepareLC( R_DEVDIAG_SKIP_LIST_QUERY );
    CleanupStack::Pop( this );
    }

// ---------------------------------------------------------------------------
// From CAknQueryDialog.
// This function is called for a button press on the dialog.  It is used to
// determine if the dialog may be exited.
// ---------------------------------------------------------------------------
//
TBool CDevDiagCommonSkipDialogs::OkToExitL( TInt aButtonId )
    {
    LOGSTRING2( "CDevDiagCommonSkipDialogs::OkToExitL( %d )", aButtonId )

    //This must be called to update iIndex to right value.
    TBool exit = CAknListQueryDialog::OkToExitL ( aButtonId );
  
    if ( aButtonId == EAknSoftkeyOk )
        {
        if ( iIndex == 0 ) //cancel this test
            {                        
            iEngine.ExecutionStopL( CDevDiagEngine::EStopModeSkip );
            }
        else if ( iIndex == 1 ) //cancel rest of the test
            {
            // Cancel all execution, as the user requested.
            iEngine.ExecutionStopL( CDevDiagEngine::EStopModeCancel );
            }
        else
            {
            //Error
            }
        
        }
    else
        {
        // Resume execution because the user decided not to cancel.
        if ( iSuspendTestsOnExecute )
            {
            iEngine.ExecutionResumeL( CDevDiagEngine::EResumeModeResume );
            }
        // Restart the watchdog timer.
        else
            {
            iEngine.ExecutionResumeL( CDevDiagEngine::EResumeModeWatchdog );
            }
        }

    return ETrue;
    }

// End of File
