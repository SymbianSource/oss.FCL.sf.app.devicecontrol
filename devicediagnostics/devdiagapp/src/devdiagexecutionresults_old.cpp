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
* Description:  This class encapsulates test results from live execution runs
*                as well as logged results and presents a common interface for
*                both.
*
*/


// System Include Files
#include <centralrepository.h>                  // CRepository
#include <drmserviceapi.h>                      // CDrmServiceApi
#include <diagenginecommon.h>                   // MDiagEngineCommon
#include <diagpluginexecplan.h>                 // CDiagPluginExecPlan
#include <diagexecplanentry.h>                  // MDiagExecPlanEntry
#include <diagresultsdatabaseitem.h>            // CDiagResultsDatabaseItem
#include <diagplugin.h>                         // MDiagPlugin
#include <diagtestplugin.h>                     // MDiagTestPlugin
#include <diagpluginpool.h>                     // CDiagPluginPool
#include <diagresultsdatabasetestrecordinfo.h>  // TDiagResultsDatabaseTestRecordInfo
#include <diagresultsdatabase.h>                // RDiagResultsDatabase,
                                                // RDiagResultsDatabaseRecord
#include <diagframeworkdebug.h>                 // Debugging Macros

// User Include Files
#include "DevDiagExecutionResults.h"            // CDevDiagExecResults
#include "DevDiagApp.hrh"                    // UID definition
#include "DevDiag.pan"                 // Panic
#include "phonedoctorprivatecrkeys.h"           // CR Key ID Definitions

// Local Constants
const TInt KRecordsArrayGranularity = ( 5 );

