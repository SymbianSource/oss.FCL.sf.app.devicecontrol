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
* Description:  Handles most of the model related logic..
*
*/


#ifndef DEVDIAGDENGINE_H
#define DEVDIAGDENGINE_H

// System Include Files
#include <e32base.h>                // CActive
#include <etel3rdparty.h>           // CTelephony
#include <DiagResultsDatabase.h>    // RDiagResultsDatabase,
                                    // RDiagResultsDatabaseRecord
#include <DiagEngineObserver.h>     // MDiagEngineObserver
#include <DiagPluginPoolObserver.h> // MDiagPluginPoolObserver
#include <DiagCommonDialog.h>       // TDiagCommonDialog

// Forward Declarations
class MDevDiagEngineObserver;
class MDiagPlugin;
class MDiagSuitePlugin;
class CDevDiagExecResults;
class CDiagResultsDatabaseItem;
class CDiagEngine;
class RFs;
class CDiagPluginPool;
class CAknViewAppUi;
class CAknDialog;
class CIdle;

/**
 *  Device Diagnostics Application Engine
 *  This class defines the engine used by the Device Diagnostics application.  The
 *  engine class handles the interfacing between UI components and diagnostics
 *  components.
 *
 *  @lib euser.lib
 *  @lib diagframework.lib
 *  @lib etel3rdparty.lib
 *  @lib diagresultsdatabase.lib
 *  @lib avkon.lib
 *  @lib eikcore.lib
 *  @lib eiksrv.lib
 *  @lib efsrv.lib
 *  @lib drmserviceapi.lib
 */
