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
* Description:  Engine implementation
*
*/


// System Include Files
#include <e32std.h>                     // User
#include <f32file.h>                    // RFs
#include <DiagEngine.h>                 // CDiagEngine
#include <DiagPlugin.h>                 // MDiagPlugin
#include <DiagSuitePlugin.h>            // MDiagSuitePlugin
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem
#include <DiagPluginPool.h>             // CDiagPluginPool
#include <DiagResultsDatabaseTestRecordInfo.h>
                                        // TDiagResultsDatabaseTestRecordInfo
#include <drmserviceapi.h>              // CDrmServiceApi
#include <e32property.h>                // RProperty
#include <PSVariables.h>                // Property values
#include <startupdomainpskeys.h>        // Property values
#include <centralrepository.h>          // CRepository
#include <ProfileEngineSDKCRKeys.h>     // CR Keys for Profile
#include <DiagFrameworkDebug.h>         // Debug Logger
#include <DiagResultsDbRecordEngineParam.h>

// User Include Files
#include "devdiagapp.hrh"               // UID definition
#include "devdiagengine.h"              // CDevDiagEngine
#include "devdiagengineobserver.h"      // CDevDiagEngineObserver
#include "devdiagexecutionresults.h"    // CDevDiagExecResults
#include "devdiag.pan"                  // Panic
#include "devdiagcommoncanceldialogs.h"       // CDevDiagCancelExecutionDialog
#include "devdiagcommonskipdialogs.h"
// Local Constants
const TInt KErrDevDiagAlreadyInitialized = ( -1 );
const TInt KErrDevDiagAlreadyLoaded      = ( -2 );
const TInt KErrDevDiagUninitialized      = ( -3 );

///@@@KSR: changes for BAD Warnings - #177-D: variable "KErrDevDiagExecuting" was declared but never referenced
//const TInt KErrDevDiagExecuting          = ( -4 );

const TInt KErrDevDiagAlreadyRunning     = ( -5 );
const TInt KErrDevDiagNotRunning         = ( -6 );
const TInt KErrDevDiagSuspendResume      = ( -7 );
const TUid KUidDevDiagApplication        = { _UID3 };
///@@@KSR: changes for BAD Warnings - #177-D: variable "KProgressGranularity" was declared but never referenced
//const TInt KProgressGranularity     = ( 2 );

// Local Data Types
typedef CArrayFixFlat< TDiagResultsDatabaseTestRecordInfo > CDatabaseRecordInfoArray;

