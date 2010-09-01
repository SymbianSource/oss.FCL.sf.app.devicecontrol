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
#include <DiagEngineCommon.h>                   // MDiagEngineCommon
#include <DiagPluginExecPlan.h>                 // CDiagPluginExecPlan
#include <DiagExecPlanEntry.h>                  // MDiagExecPlanEntry
#include <DiagResultsDatabaseItem.h>            // CDiagResultsDatabaseItem
#include <DiagPlugin.h>                         // MDiagPlugin
#include <DiagTestPlugin.h>                     // MDiagTestPlugin
#include <DiagPluginPool.h>                     // CDiagPluginPool
#include <DiagResultsDatabaseTestRecordInfo.h>  // TDiagResultsDatabaseTestRecordInfo
#include <DiagResultsDbRecordEngineParam.h>     // CDiagResultsDbRecordEngineParam
#include <DiagResultsDatabase.h>                // RDiagResultsDatabase,
                                                // RDiagResultsDatabaseRecord
#include <DiagFrameworkDebug.h>                 // Debugging Macros

// User Include Files
#include "devdiagexecutionresults.h"                 // CDevDiagExecResults
#include "devdiagapp.hrh"                    // UID definition
#include "devdiag.pan"                    // Panic
#include "devdiagprivatecrkeys.h"           // CR Key ID Definitions

