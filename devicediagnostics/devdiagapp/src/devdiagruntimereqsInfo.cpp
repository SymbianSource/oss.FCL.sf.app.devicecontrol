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
* Description:  This is the implementation of the runtime requirements
*                information data class.
*
*/


// System Include Files
#include <aknmessagequerydialog.h>      // CAknMessageQueryDialog
#include <centralrepository.h>          // CRepository
#include <DiagFrameworkDebug.h>         // Debugging Macros
#include <DiagResultsDatabase.h>        // RDiagResultsDatabase
#include <DiagResultsDatabaseTestRecordInfo.h>
#include <f32file.h>                    // RFs
#include <StringLoader.h>               // StringLoader
#include <devdiagapp.rsg>            // Resource defintions

// User Include Files
#include "devdiag.pan"            // Panic
#include "devdiagruntimereqsinfo.h"          // CDevDiagRuntimeReqsInfo
#include "devdiagprivatecrkeys.h"   // CR Key ID Definitions



// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagRuntimeReqsInfo* CDevDiagRuntimeReqsInfo::NewL( RFs& aFs,
                                              RDiagResultsDatabase& aDb,
                                              TCallBack aCallback )
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::NewL()" )

    CDevDiagRuntimeReqsInfo* self = new ( ELeave ) CDevDiagRuntimeReqsInfo( aFs,
                                                                  aDb,
                                                                  aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDevDiagRuntimeReqsInfo::~CDevDiagRuntimeReqsInfo()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::~CDevDiagRuntimeReqsInfo()" )

    Cancel();
    delete iTelephony;
    
    }

// ---------------------------------------------------------------------------
// Begins checking if the diagnostics runtime requirements are satisfied.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::CheckRuntimeRequirementsL( TBool aResumingTests )
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::CheckRuntimeRequirementsL()" )

    // If the requirements are already being checked, start over.
    if ( iState != EStateInitial )
        {
        Cancel();
        }

    iResumingTests = aResumingTests;

    // (Check 1) Check the free disk space.
    CheckDiskSpaceL();

    // (Check 2) Check the battery level.
    CheckBatteryLevelL();
    }

// ---------------------------------------------------------------------------
// Returns the runtime requirements result, which indicates if the runtime
// requirements were satisfied.
// ---------------------------------------------------------------------------
//
CDevDiagRuntimeReqsInfo::TResult CDevDiagRuntimeReqsInfo::Result() const
    {
    return iResult;
    }

// ---------------------------------------------------------------------------
// Displays the dialog associated with the result of the runtime checks.
// ---------------------------------------------------------------------------
//
TInt CDevDiagRuntimeReqsInfo::DisplayDialogL() const
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::DisplayDialogL()" )

    // Act on the result.
    switch ( iResult )
        {      
        case EResultErrLowDisk:
            return DisplayMessageQueryDialogL( R_DEVDIAG_LOW_DISK_DIALOG );
 
        case EResultErrLowBattery:
            return DisplayMessageQueryDialogL( R_DEVDIAG_LOW_BATTERY_DIALOG );      

        case EResultOk:
        default:
            return EAknSoftkeyOk;
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CDevDiagRuntimeReqsInfo::CDevDiagRuntimeReqsInfo( RFs& aFs,
                                        RDiagResultsDatabase& aDb,
                                        TCallBack aCallback )
:   CActive( EPriorityStandard ),
    iFs( aFs ),
    iDb( aDb ),
    iCallback( aCallback ), 
    iBatteryInfo(),
    iBatteryInfoPackage( iBatteryInfo )
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::CDevDiagRuntimeReqsInfo()" )

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::ConstructL()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::ConstructL()" )

    ReadCentralRepositoryValuesL();
    }

// ---------------------------------------------------------------------------
// Reads the runtime requirements values from the central repository and
// stores them to the object.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::ReadCentralRepositoryValuesL()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::ReadCentralRepositoryValuesL()" )

    CRepository* repository = CRepository::NewLC( KCRUidDevDiag );

    // The required battery level.
    User::LeaveIfError( repository->Get( KDevDiagAppMinBatteryLevel,
                                         iMinBatteryLevel ) );

    // The required free disk space.  A local TInt is used because the data
    // member is a TInt64, but the CR only has a getter function for TInt.
    TInt minDiskSpaceRequired;
    User::LeaveIfError( repository->Get( KDevDiagAppMinDiskSpace,
                                         minDiskSpaceRequired ) );
    iMinDiskSpaceFree = minDiskSpaceRequired;

 
    LOGSTRING3( "CDevDiagRuntimeReqsInfo::ReadCentralRepositoryValuesL() -"
                L" iMinBatteryLevel %d, iMinDiskSpaceFree %d",
                iMinBatteryLevel,
                minDiskSpaceRequired )

    CleanupStack::PopAndDestroy( repository );
    }