const TInt KArrayGranuality(50);


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagEngine* CDevDiagEngine::NewL()
    {
    LOGSTRING( "CDevDiagEngine::NewL()" )

    CDevDiagEngine* self = CDevDiagEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDevDiagEngine* CDevDiagEngine::NewLC()
    {
    LOGSTRING( "CDevDiagEngine::NewLC()" )

    CDevDiagEngine* self = new ( ELeave ) CDevDiagEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDevDiagEngine::~CDevDiagEngine()
    {
    LOGSTRING( "CDevDiagEngine::~CDevDiagEngine()" )

    // Cancel any outstanding asynchronous requests.
    Cancel();

    // Delete the member variables.
    delete iResults;
    delete iDiagEngine;
    delete iPluginPool;
    
    if ( iUids )
        {
        delete iUids;
        iUids = NULL;
        }

    iLastResults.ResetAndDestroy();
    iLastResults.Close();

    // Close the session with the results database.
    iResultsDatabase.Close();
    }


// ---------------------------------------------------------------------------
// This function allows the UI to receive application engine callbacks by
// implementing the MDevDiagEngineObserver interface.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::SetObserver( MDevDiagEngineObserver* aObserver )
    {
    LOGSTRING2( "CDevDiagEngine::SetObserver( 0x%x )", aObserver )

    if ( iObserver && aObserver )
        {
        // We have cannot leave here.
        TRAP_IGNORE( iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandObserverChanged,
            KErrNone,
            NULL ) )
        }

    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// This function begins test execution.  It is the responsibility of the
// caller to check the runtime requirements prior to calling this function.
// This is an asynchronous request.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::ExecuteTestL( TUid aUid, CAknViewAppUi& aAppUi )
    {
    LOGSTRING2( "CDevDiagEngine::ExecuteTestL( 0x%x )", aUid.iUid )

    // Reset the member variables to prepare for a new execution run.
    delete iDiagEngine;
    iDiagEngine = NULL;
    delete iResults;
    iResults = NULL;
    iSuspendCounter = 0;

    RArray< TUid > uidArray;
    uidArray.Append( aUid );
    CleanupClosePushL( uidArray );

    // Create an engine instance and execute the test or suite.
    iDiagEngine = CDiagEngine::NewL( aAppUi,
                                     *this,
                                     iResultsDatabase,
                                     *iPluginPool,
                                     EFalse,
                                     uidArray );
    CleanupStack::PopAndDestroy(); // uidArray
    iDiagEngine->ExecuteL();

    // Do the state transition now that we are successfully starting.
    SetState( EStateStartingExecution );
    }

// ---------------------------------------------------------------------------
// This function will provides the execution results, which may be from an
// execution run which is ongoing, or from logged test results.
// ---------------------------------------------------------------------------
//
const CDevDiagExecResults& CDevDiagEngine::ExecutionResults() const
    {
    LOGSTRING( "CDevDiagEngine::ExecutionResults()" )

    __ASSERT_ALWAYS( HasExecutionResults(),
                     Panic( EDevDiagApplicationNotInitialized ) );
    return *iResults;
    }

// ---------------------------------------------------------------------------
// This function will handle getting the results information for the most
// recent test execution.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::LoadLastLoggedResultsL()
    {
    LOGSTRING( "CDevDiagEngine::LoadLastLoggedResultsL()" )

    // Check the state.
    __ASSERT_ALWAYS( iState == EStateReady,
                     Panic( EDevDiagApplicationInvalidEngineState ) );

    // If there are any existing results, just use them.
    if ( iResults )
        {
        return;
        }

    // Get info about the last test record.  If there are no existing records,
    // just return -- the caller must check if there are results before they
    // do anything.
    TUid lastExecutionRecord;
    if ( iResultsDatabase.GetLastRecord( lastExecutionRecord ) != KErrNone )
        {
        return;
        }

    // Now, load the logged results.
    iResults = CDevDiagExecResults::NewL( lastExecutionRecord,
                                          *iPluginPool,
                                          iResultsDatabase );
    }

// ---------------------------------------------------------------------------
// This function will stop test execution.  The argument specifies whether all
// execution should be stopped, or just the currently running test.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::ExecutionStopL( TDevDiagAppEngineStopMode aReason )
    {
   LOGSTRING2( "CDevDiagEngine::ExecutionStopL( %d )", aReason )

    // Check the state.
    __ASSERT_ALWAYS( IsRunningPlugins(),
                     Panic( EDevDiagApplicationInvalidEngineState ) );

    // Stop the execution.
    switch ( aReason )
        {
        case EStopModeSkip:
            iDiagEngine->ExecutionStopL( MDiagEngineCommon::ESkip );
            break;

        case EStopModeCancel:
            SetState( EStateStoppingExecution );

            
            // Start an idle active object, which will handle deleting the
            // diagnostics engine.  This cannot be done here because the
            // engine may be executing a test which is displaying the "Cancel
            // Execution" dialog, which calls this function.  The priority of
            // this CIdle MUST be higher than then diagnostics engine and ALL
            // of its active objects.
            delete iIdle;
            iIdle = NULL;
            iIdle = CIdle::NewL( CActive::EPriorityHigh );
            iIdle->Start( TCallBack( HandleExecutionCancelledL, this ) );

            // Inform the observer.  In the case where the observer is
            // displaying a dialog, this will allow the observer to cancel it.
            if ( iObserver )
                {
                iObserver->HandleEngineCommandL(
                    MDevDiagEngineObserver::EDevDiagEngineCommandExecutionStopping,
                    KErrCancel,
                    NULL );
                }
            
            break;

        case EStopModeSuspend:
            // Increment the suspend counter and check the state.
            LOGSTRING2( "CDevDiagEngine::ExecutionStopL, counter: %d",
                        iSuspendCounter )
            iSuspendCounter++;
            if ( iState == EStateExecutionSuspended )
                {
                // Do nothing - we're already suspended.
                __ASSERT_DEBUG( iSuspendCounter > 1,
                                Panic( EDevDiagApplicationInvalidEngineState ) );
                break;
                }

            SetState( EStateExecutionSuspended );
            iDiagEngine->SuspendL();
            break;

        case EStopModeWatchdog:
            iDiagEngine->StopWatchdogTemporarily();
            break;

        default:
            __ASSERT_DEBUG( EFalse, Panic( EDevDiagApplicationInvalidStopMode ) );
            break;
        }
    }


TInt CDevDiagEngine::HandleExecutionCancelledL( TAny* aPtr )
    {
    LOGSTRING2( "CDevDiagEngine::HandleExecutionCancelledL( 0x%x )", aPtr )

    CDevDiagEngine* myThis = static_cast< CDevDiagEngine* >( aPtr );
    myThis->iResults->Finalize( EFalse );

    delete myThis->iDiagEngine;
    myThis->iDiagEngine = NULL;

    delete myThis->iIdle;
    myThis->iIdle = NULL;

    myThis->SetState( EStateReady );

    // Inform the observer object.
    if ( myThis->iObserver )
        {
        
        myThis->iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandTestExecutionCancelled,
            KErrCancel,
            NULL );
            
        }

    return KErrNone;
    
    }
// ---------------------------------------------------------------------------
// This function will resume suspended test execution.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::ExecutionResumeL( TDevDiagAppEngineResumeMode aReason )
    {
    LOGSTRING2( "CDevDiagEngine::ExecutionResumeL( %d )", aReason )

    // Check the state.
    __ASSERT_ALWAYS( IsRunningPlugins(),
                     Panic( EDevDiagApplicationInvalidEngineState ) );

    // Resume the execution.
    switch ( aReason )
        {
        case EResumeModeResume:
            {
            LOGSTRING2( "CDevDiagEngine::ExecutionResumeL, counter: %d",
                        iSuspendCounter )
            // Decrement the suspend counter and check the state.
            --iSuspendCounter;
            __ASSERT_DEBUG( iSuspendCounter >= 0,
                            Panic( EDevDiagApplicationInvalidEngineState ) );
            __ASSERT_ALWAYS( iState == EStateExecutionSuspended,
                            Panic( EDevDiagApplicationInvalidEngineState ) );

            // Resume the execution, but only if there have been enough
            // "resume" calls to match the number of "suspend" calls.
            if ( iSuspendCounter == 0 )
                {
                SetState( EStateRunningTests );
                iDiagEngine->ResumeL();
                }

            break;
            }

        case EResumeModeWatchdog:
           // iDiagEngine->ResetWatchdogL();
            break;

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDevDiagApplicationInvalidEngineState ) );
            break;
        }
    }