// Local Constants
///@@@KSR: changes for BAD Warnings - #177-D: variable "KRecordsArrayGranularity" was declared but never referenced
//const TInt KRecordsArrayGranularity = ( 5 );

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
    RDiagResultsDatabase& aDbSession )
    {
    LOGSTRING2( "CDevDiagExecResults::NewL( %d )", aRecordId.iUid )

    CDevDiagExecResults* self = new ( ELeave ) CDevDiagExecResults(
        aRecordId,
        aPluginPool,
        aDbSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults* CDevDiagExecResults::NewL(
    TUid aRecordId,
    CDiagPluginPool& aPluginPool,
    RDiagResultsDatabase& aDbSession,
    TUid aExecutedUid,
    MDiagEngineCommon* aDiagEngine )
    {
    LOGSTRING3( "CDevDiagExecResults::NewL( %d, 0x%x )",
                aRecordId.iUid,
                aDiagEngine )

    CDevDiagExecResults* self = new ( ELeave ) CDevDiagExecResults(
        aRecordId,
        aPluginPool,
        aDbSession,
        aExecutedUid,
        aDiagEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
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

        if ( aResult && ( aResult->TestResult() ==
                            CDiagResultsDatabaseItem::EFailed ||
                          aResult->TestResult() ==
                            CDiagResultsDatabaseItem::EDependencyFailed ) )
            {
            iFailedCount++;
            }
        
        if ( aResult && ( aResult->TestResult() ==
                            CDiagResultsDatabaseItem::ESuccess) )
            {
            iPassedCount++;
            }
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
// complete but the diagnostics engine has not yet reported testing as
// finished.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::LastTestFinishedL() const
    {
    if ( !iDiagEngine ||
         ( iDiagEngine->ExecutionPlanL().IsLastTest() &&
           ( iResults[ iLastTestIndex ] )->Result() ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// This utility function checks to see if a test is being executed / was
// executed as a dependency.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::IsDependencyL( TInt aIndex ) const
    {
    //__ASSERT_ALWAYS( ( aIndex < Count() ), Panic( EDevDiagApplicationGeneral ) );

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
// Returns whether or not the results are from a completed test record.  This
// information is used to determine the validity of the session's "end time."
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::IsRecordCompleted() const
    {
    return iCompleted;
    }

// ---------------------------------------------------------------------------
// This function locks "execution" style results from further changes, by
// changing the results type and clearing the reference to the engine.
// ---------------------------------------------------------------------------
//
void CDevDiagExecResults::Finalize( TBool aTestingComplete )
    {
    LOGSTRING( "CDevDiagExecResults::Finalize()" )

    iDiagEngine = NULL;
    iType = ETypeLog;
    iCompletedSteps = iTotalSteps;
    iCurrentPluginStep = 0;
    iCompleted = aTestingComplete;
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
// Returns a reference to the plugin that execution was requested for.
// ---------------------------------------------------------------------------
//
const MDiagPlugin& CDevDiagExecResults::ExecutedPluginL() const
    {
    LOGSTRING( "CDevDiagExecResults::ExecutedPluginL()" )

    MDiagPlugin* plugin = NULL;
    User::LeaveIfError( iPluginPool.FindPlugin( iExecutedPlugin, plugin ) );

    return *plugin;
    }

// ---------------------------------------------------------------------------
// Returns whether or not the execution is for a single plugin.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::SinglePluginExecutionL() const
    {
    if ( !iDiagEngine )
        {
        return EFalse;
        }
    
    return ( iDiagEngine->ExecutionPlanL().TestCount( EFalse ) == 1 );
    }

// ---------------------------------------------------------------------------
// Returns the number of failed tests in this set of results.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::FailedResultsCount() const
    {
    return iFailedCount;
    }


// ---------------------------------------------------------------------------
// Returns the number of passed tests in this set of results.
// ---------------------------------------------------------------------------
//
TInt CDevDiagExecResults::PassedResultsCount() const
    {
    return iPassedCount;
    }

// ---------------------------------------------------------------------------
// Returns whether or not the execution plan's resume index has been reached.
// ---------------------------------------------------------------------------
//
TBool CDevDiagExecResults::ResumeIndexReachedL() const
    {
    if ( !iDiagEngine )
        {
        return ETrue;
        }

    return ( iDiagEngine->ExecutionPlanL().CurrentIndex() >=
        iDiagEngine->ExecutionPlanL().ResumeIndex() );
    }

// ---------------------------------------------------------------------------
// Returns the UID of the record associated with this set of results.
// ---------------------------------------------------------------------------
//
const TUid& CDevDiagExecResults::RecordId() const
    {
    return iRecordId;
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDevDiagExecResults::CDevDiagExecResults( TUid aRecordId,
                                          CDiagPluginPool& aPluginPool,
                                          RDiagResultsDatabase& aDbSession,
                                          TUid aExecutedUid,
                                          MDiagEngineCommon* aDiagEngine )
:   iRecordId( aRecordId ),
    iPluginPool( aPluginPool ),
    iDbSession( aDbSession ),
    iDiagEngine( aDiagEngine ),
    iExecutedPlugin( aExecutedUid )
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
            CResult* result = CResult::NewLC( execPlan[ i ].Plugin() );
            iResults.AppendL( result );
            CleanupStack::Pop( result );

            // Update the total step counter with this plugin's information.
            if ( execPlan[ i ].Plugin().Type() == MDiagPlugin::ETypeTestPlugin )
                {
                iTotalSteps += static_cast< const MDiagTestPlugin& >(
                    execPlan[ i ].Plugin() ).TotalSteps();
                iLastTestIndex = i;
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
        User::LeaveIfError( dbRecord.Connect( iDbSession, iRecordId, ETrue ) );
        CleanupClosePushL( dbRecord );
        User::LeaveIfError( dbRecord.IsTestCompleted( iCompleted ) );
        RPointerArray< CDiagResultsDatabaseItem > logResults;
        CleanupClosePushL( logResults );
        User::LeaveIfError( dbRecord.GetTestResults( logResults ) );

        // Add an entry for each result item.
        TUint numItems = logResults.Count();
        for ( TUint i = 0; i < numItems; i++ )
            {
            MDiagPlugin* plugin = NULL;
            TInt err = iPluginPool.FindPlugin( logResults[ i ]->TestUid(),
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
            result->SetResult( logResults[ i ], KErrNone );
            logResults[ i ] = NULL;

            // Get the test result.
            if ( result->Result()->TestResult() ==
                    CDiagResultsDatabaseItem::EFailed ||
                 result->Result()->TestResult() ==
                    CDiagResultsDatabaseItem::EDependencyFailed )
                {
                iFailedCount++;
                }

            iResults.AppendL( result );
            CleanupStack::Pop( result );
            }
        iLastTestIndex = numItems - 1;

        CleanupStack::Pop(); // logResults
        logResults.ResetAndDestroy();

        // Get the uid of the executed test.
        CDiagResultsDbRecordEngineParam* execParam;
        User::LeaveIfError( dbRecord.GetEngineParam( execParam ) );
        iExecutedPlugin = execParam->ExecutionsUidArray()[ 0 ];
        delete execParam;

        CleanupStack::PopAndDestroy(); // dbRecord

        // Set dummy numbers for the execution steps.
        iTotalSteps = 1;
        iCompletedSteps = 1;
        }
    }
    
    

 const CDiagResultsDatabaseItem* CDevDiagExecResults::GetSinglepluginExecutionResult() const
    {
    
    	return iResults[ iLastTestIndex ]->Result();
    	/*
    	CDiagResultsDatabaseItem* aResult;
    	const CDevDiagExecResults::CResult& result = CDevDiagExecResults::CurrentItemL();
    	aResult = result.Result();
    	return aResult;*/
    	
    }
// End of File