class CDevDiagEngine : public CActive,
                       public MDiagEngineObserver,
                       public MDiagPluginPoolObserver
    {

public: // Data Types

    /**  This enum indicates how the engine should stop tests. */
    enum TDevDiagAppEngineStopMode
        {
        EStopModeSkip,
        EStopModeCancel,
        EStopModeSuspend,
        EStopModeWatchdog
        };

    enum TDevDiagAppEngineResumeMode
        {
        EResumeModeResume,
        EResumeModeWatchdog
        };


public: // New Functions

    /**
     * Two-phased constructor.
     */
    static CDevDiagEngine* NewL();

    /**
     * Two-phased constructor.
     * Leaves the new engine instance on the cleanup stack.
     */
    static CDevDiagEngine* NewLC();

    /**
    * Destructor.
    */
    virtual ~CDevDiagEngine();

    /**
     * Sets the engine observer, for receiving engine events.
     *
     * @param aObserver A pointer to a class implementing the observer
     *                  interface.  Ownership is not transferred.
     */
    void SetObserver( MDevDiagEngineObserver* aObserver );

    /**
     * Run a test or suite of tests.
     *
     * @param aUid The UID of the test or suite to execute.
     * @param aAppUi The application UI which is executing tests.
     */
    void ExecuteTestL( TUid aUid, CAknViewAppUi& aAppUi );

    /**
     * Gets the results from test execution.  Results may be from a log or
     * from live test execution.  The caller should check if results are
     * available before calling this function.
     *
     * @return A reference to the execution results.
     */
    const CDevDiagExecResults& ExecutionResults() const;

    /**
     * Instructs the engine to populate the execution results information with
     * the last set of logged data.  This may be followed by a call to
     * ExecutionResults to retrieve the data.
     */
    void LoadLastLoggedResultsL();

    /**
    * Get results of the suite. Observer will be notified when
    * results are ready (EDevDiagEngineCommandGetLastResult).
    * 
    * @param aParentUid UID of the plug-in whose results are needed.
    */
    void GetLastResultsL( TUid aParentUid );

    /**
     * Suspends, skips, or halts test execution.
     *
     * @param aReason The reason for stopping execution.
     */
    void ExecutionStopL( TDevDiagAppEngineStopMode aReason );

    /**
     * Resumes the diagnostics engine.  In the case of resuming suspended test
     * execution, this must have been preceded by a call to ExecutionStopL
     * with EStopModeSuspend specified.
     *
     * @param aReason The reason for resuming execution.
     */
    void ExecutionResumeL( TDevDiagAppEngineResumeMode aReason );

    /**
     * Gets the plugin pool, which is owned by the engine.
     *
     * @return A reference to the plugin pool.
     */
    const CDiagPluginPool& PluginPool() const;

    /**
     * Checks if the engine is currently running plugins.  This includes
     * when the engine is suspended for any reason.
     *
     * @return ETrue if plugins are running, EFalse otherwise.
     */
    TBool IsRunningPlugins() const;

    /**
     * Checks if the engine is currently stopping execution.
     *
     * @return ETrue if execution is being cancelled, EFalse otherwise.
     */
    TBool IsStoppingExecution() const;

    /**
     * Checks if the engine is initialzed (it has finished loading plugins).
     * This must be true before the runtime requirements can be checked or
     * the reference to the plugin pool can be returned.
     *
     * @return ETrue if plugins are loaded, EFalse otherwise.
     */
    TBool ArePluginsLoaded() const;

    /**
     * Checks if the engine has execution results available.  This must be
     * true before a reference to the results can be retrieved.
     *
     * @return ETrue if results are available, EFalse otherwise.
     */
    TBool HasExecutionResults() const;
    
    /**
     * Searches Results Database for a crashed test plug-in.
     * 
     * @param aPluginUid Plug-in Uid that crashed.
     * @return ETrue if crashed plug-in was found, EFalse otherwise.
     **/
    TBool CrashedPluginL( TUid& aPluginUid );

    /**
     * Marks the crashed test record as complete in the database.  This
     * means that the test record is not found again when calling CrashedTestRecordL.
     *
     * @return KErrNone if the record was successfully completed, otherwise, a
     *         system error code.
     */
    //TInt CompleteCrashedTestRecord();
    ///@@@KSR: changes for Codescanner error val = High
    TInt CompleteCrashedTestRecordL();
    ///@@@KSR: changes for Codescanner error val = High
    // ADO & Platformization Changes
    TBool GetPluginDependencyL();
    // GetState
    TInt GetState();

private: // Data Types

    /**  This enum indicates the engine's internal state.
     *   Any changes to this enum must be reflected with changes to the
     *   SetState function.
     */
    enum TDevDiagAppEngineState
        {
        EStateInitial = 0,
        EStateLoadingPlugins,
        EStateReady,
        EStateStartingExecution,
        EStateRunningTests,
        EStateStoppingExecution,
        EStateExecutionSuspended,
        EStateMax
        };


private: // New Functions

    /**
     * The default constructor.
     */
    CDevDiagEngine();

    /**
     * Two-phased constructor.
     */
    void ConstructL();

    /**
     * Handles state transitions in the application engine by validating the
     * requested transition.
     *
     * @param aNextState The new state to transition to.
     */
    void SetState( TDevDiagAppEngineState aNextState );

    /**
     * The callback function to continue handling cancelling execution.
     *
     * @param aPtr A pointer to "this" object.
     * @return Always KErrNone.
     */
    static TInt HandleExecutionCancelledL( TAny* aPtr );


private: // From base class CActive

    /**
     * From CActive.
     * This function is called when an active request completes.  For the
     * application engine, this only happens when checking runtime
     * requirements.
     */
    virtual void RunL();

    /**
     * From CActive.
     * This function is called to cancel any outstanding asynchronous
     * requests.
     */
    virtual void DoCancel();


private: // From base class MDiagEngineObserver

    /**
     * From MDiagEngineObserver.
     * This callback is used when test execution starts.
     *
     */
    virtual void TestExecutionBeginL();
    
    /**
    * Notify client of execution completion of one plug-in.
    *   This can be either test plug-in or suite plug-in.
    *
    * @param aError  - KErrNone - Success.
    *   KErrNoMemory - Out of memory.
    *   KErrCancel   - Cancelled due to ExecutionStopL()
    *
    * @param aResult - Result of the test. Ownership is transferred here. 
    *  Client must deallocate aResult to avoid memory leak. aResult is Null 
    *  if the plug-in being executed is a suite pre/post method.
    */
    virtual void TestExecutionPluginExecutedL( TInt aError,
                                               CDiagResultsDatabaseItem* aResult );

    /**
     * From MDiagEngineObserver.
     * This callback is used to inform the application of the execution
     * progress for the currently executing test.
     *
     * @param aCurrentItemStep The current step for this test.
     * @param aCurrentItemTotalSteps The total number of steps for this test.
     */
    virtual void TestExecutionProgressL( TUint aCurrentItemStep,
                                         TUint aCurrentItemTotalSteps);


    /**
    * From MDiagEngineObserver.
    * Notify client of engine stopping.
    *
    * This indicates that engine execution has stopped. There will not be any
    * further messages from engine. This could be called because all
    * tests are completed, or an unrecoverable error occured during execution.
    *
    * Note that if cancel is called during plan creation or if plan creation
    * fails, TestExecutionStoppedL() may be called withing first calling
    * TestExecutionBeginL(). 
    *
    * @param aError - Reason for engine stopping. 
    *   a) KErrNone - All tests are successfully completed.
    *   b) KErrCancel - ExecutionStopL is called with ECancelAll. 
    *       Test session cannot be resumed later.
    *   c) KErrArgument - Parameters passed to engine are invalid.
    *   d) Others - Other critical that could not be recovered occured during
    *       test execution.  Test may be resumed later in this case.
    */
    virtual void TestExecutionCompletedL( TInt aError );

    /**
     * From MDiagEngineObserver.
     * Notifes that execution has been suspended.
     *
     * @param aSuspendReason Why execution is suspended.
     */
    virtual void TestExecutionSuspendedL( TSuspendReason aSuspendReason );

    /**
     * From MDiagEngineObserver.
     * Notifes that execution has been resumed.
     *
     * @param aResumeReason Why execution is resumed.
     */
    virtual void TestExecutionResumedL( TResumeReason aResumeReason );

    /**
     * From MDiagEngineObserver.
     * Create a common dialog.
     * @param aDialogType Type of dialog to create.
     * @param aData Initialization parameter. Ownership is transferred.
     * @return A pointer to newly created dialog. Ownership is passed to
     *         caller.
    */
    virtual CAknDialog* CreateCommonDialogLC( TDiagCommonDialog aDialogType,
                                              TAny* aInitData );
                                              
    /**
    * From MDiagEngineObserver.
    *
    * Execute a command from plug-in. Parameters are identical to 
    * MDiagEngineCommon::ExecuteAppCommandL(). 
    * @see MDiagEngineCommon::ExecuteAppCommandL()
    * @see TDiagAppCommand
    */
    virtual void ExecuteAppCommandL( TDiagAppCommand aCommand, 
                                     TAny* aParam1,
                                     TAny* aParam2 );                                              


private: // From base class MDiagPluginPoolObserver

       /**
     * From MDiagPluginPoolObserver.
     * This callback is used to inform the application about loading progress
     * of plugins.
     *
     * @param aCurrentPlugin The current plugin index in the loading progress.
     * @param aPluginCount The total number of plugins to load.
     @ @param aLoadedPluginUid The UID of the plugin that was just loaded.
     */
    virtual void LoadProgressL ( TUint aCurrentPlugin,
                                 TUint aPluginCount,
                                 const TUid& aLoadedPluginUid );


    /**
    * Notify client of plug-in loading completion
    *
    * @param aError     KErrNone - Success
    *                   KErrCorrupt  - One ore more plugin could not
    *                   be loaded. 
    *                   KErrNoMemory - Not enough memory.
    */
    virtual void LoadCompletedL( TInt aError );


private: // Data

    /**
     * The application engine's current state.
     */
    TDevDiagAppEngineState iState;

    /**
     * The test execution results.
     * Own.
     */
    CDevDiagExecResults* iResults;

    /**
     * The current observer of the application engine.
     * Not own.
     */
    MDevDiagEngineObserver* iObserver;

    /**
     * The diagnostics engine.
     * Own.
     */
    CDiagEngine* iDiagEngine;

    /**
     * The session with the results database server.
     */
    RDiagResultsDatabase iResultsDatabase;

    
    /**
     * The plugin pool, which loads and owns plugins.
     * Own.
     */
    CDiagPluginPool* iPluginPool;


    /**
     * Holds the number of times suspend has been called.  In nested suspend /
     * resume scenarios, this is used to ensure we don't resume until we have
     * matched the number of suspends.
     */
    TInt iSuspendCounter;
    
     /**
     * A utility active object that is used to continue handling cancelling
     * execution, because we cannot use synchronous cancelling due to the
     * possibility of test plugins displaying the common "Cancel Execution"
     * dialog.
     * Own.
     */
    CIdle* iIdle;
    
    /**
    * Get Last results UIDs. Own.
    **/
    CArrayFixFlat<TUid>* iUids;
    
    /**
    * Buffered last results. Own.
    **/
    RPointerArray<CDiagResultsDatabaseItem> iLastResults;

    };

#endif // DEVDIAGDENGINE_H