// Local Data Types
typedef CArrayFixFlat< TDiagResultsDatabaseTestRecordInfo > CDatabaseRecordInfoArray;


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult* CDevDiagExecResults::CResult::NewL(
    const MDiagPlugin& aPlugin )
    {
    CResult* self = CResult::NewLC( aPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult* CDevDiagExecResults::CResult::NewLC(
    const MDiagPlugin& aPlugin )
    {
    CResult* self = new ( ELeave ) CResult( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Destructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult::~CResult()
    {
    delete iResult;
    iResult = NULL;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Return the test result.  This may be NULL if the test has not been run.
// ---------------------------------------------------------------------------
//
const CDiagResultsDatabaseItem* CDevDiagExecResults::CResult::Result() const
    {
    return iResult;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Return a reference to the plugin for this result.
// ---------------------------------------------------------------------------
//
const MDiagPlugin& CDevDiagExecResults::CResult::Plugin() const
    {
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Return the execution error code.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::CResult::ExecStatus() const
    {
    return iExecStatus;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Return the current execution step for this result.
// ---------------------------------------------------------------------------
//
TUint CDevDiagExecResults::CResult::CurrentStep() const
    {
    return iCurrentStep;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Return the total execution steps for this result.
// ---------------------------------------------------------------------------
//
TUint CDevDiagExecResults::CResult::TotalSteps() const
    {
    return iTotalSteps;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// This function takes ownership of a result object.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::CResult::SetResult(
    CDiagResultsDatabaseItem* aResult,
    TInt aError )
    {
    // Sanity check.
    if ( iResult )
        {
        delete iResult;
        }

    // Set the result and error code Ownership of the result is transferred.
    iResult = aResult;
    iExecStatus = aError;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// Set the progress information for this result.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::CResult::SetProgress( TUint aCurrentStep,
                                                TUint aTotalSteps )
    {
    iCurrentStep = aCurrentStep;
    iTotalSteps = aTotalSteps;
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// The default constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult::CResult( const MDiagPlugin& aPlugin )
:   iPlugin( aPlugin )
    {
    }

// ---------------------------------------------------------------------------
// From nested class CDevDiagExecResults::CResult.
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::CResult::ConstructL()
    {
    }


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults* CDevDiagExecResults::NewL(
    TUid aRecordId,
    CDiagPluginPool& aPluginPool,
    RDiagResultsDatabase& aDbSession,
    MDiagEngineCommon* aDiagEngine )
    {
    LOGSTRING3( "CDevDiagExecResults::NewL( %d, 0x%x )",
                aRecordId.iUid,
                aDiagEngine )

    CDevDiagExecResults* self = CDevDiagExecResults::NewLC( aRecordId,
                                                            aPluginPool,
                                                            aDbSession,
                                                            aDiagEngine );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults* CDevDiagExecResults::NewLC(
    TUid aRecordId,
    CDiagPluginPool& aPluginPool,
    RDiagResultsDatabase& aDbSession,
    MDiagEngineCommon* aDiagEngine )
    {
    LOGSTRING3( "CDevDiagExecResults::NewLC( %d, 0x%x )",
                aRecordId.iUid,
                aDiagEngine )

    CDevDiagExecResults* self = new ( ELeave ) CDevDiagExecResults(
        aRecordId,
        aPluginPool,
        aDbSession,
        aDiagEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::~CDevDiagExecResults()
    {
    LOGSTRING( "CDevDiagExecResults::~CDevDiagExecResults()" )
    iResults.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// This function is called during live execution to transfer ownership of a
// test result.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::AddEntryL( CDiagResultsDatabaseItem* aResult,
                                     TInt aStatus )
    {
    LOGSTRING3( "CDevDiagExecResults::AddEntryL( 0x%x, %d )",
                aResult,
                aStatus )

    // This is only valid for live executions.
    if ( iType != ETypeExecution )
        {
        delete aResult;
        User::Leave( KErrCompletion );
        }

    // Assign the result object to the corresponding item in the local array.
    iResults[ CurrentIndexL() ]->SetResult( aResult, aStatus );

    // Update the completed step counter.
    if ( iResults[ CurrentIndexL() ]->Plugin().Type() ==
        MDiagPlugin::ETypeTestPlugin )
        {
        iCompletedSteps += static_cast< const MDiagTestPlugin& >(
            iResults[ CurrentIndexL() ]->Plugin() ).TotalSteps();
        }
    iCurrentPluginStep = 0;
    }

// ---------------------------------------------------------------------------
// This function is called during live execution to transfer ownership of a
// test result.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::SetProgressL( TUint aCurrentStep, TUint aTotalSteps )
    {
    LOGSTRING3( "CDevDiagExecResults::SetProgressL( %d, %d )",
            aCurrentStep,
            aTotalSteps )

    // This is only valid for live executions.
    if ( iType != ETypeExecution )
        {
        return;
        }

    // Assign the progress to the corresponding item in the local array.
    iResults[ CurrentIndexL() ]->SetProgress( aCurrentStep, aTotalSteps );

    // Update the current step counter.
    if ( iResults[ CurrentIndexL() ]->Plugin().Type() ==
        MDiagPlugin::ETypeTestPlugin )
        {
        iCurrentPluginStep = aCurrentStep;
        }
    else
        {
        iCurrentPluginStep = 0;
        }
    }

// ---------------------------------------------------------------------------
// This function returns the corresponding results item from the local array.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult& CDevDiagExecResults::operator[] ( TInt aIndex )
    {
    return *iResults[ aIndex ];
    }

// ---------------------------------------------------------------------------
// This function returns the corresponding results item from the local array.
// ---------------------------------------------------------------------------
//
const CDevDiagExecResults::CResult& CDevDiagExecResults::operator[] (
    TInt aIndex ) const
    {
    return *iResults[ aIndex ];
    }

// ---------------------------------------------------------------------------
// This function returns the current item from the local array.  For results
// which are from execution, this is the current execution item.  For results
// which are from the database, this is always the last item.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CResult& CDevDiagExecResults::CurrentItemL()
    {
    return *iResults[ CurrentIndexL() ];
    }

// ---------------------------------------------------------------------------
// This function returns the current item from the local array.  For results
// which are from execution, this is the current execution item.  For results
// which are from the database, this is always the last item.
// ---------------------------------------------------------------------------
//
const CDevDiagExecResults::CResult& CDevDiagExecResults::CurrentItemL() const
    {
    return *iResults[ CurrentIndexL() ];
    }

// ---------------------------------------------------------------------------
// This function returns the number of items in the local array.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::Count() const
    {
    return iResults.Count();
    }

// ---------------------------------------------------------------------------
// This function returns the index of the currently-executing item.
// ---------------------------------------------------------------------------
//
TUint CDevDiagExecResults::CurrentIndexL() const
    {
    // For live execution, the index from the execution plan is returned.
    if ( iType == ETypeExecution )
        {
        TUint index = 0;
        index = iDiagEngine->ExecutionPlanL().CurrentIndex();
        return index;
        }

    // For logged test results, the last index is returned.
    return iResults.Count() - 1;
    }

// ---------------------------------------------------------------------------
// This utility function allows the result view to check if testing is
// complete so that it can cancel the progress dialog.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::IsLastTestL() const
    {
    return ( CurrentIndexL() == ( Count() - 1 ) );
    }

// ---------------------------------------------------------------------------
// This utility function checks to see if a test is being executed / was
// executed as a dependency.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::IsDependencyL( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( ( aIndex < Count() ), Panic( EPDApplicationGeneral ) );

    // For live execution, get the information from the execution plan.
    if ( iType == ETypeExecution )
        {
        TBool dependent = EFalse;
        dependent = iDiagEngine->ExecutionPlanL()[ aIndex ].AsDependency();
        return dependent;
        }

    // For logged test results, get the information from the test result.  The
    // NULL check is needed in case the plugin failed during running and did
    // not report a result.
    if ( iResults[ aIndex ]->Result() )
        {
        return iResults[ aIndex ]->Result()->WasDependency();
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// This function locks "execution" style results from further changes, by
// changing the results type and clearing the reference to the engine.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::Finalize()
    {
    LOGSTRING( "CDevDiagExecResults::Finalize()" )

    iDiagEngine = NULL;
    iType = ETypeLog;
    iCompletedSteps = iTotalSteps;
    iCurrentPluginStep = 0;
    }

// ---------------------------------------------------------------------------
// This function returns the total number of steps for all items in the
// execution results.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::TotalExecutionSteps() const
    {
    return iTotalSteps;
    }

// ---------------------------------------------------------------------------
// This function returns the current execution step based on all items in the
// execution results.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::CurrentExecutionStep() const
    {
    return iCompletedSteps + iCurrentPluginStep;
    }

// ---------------------------------------------------------------------------
// Gets information about the test log.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::GetSessionInformationL( TInt& aSessionNumber,
                                                  TInt& aTimeInterval,
                                                  TTime& aStartTime,
                                                  TTime& aEndTime ) const
    {
    LOGSTRING( "CDevDiagExecResults::GetSessionInformationL()" )

    // Get the time interval from the Central Repository.
    CRepository* repository = CRepository::NewLC( KCRUidPhoneDoctor );
    User::LeaveIfError( repository->Get( KPDAppMaxUseTime, aTimeInterval ) );
    CleanupStack::PopAndDestroy( repository );

    // Get the list of database records.
    CDatabaseRecordInfoArray* oldRecords =
        new ( ELeave ) CDatabaseRecordInfoArray( KRecordsArrayGranularity );
    CleanupStack::PushL( oldRecords );
    User::LeaveIfError( iDbSession.GetAllRecordInfos( *oldRecords ) );

    // Get the current DRM time.
    TTime currentTime( TInt64( 0 ) );
    DRM::CDrmServiceApi* drmApi = DRM::CDrmServiceApi::NewLC();
    TInt timeZone = 0;
    DRMClock::ESecurityLevel secLevel = DRMClock::KInsecure;
    User::LeaveIfError( drmApi->GetSecureTime( currentTime,
                                               timeZone,
                                               secLevel ) );
    CleanupStack::PopAndDestroy( drmApi );

    // Go through the records list.
    aSessionNumber = 0;
    for ( TInt i = 0; i < oldRecords->Count(); i++ )
        {
        TDiagResultsDatabaseTestRecordInfo record = (*oldRecords)[ i ];

        // If this record matches the one we have execution results for, store
        // its start and end time.
        if ( record.iRecordId == iRecordId )
            {
            aStartTime = record.iStartTime;
            aEndTime = record.iFinishTime;
            }

        // Check the time interval that this record occured in.
        TTimeIntervalSeconds timeDifference( 0 );
        currentTime.SecondsFrom( record.iDrmStartTime, timeDifference );
        if ( timeDifference.Int() <= aTimeInterval )
            {
            aSessionNumber++;
            }
        }

    // Free the records information.
    CleanupStack::PopAndDestroy( oldRecords );
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CDevDiagExecResults( TUid aRecordId,
                                          CDiagPluginPool& aPluginPool,
                                          RDiagResultsDatabase& aDbSession,
                                          MDiagEngineCommon* aDiagEngine )
:   iRecordId( aRecordId ),
    iPluginPool( aPluginPool ),
    iDbSession( aDbSession ),
    iDiagEngine( aDiagEngine )
    {
    LOGSTRING3( "CDevDiagExecResults::CDevDiagExecResults( %d, 0x%x )",
                aRecordId.iUid,
                aDiagEngine )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::ConstructL()
    {
    LOGSTRING( "CDevDiagExecResults::ConstructL()" )

    // There are two types of results: (1) Current Execution, and (2) Logs.

    // (1) If a pointer to the diagnostics engine was provided, then it is
    // assumed that the results are from a live execution and this object will
    // wait for the results to be assigned .
    if ( iDiagEngine )
        {
        // This is the "current execution" type.
        iType = ETypeExecution;
        const MDiagPluginExecPlan& execPlan = iDiagEngine->ExecutionPlanL();

        // Add an entry for every item in the plan.
        TUint numItems = execPlan.Count();
        for ( TUint i = 0; i < numItems; i++ )
            {
            CResult* result = CResult::NewLC( execPlan[i].Plugin() );
            iResults.AppendL( result );
            CleanupStack::Pop( result );

            // Update the total step counter with this plugin's information.
            if ( execPlan[i].Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
                {
                iTotalSteps += static_cast< const MDiagTestPlugin& >(
                    execPlan[i].Plugin() ).TotalSteps();
                }
            }
        }
    // (2) If no pointer to the diagnostics engine was provided, then all the
    // results which correspond to the record id will be looked up from the
    // results database.
    else
        {
        // This is the "logged" type.
        iType = ETypeLog;

        // Get the record's results from the database.
        RDiagResultsDatabaseRecord dbRecord;
        User::LeaveIfError( dbRecord.Connect( iDbSession, iRecordId ) );
        CleanupClosePushL( dbRecord );
        RPointerArray< CDiagResultsDatabaseItem > logResults;
        CleanupClosePushL( logResults );
        User::LeaveIfError( dbRecord.GetTestResults( logResults ) );

        // Add an entry for each result item.
        TUint numItems = logResults.Count();
        for ( TUint i = 0; i < numItems; i++ )
            {
            MDiagPlugin* plugin = NULL;
            TInt err = iPluginPool.FindPlugin( logResults[i]->TestUid(),
                                               plugin );

            // If a matching plugin was not found for this result, we just
            // skip over it, because we can't do anything useful with it.
            // This case should only occur if a plugin logged results
            // previously, but was later removed from the phone.
            if ( err != KErrNone )
                {
                continue; //lint !e960 A continue makes sense here.
                }

            CResult* result = CResult::NewLC( *plugin );

            // Ownership of the results item is transferred.
            result->SetResult( logResults[i], KErrNone );
            logResults[i] = NULL;

            iResults.AppendL( result );
            CleanupStack::Pop( result );
            }

        CleanupStack::Pop(); // logResults
        CleanupStack::PopAndDestroy(); // dbRecord
        logResults.ResetAndDestroy();

        // Set dummy numbers for the execution steps.
        iTotalSteps = 1;
        iCompletedSteps = 1;
        }
    }

// End of File