// ---------------------------------------------------------------------------
// Returns the plugin pool.
// ---------------------------------------------------------------------------
//
const CDiagPluginPool& CDevDiagEngine::PluginPool() const
    {
    LOGSTRING( "CDevDiagEngine::PluginPool()" )

    __ASSERT_ALWAYS( ArePluginsLoaded(),
                   Panic( EDevDiagApplicationNotInitialized ) );
    return *iPluginPool;
    }

// ---------------------------------------------------------------------------
// Returns whether or not tests are running.
// ---------------------------------------------------------------------------
//
TBool CDevDiagEngine::IsRunningPlugins() const
    {
    LOGSTRING( "CDevDiagEngine::IsRunningPlugins()" )

    return ( iState == EStateStartingExecution ||
             iState == EStateRunningTests ||
             iState == EStateStoppingExecution ||
             iState == EStateExecutionSuspended );
    }

// ---------------------------------------------------------------------------
// Checks if the engine is currently stopping execution.
// ---------------------------------------------------------------------------
//
TBool CDevDiagEngine::IsStoppingExecution() const
    {
    LOGSTRING( "CDevDiagEngine::IsStoppingExecution()" )

    return ( iState == EStateStoppingExecution );
    }

// ---------------------------------------------------------------------------
// Returns whether or not plugins are done loading.
// ---------------------------------------------------------------------------
//
TBool CDevDiagEngine::ArePluginsLoaded() const
    {
    LOGSTRING( "CDevDiagEngine::ArePluginsLoaded()" )

    return ( iState != EStateInitial &&
             iState != EStateLoadingPlugins );
    }