// ---------------------------------------------------------------------------
// Checks if there is enough free estimated disk space to log test results.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::CheckDiskSpaceL()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::CheckDiskSpaceL()" )

    // If the runtime requirements have already failed, this function will
    // set us active and complete the request.
    if ( CheckCompletion() )
        {
        return;
        }

    // Set default values for this check.
    iDiskSpaceFree = 0;

    // Get the free disk space.
    TVolumeInfo volumeData;
    User::LeaveIfError( iFs.Volume( volumeData ) );
    iDiskSpaceFree = volumeData.iFree;

    // Check the free disk level.
    if ( iDiskSpaceFree < iMinDiskSpaceFree )
        {
        iResult = EResultErrLowDisk;
        }

    return;
    }

// ---------------------------------------------------------------------------
// Checks if the phone's battery level is adequate to run tests.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::CheckBatteryLevelL()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::CheckBatteryLevelL()" )

    // If the runtime requirements have already failed, this function will
    // set us active and complete the request.
    if ( CheckCompletion() )
        {
        return;
        }

    // Check the battery level.
    switch ( iState )
        {
        case EStateInitial:
            {
            iState = EStateCheckingBattery;

            // Issue the asynchronous request.
            if ( !iTelephony )
                {
                iTelephony = CTelephony::NewL();
                }

            iTelephony->GetBatteryInfo( iStatus, iBatteryInfoPackage );
            SetActive();
            break;
            }

        case EStateCheckingBattery:
            {
            iState = EStateInitial;

            // Check the battery level.
            if ( iStatus != KErrNone )
                {
                iResult = EResultErrLowBattery;
                break;
                }

            if ( iBatteryInfo.iChargeLevel < TUint( iMinBatteryLevel ) )
                {
                iResult = EResultErrLowBattery;
                }

            iBatteryLevel = iBatteryInfo.iChargeLevel;
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EDevDiagApplicationRuntimeReqs) );
            iResult = EResultErrLowBattery;
            CheckCompletion();
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// This is a utility function to handle the common checking for completion
// of the runtime requirements checks.
// ---------------------------------------------------------------------------
//
TBool CDevDiagRuntimeReqsInfo::CheckCompletion()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::CheckCompletion()" )

    // If there is already a failed check, complete the asynchronous request
    // if it hasn't already been done.
    if ( iResult != EResultOk && iState != EStateComplete )
        {
        LOGSTRING( "CDevDiagRuntimeReqsInfo::CheckCompletion() - completed" )
        iState = EStateComplete;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        return ETrue;
        }

    if ( iState == EStateComplete )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// Utility function to display a message dialog associated with the specified
// resource.
// ---------------------------------------------------------------------------
//
TInt CDevDiagRuntimeReqsInfo::DisplayMessageQueryDialogL( TInt aResource )
    {
    LOGSTRING2( "CDevDiagRuntimeReqsInfo::DisplayMessageQueryDialogL( %d )",
                aResource )
    CAknMessageQueryDialog* dlg = new ( ELeave ) CAknMessageQueryDialog;
    return dlg->ExecuteLD( aResource );
    }


// ---------------------------------------------------------------------------
// From CActive.
// This function is called when an active request completes.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::RunL()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::RunL()" )

    switch ( iState )
        {
        case EStateCheckingBattery:
            // (Check 1) Check the battery level.
            CheckBatteryLevelL();
                   
      
        case EStateComplete:
        default:
            {          
            delete iTelephony;
            iTelephony = NULL;
            // Inform the observer that checking is complete.
            iCallback.CallBack();
            }
        }
    }

// ---------------------------------------------------------------------------
// From CActive.
// This function is called to cancel any outstanding asynchronous requests.
// ---------------------------------------------------------------------------
//
void CDevDiagRuntimeReqsInfo::DoCancel()
    {
    LOGSTRING( "CDevDiagRuntimeReqsInfo::DoCancel()" )
   
     // Cancel the asynchronous requests.
    switch ( iState )
        {
        case EStateCheckingBattery:
            __ASSERT_DEBUG( iTelephony, Panic( EDevDiagApplicationRuntimeReqs ) );
            iTelephony->CancelAsync( CTelephony::EGetBatteryInfoCancel );
            break;
     
        default:
            break;
        }
    }

// End of File