// ---------------------------------------------------------------------------
// Returns whether execution results are available.
// ---------------------------------------------------------------------------
//
TBool CDevDiagEngine::HasExecutionResults() const
    {
    LOGSTRING( "CDevDiagEngine::HasExecutionResults()" )

    if ( iResults )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDevDiagEngine::CDevDiagEngine() : CActive( EPriorityStandard ),                            
                               iDiagEngine( NULL )
    {
    LOGSTRING( "CDevDiagEngine::CDevDiagEngine()" )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::ConstructL()
    {
    LOGSTRING( "CDevDiagEngine::ConstructL()" )

    // Connect to the results database.
    User::LeaveIfError( iResultsDatabase.Connect( KUidDevDiagApplication ) );
    LOGSTRING( "CDevDiagEngine::ResultDB connect");
    // Load the plugins.
    iPluginPool = CDiagPluginPool::NewL( *this );
    LOGSTRING( "CDevDiagEngine::NewL");
    SetState( EStateLoadingPlugins );
    iPluginPool->LoadAsyncL( KDiagPluginInterfaceUid );
    LOGSTRING( "CDevDiagEngine::LoadPlugin");
    // Add the application engine to the active scheduler.
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// To obtain the current engine state
// ---------------------------------------------------------------------------
TInt CDevDiagEngine::GetState()
	{
	return iState;
	}


// ---------------------------------------------------------------------------
// The state machine transition handler.  If there is an error in changing
// states, then it will be returned and the state will be left unchanged.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::SetState( TDevDiagAppEngineState aNextState )
    {
    LOGSTRING3( "CDevDiagEngine::SetState( %d ), iState = %d",
                aNextState,
                iState )    

    // The state transition table.  Structure: State in / State to go to.
    static const TInt KStateTable[ EStateMax ][ EStateMax ] =
        {
            // EStateInitial
            {
            KErrNone,                        // To EStateInitial
            KErrNone,                        // To EStateLoadingPlugins
            KErrDevDiagUninitialized,        // To EStateReady
            KErrDevDiagUninitialized,        // To EStateStartingExecution
            KErrDevDiagUninitialized,        // To EStateRunningTests
            KErrDevDiagUninitialized,        // To EStateStoppingExecution
            KErrDevDiagUninitialized         // To EStateExecutionSuspended
            },

            // EStateLoadingPlugins
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrNone,                        // To EStateLoadingPlugins
            KErrNone,                        // To EStateReady
            KErrDevDiagUninitialized,        // To EStateStartingExecution
            KErrDevDiagUninitialized,        // To EStateRunningTests
            KErrDevDiagUninitialized,        // To EStateStoppingExecution
            KErrDevDiagUninitialized         // To EStateExecutionSuspended
            },

            // EStateReady
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrDevDiagAlreadyLoaded,        // To EStateLoadingPlugins
            KErrNone,                        // To EStateReady
            KErrNone,                        // To EStateStartingExecution
            KErrDevDiagNotRunning,           // To EStateRunningTests
            KErrDevDiagNotRunning,           // To EStateStoppingExecution
            KErrDevDiagNotRunning            // To EStateExecutionSuspended
            },

            // EStateStartingExecution
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrDevDiagAlreadyLoaded,        // To EStateLoadingPlugins
            KErrNone,                        // To EStateReady
            KErrNone,                        // To EStateStartingExecution
            KErrNone,                        // To EStateRunningTests
            KErrNone,                        // To EStateStoppingExecution
            KErrDevDiagNotRunning            // To EStateExecutionSuspended
            },

            // EStateRunningTests
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrDevDiagAlreadyLoaded,        // To EStateLoadingPlugins
            KErrNone,                        // To EStateReady
            KErrDevDiagAlreadyRunning,       // To EStateStartingExecution
            KErrNone,                        // To EStateRunningTests
            KErrNone,                        // To EStateStoppingExecution
            KErrNone                         // To EStateExecutionSuspended
            },

            // EStateStoppingExecution
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrDevDiagAlreadyLoaded,        // To EStateLoadingPlugins
            KErrNone,                        // To EStateReady
            KErrDevDiagAlreadyRunning,       // To EStateStartingExecution
            KErrDevDiagAlreadyRunning,       // To EStateRunningTests
            KErrNone,                        // To EStateStoppingExecution
            KErrDevDiagNotRunning            // To EStateExecutionSuspended
            },

            // EStateExecutionSuspended
            {
            KErrDevDiagAlreadyInitialized,   // To EStateInitial
            KErrDevDiagAlreadyLoaded,        // To EStateLoadingPlugins
            KErrDevDiagSuspendResume,        // To EStateReady
            KErrDevDiagAlreadyRunning,       // To EStateStartingExecution
            KErrNone,                        // To EStateRunningTests
            KErrNone,                        // To EStateStoppingExecution
            KErrNone                         // To EStateExecutionSuspended
            }
        };

    // Check if there is an invalid state.
    __ASSERT_DEBUG( ( iState < EStateMax && aNextState < EStateMax ),
                    Panic( EDevDiagApplicationInvalidEngineState ) );

    // Change the state if there was no error, and return.
    __ASSERT_ALWAYS( KStateTable[ iState ][ aNextState ] == KErrNone,
                     Panic( EDevDiagApplicationInvalidEngineState ) );
    iState = aNextState;
    
    }

// ---------------------------------------------------------------------------
// From class CActive.
// The active object completion function.  This is only used for checking the
// runtime requirements.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::RunL()
    {
    LOGSTRING( "CDevDiagEngine::RunL()" )

        if ( iStatus.Int() == KErrNone )
            {
            TInt error = iResultsDatabase.GetLastResults( iLastResults );
            
            if ( error != KErrNone )
                {
                LOGSTRING2( "CDevDiagEngine::RunL GetLastResults error: %d", error );
                
                }
            
            if ( iObserver )
                {
                iObserver->HandleEngineCommandL(
                MDevDiagEngineObserver::EDevDiagEngineCommandGetLastResults,
                error,
                &iLastResults );
                }                
            }
        else 
            {
            LOGSTRING2( "CDevDiagEngine::RunL error: %d", iStatus.Int() );
            }
        
    }

// ---------------------------------------------------------------------------
// CDevDiagEngine::RunError
// ---------------------------------------------------------------------------
//
TInt CDevDiagEngine::RunError( TInt aError )
	{
		LOGSTRING2( "CDevDiagEngine::RunError error: %d", aError );      
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// From class CActive.
// The active object cancellation function.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::DoCancel()
    {
    LOGSTRING( "CDevDiagEngine::DoCancel()" )

    // Stop plugin loading, if it is ongoing.
    if ( iState == EStateLoadingPlugins )
        {
        // Cannot leave here.
        TRAP_IGNORE( iPluginPool->CancelLoadPluginsL() )
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate that test execution is starting.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionBeginL()
    {
    LOGSTRING( "CDevDiagEngine::TestExecutionBeginL()" )

    SetState( EStateRunningTests );

    // Get the record info, so we can get the record uid.
    TDiagResultsDatabaseTestRecordInfo recordInfo;
    User::LeaveIfError( iDiagEngine->DbRecord().GetRecordInfo( recordInfo ) );

    // Get the engine parameters, so we can get the executed uid.
    CDiagResultsDbRecordEngineParam* execParam;
    User::LeaveIfError( iDiagEngine->DbRecord().GetEngineParam( execParam ) );
    CleanupStack::PushL( execParam );

    // Initialize the results data.
    delete iResults;
    iResults = NULL;
    iResults = CDevDiagExecResults::NewL( recordInfo.iRecordId,
                                          *iPluginPool,
                                          iResultsDatabase,
                                          execParam->ExecutionsUidArray()[ 0 ],
                                          iDiagEngine );

    CleanupStack::PopAndDestroy( execParam );

    // Inform the observer object.
    if ( iObserver )
        {
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandTestExecutionBegin,
            KErrNone,
            NULL );
        }                
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate test progress.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionProgressL( TUint aCurrentItemStep,
                                           TUint aCurrentItemTotalSteps )
    {
    LOGSTRING3( "CDevDiagEngine::TestExecutionProgressL( %d, %d )",
                aCurrentItemStep,
                aCurrentItemTotalSteps )

    // Update the results data.
    __ASSERT_ALWAYS( iResults, Panic( EDevDiagApplicationInvalidEngineState ) );
    iResults->SetProgressL( aCurrentItemStep, aCurrentItemTotalSteps );

    // Inform the observer object.
    if ( iObserver )
        {
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandProgressDataUpdated,
            KErrNone,
            NULL );
        }
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate that a plugin has completed and provide the
// result.  Ownership of aResult is transferred.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionPluginExecutedL(
    TInt aError,
    CDiagResultsDatabaseItem* aResult )
    {
    LOGSTRING3( "CDevDiagEngine::TestExecutionPluginExecutedL( %d, 0x%x )",
                aError,
                aResult )

    // Update the results data.
    __ASSERT_ALWAYS( iResults, Panic( EDevDiagApplicationNoExecutionResults ) );
    iResults->AddEntryL( aResult, aError );
    
    if ( aResult )
        {
        TInt result = aResult->TestResult();    
        }    

    
    const MDiagPlugin& plugin = iResults->CurrentItemL().Plugin();
    
    if ( plugin.Type() == MDiagPlugin::ETypeTestPlugin  )
        {
         //Now we are executing a test (NULL indicates a test suite).
        if ( aResult )
            {
                MDevDiagEngineObserver::TAppEngineCommand cmd = MDevDiagEngineObserver::EDevDiagEngineCommandSinglePluginExecutionDone;
                if ( iObserver )
                    iObserver->HandleEngineCommandL(cmd,aError,NULL );
            }
        }

    /*
    MDevDiagEngineObserver::TAppEngineCommand cmd;
    
    const MDiagPlugin& plugin = iResults->CurrentItemL().Plugin();
    
    if ( plugin.Type() == MDiagPlugin::ETypeTestPlugin  )
        {
         //Now we are executing a test (NULL indicates a test suite).
        if ( aResult )
            {        javascript:submitForm('CreateAccountServlet', target='/accounts/my/create_selectService.jsp', '')
9
            cmd = MDevDiagEngineObserver::EDevDiagEngineCommandSinglePluginExecutionDone;
                 
            }    
        else 
            {
            //cmd = MDevDiagEngineObserver::EDevDiagEngineCommandGroupExecutionProgress;
            }
        
        if ( iObserver )
            {
            iObserver->HandleEngineCommandL(
                cmd,
                aError,
                NULL );
            }           
        }
        */
    }


// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate that the diagnostics engine has finished executing
// plugins.  This can also indicate a failure to begin executing plugins.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionCompletedL( TInt aError )
    {
    LOGSTRING2( "CDevDiagEngine::TestExecutionCompletedL( %d )", aError )

    SetState( EStateReady );
    
    TBool singleExecution = iResults->SinglePluginExecutionL();

    // Clean up the execution information.
    if ( iResults )
        {
        // The results are fully complete only if execution cannot be resumed,
        // so these are the only error codes that indicate that.
        iResults->Finalize( aError == KErrNone ||
                            aError == KErrCancel ||
                            aError == KErrArgument );
        }
        
    delete iDiagEngine;
    iDiagEngine = NULL;

    // Inform the observer object.
    if ( iObserver && !singleExecution )
        {
        
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandGroupExecutionDone,
            aError,
            NULL );           
        }
        
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate that test execution has been suspended.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionSuspendedL( TSuspendReason aSuspendReason )
    {
    LOGSTRING2( "CDevDiagEngine::TestExecutionSuspendedL( %d )",
                aSuspendReason )

    // Don't do anything if this suspend came from the application, as this
    // would duplicate the counter increase.
    if ( aSuspendReason == ESuspendByClient )
        {
        __ASSERT_DEBUG(
            ( iSuspendCounter > 0 && iState == EStateExecutionSuspended ),
            Panic( EDevDiagApplicationInvalidEngineState ) );
        return;
        }

    LOGSTRING3( "CDevDiagEngine::TestExecutionSuspendedL: counter %d, state: %d",
                iSuspendCounter,
                iState )
    // Increment the suspend counter and check the state.
    iSuspendCounter++;
    if ( iState == EStateExecutionSuspended )
        {
        // If we are already suspended, then this suspend has no effect.
        return;
        }

    // Suspend ourself and inform the user.
    SetState( EStateExecutionSuspended );
    if ( iObserver )
        {
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandTestExecutionSuspended,
            KErrNone,
            NULL );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to indicate that test execution has been resumed.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::TestExecutionResumedL( TResumeReason aResumeReason )
    {
    LOGSTRING2( "CDevDiagEngine::TestExecutionResumedL( %d )", aResumeReason )

    // Don't do anything if this suspend came from the application, as this
    // would duplicate the counter decrease.
    if ( aResumeReason == EResumedByClient )
        {
        return;
        }

    LOGSTRING3( "CDevDiagEngine::TestExecutionResumedL: counter %d, state: %d",
                iSuspendCounter,
                iState )
    // Decrement the suspend counter and check the state.
    --iSuspendCounter;
    __ASSERT_DEBUG( iSuspendCounter >= 0,
                    Panic( EDevDiagApplicationInvalidEngineState ) );
    if ( iState != EStateExecutionSuspended )
        {
        // The resume was already done, so just return.
        return;
        }

    // Resume ourself and inform the user.
    SetState( EStateRunningTests );
    if ( iObserver )
        {
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandTestExecutionResumed,
            KErrNone,
            NULL );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to to create a custom common-use dialog.  This may ONLY be
// called by the Diagnostics Engine.  Any application classes must use the
// dialog classes directly.
// ---------------------------------------------------------------------------
//
CAknDialog* CDevDiagEngine::CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                                TAny* aInitData )
    {
    LOGSTRING3( "CDevDiagEngine::CreateCommonDialogLC( %d, 0x%x )",
                aDialogType,
                aInitData )

    switch ( aDialogType )
        {
        case EDiagCommonDialogConfirmCancelAll:
            {
            __ASSERT_ALWAYS( !aInitData,
                             Panic( EDevDiagApplicationGeneral ) );
            return CDevDiagCommonCancelDialogs::NewLC( *this, EFalse );
            }
           
        case EDiagCommonDialogConfirmSkipAll:
        	{
            __ASSERT_ALWAYS( !aInitData,
                             Panic( EDevDiagApplicationGeneral ) );
            return CDevDiagCommonSkipDialogs::NewLC( *this, EFalse );        		
        	}
        }

    __ASSERT_DEBUG( EFalse, Panic( EDevDiagApplicationBadType ) );
    return NULL;
    }



// ---------------------------------------------------------------------------
// From class MDiagEngineObserver.
// The callback to to execute an application command. This may ONLY be 
// called by the Diagnostics Engine.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::ExecuteAppCommandL( TDiagAppCommand aCommand,
                                       TAny* aParam1,
                                       TAny* aParam2 )
    {
    LOGSTRING4( "CDevDiagEngine::ExecuteAppCommandL( %d, 0x%x, 0x%x )",
                aCommand,
                aParam1,
                aParam2 )

    switch ( aCommand )
        {
        case EDiagAppCommandSwitchToMainView:
            {
            __ASSERT_ALWAYS( ( !aParam1 && !aParam2 ),
                             Panic( EDevDiagApplicationGeneral ) );
            if ( iObserver )
                {
                iObserver->HandleEngineCommandL(
                    MDevDiagEngineObserver::EDevDiagEngineCommandViewSwitch,
                    KErrNone,
                    NULL );
                }
            break;
            }

        default:
            User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPluginPoolObserver.
// The callback to indicate plugin loading progress.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::LoadProgressL( TUint aCurrentStep,
                                  TUint aTotalSteps,
                                  const TUid& aLoadedPluginUid )
    {
   
    LOGSTRING4( "CDevDiagEngine::LoadProgressL( %d, %d, 0x%x )",
                aCurrentStep,
                aTotalSteps,
                aLoadedPluginUid.iUid )

    __ASSERT_ALWAYS( iState == EStateLoadingPlugins,
                     Panic( EDevDiagApplicationInvalidEngineState ) );

    // Inform the observer with the loading information.
    if ( iObserver )
        {
        MDiagPlugin* plugin = NULL;
        iPluginPool->FindPlugin( aLoadedPluginUid, plugin );

        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandPluginLoadProgress,
            KErrNone,
            plugin );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPluginPoolObserver.
// The callback to indicate plugin loading completion.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::LoadCompletedL( TInt aError )
    {
    LOGSTRING2( "CDevDiagEngine::LoadCompletedL( %d )", aError )

    SetState( EStateReady );

    // Inform the observer of the loading completion.  The observer will
    // display error messages for loading failures.
    if ( iObserver )
        {
        iObserver->HandleEngineCommandL(
            MDevDiagEngineObserver::EDevDiagEngineCommandPluginLoadComplete,
            aError,
            NULL );
        }
    }

// ---------------------------------------------------------------------------
// Get Last results from the Results Database.
// ---------------------------------------------------------------------------
//
void CDevDiagEngine::GetLastResultsL( TUid aParentUid )
    {
    LOGSTRING( "CDevDiagEngine::GetLastResultsL");
    
    RPointerArray<MDiagPlugin> children;
    CleanupClosePushL( children );
    
    MDiagPlugin* plugin;
    ///@@@KSR: changes for BAD Warnings - #177-D: variable "formattedName" was declared but never referenced
    //HBufC* formattedName = NULL;
    
    if ( PluginPool().FindPlugin( aParentUid, plugin ) == KErrNone )
        {        
        MDiagSuitePlugin* suite = static_cast< MDiagSuitePlugin* >( plugin );
        suite->GetChildrenL( children, MDiagSuitePlugin::ESortByPosition );
        }
     else 
        {
        User::Leave(KErrNotFound);
        }
     
    if ( iUids )
        {
        delete iUids;
        iUids = NULL;
        }
        
    iUids = new (ELeave) CArrayFixFlat<TUid>( KArrayGranuality );
    
    iLastResults.ResetAndDestroy();
    
    //Create UID array that is needed when results are searched from the DB.
    for ( TInt i = 0; i < children.Count(); ++i )
        {
        iUids->AppendL( children[i]->Uid() );
        }

    CleanupStack::PopAndDestroy( &children );
    
    //Start search. This is an asynchronous call.
    iResultsDatabase.InitiateGetLastResults( *iUids, iStatus );
    
    SetActive();
    }


// ---------------------------------------------------------------------------
// Get last plug-in that crashed (if any). 
//
// CDiagResultsDatabaseItem::EQueuedToRun indicates that the test was not
// executed properly.
// ---------------------------------------------------------------------------
// 
TBool CDevDiagEngine::CrashedPluginL( TUid& aPluginUid )
    {
    LOGSTRING( "CDevDiagEngine::CrashedPluginL");
    TUid recordUid;
    TInt error = iResultsDatabase.GetLastNotCompletedRecord ( recordUid ); 
    LOGSTRING2("crashedpluginL::dbconnect error %d",error);
    TBool found = EFalse;
    if ( error == KErrNone )
        {                
        RDiagResultsDatabaseRecord crashedRecord;
        
        error = crashedRecord.Connect( iResultsDatabase, recordUid, EFalse );
        CleanupClosePushL( crashedRecord );
        
        if ( error != KErrNone )
            {
            LOGSTRING2( "CDevDiagEngine::CrashedPluginL connect error: %d", error );
            User::Leave( error );
            }
        
        //ResetAndDestroy + Close would be better
        RPointerArray<CDiagResultsDatabaseItem> resultsArray;
        CleanupClosePushL( resultsArray );
        
        error = crashedRecord.GetTestResults ( resultsArray );
        
        if ( error != KErrNone )
            {
            LOGSTRING2( "CDevDiagEngine::CrashedPluginL GetTestResults error: %d", error );
            User::Leave( error );
            }
        
        for ( TInt i = 0; i < resultsArray.Count(); ++i )
            {
            if ( resultsArray[i]->TestResult() == CDiagResultsDatabaseItem::EQueuedToRun )
                {
                aPluginUid = resultsArray[i]->TestUid();                
                found = ETrue;
                break;
                }
            }
        
        resultsArray.ResetAndDestroy();
        CleanupStack::PopAndDestroy();        
        CleanupStack::PopAndDestroy(); //crashedRecord    
        }        

    LOGSTRING( "CDevDiagEngine::CrashedPluginL end" );       
        
    return found;    
    }


// ---------------------------------------------------------------------------
// Completes the crashed test record.
// ---------------------------------------------------------------------------
//    
///@@@KSR: changes for Codescanner error val = High
//TInt CDevDiagEngine::CompleteCrashedTestRecord()
TInt CDevDiagEngine::CompleteCrashedTestRecordL()
    {    
    LOGSTRING( "CDevDiagEngine::CompleteCrashedTestRecordL");
    TUid recordUid;
    TInt error = iResultsDatabase.GetLastNotCompletedRecord ( recordUid );
    
    if ( error == KErrNone )
        {                
        RDiagResultsDatabaseRecord crashedRecord;
        
        error = crashedRecord.Connect( iResultsDatabase, recordUid, EFalse );
        CleanupClosePushL( crashedRecord );
        
        if ( error != KErrNone )
            {
            LOGSTRING2( "CDevDiagEngine::CompleteCrashedTestRecordL connect error: %d", error );
            return error;
            }
            
        error = crashedRecord.TestCompleted( ETrue );
        
        if ( error != KErrNone )
            {
            LOGSTRING2( "CDevDiagEngine::CompleteCrashedTestRecordL TestCompleted error: %d", error );
            return error;
            }
            
        CleanupStack::PopAndDestroy(); //crashedRecord     
        }

    LOGSTRING( "CDevDiagEngine::CompleteCrashedTestRecordL end");
                    
    return error;   
    }
    
 
// ADO & Platformization Changes
 TBool CDevDiagEngine::GetPluginDependencyL()
 {
     ///@@@KSR: changes for Codescanner error val = High 	
     //return iDiagEngine->GetPluginDependency();
     return iDiagEngine->GetPluginDependencyL();
 }

// End of File
