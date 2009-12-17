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
* Description:  This is the declaration of the execution results class used
*                by the Device Diagnostics Application to present a common interface
*                for results information to the application.
*
*/


#ifndef DEVDIAGEXECUTIONRESULTS_H
#define DEVDIAGEXECUTIONRESULTS_H

// System Include Files
#include <e32base.h>                // CBase
#include <e32cmn.h>                 // RPointerArray, TUid

// Forward Declarations
class MDiagEngineCommon;
class MDiagPlugin;
class MPDResultsObserver;
class CDiagResultsDatabaseItem;
class CDiagPluginPool;
class RDiagResultsDatabase;

/**
 *  Device Diagnostics Execution Results Class.
 *  This class presents a common interface to the application for results from
 *  both live test runs as well as from the results database.
 *
 *  @lib diagresultsdatabase.lib
 *  @lib diagframework.lib
 */
class CDevDiagExecResults : public CBase
    {

public: // Nested Classes

    /**
     *  Device Diagnostics Execution Results Information Class.
     *  This class provides a common interface to individual results
     *  information from either live execution or the results database.
     */
    class CResult : public CBase
        {

    public: // New Functions

        /**
         * Two-phased constructor.
         *
         * @param aPlugin The plugin that this result corresponds to.
         * @return A pointer to the newly-created instance of CResult.
         */
        static CResult* NewL( const MDiagPlugin& aPlugin );

        /**
         * Two-phased constructor.
         *
         * @param aPlugin The plugin that this result corresponds to.
         * @return A pointer to the newly-created instance of CResult.
         */
        static CResult* NewLC( const MDiagPlugin& aPlugin );

        /**
         * Destructor.
         */
        virtual ~CResult();

        /**
         * Returns the pointer to the test result.  This may be NULL if the
         * item is a suite or has not been run yet.  The client must not
         * delete this pointer.
         *
         * @return A pointer to the test result for this item, or NULL.
         */
        const CDiagResultsDatabaseItem* Result() const;

        /**
         * Returns a reference to the plugin that corresponds to this item.
         *
         * @return A reference to the plugin that corresponds to this item.
         */
        const MDiagPlugin& Plugin() const;

        /**
         * Returns the execution status (error value) for this plugin.  This
         * will only contain meaningful data for live test executions.
         *
         * @return The execution status (error value) for this plugin
         */
        TInt ExecStatus() const;

        /**
         * Returns the current execution step for this plugin.  This will only
         * contain meaningful data for live test executions.
         *
         * @return The current execution step for this plugin.
         */
        TUint CurrentStep() const;

        /**
         * Returns the total number of execution steps for this plugin.
         *
         * @return The total number of execution steps for this plugin.
         */
        TUint TotalSteps() const;

        /**
         * Sets the result information for this plugin.  Ownership of the
         * result is transferred.  If the result was previously set, it will
         * be deleted and replaced with the new data.
         *
         * @param aResult The test result.
         * @param aError The execution status error value.
         */
        void SetResult( CDiagResultsDatabaseItem* aResult, TInt aError );

        /**
         * Sets the test progress information.
         *
         * @param aCurrentStep The current execution step for this plugin.
         * @param aCurrentStep The total number of execution steps for this
         *                     plugin.
         */
        void SetProgress( TUint aCurrentStep, TUint aTotalSteps );


    private: // New Functions

        /**
         * Default constructor.
         *
         * @param aPlugin The plugin that this result corresponds to.
         */
        CResult( const MDiagPlugin& aPlugin );

        /**
         * Two-phased constructor.
         */
        void ConstructL();


    private: // Data

        /**
         * The test results information.
         * Own.
         */
        CDiagResultsDatabaseItem* iResult;

        /**
         * A reference to the plugin that this result corresponds to.
         */
        const MDiagPlugin& iPlugin;

        /**
         * The test execution status (error value).
         */
        TInt iExecStatus;

        /**
         * The current step in the execution of this plugin.
         */
        TUint iCurrentStep;

        /**
         * The total number of steps in the execution of this plugin.
         */
        TUint iTotalSteps;

        };


public: // New Functions

    /**
     * Two-phased constructor.  This creates an execution results object by
     * loading test results from the Results Database (logged results).
     *
     * @param aRecordId The unique record identifier from the results database
     *                  for this set of results.
     * @param aPluginPool A reference to the plugin pool, which is used for
     *                    getting plugin information.
     * @param aDbSession A reference to an open session with the results
     *                   database server.
     * @return A pointer to the newly-created instance of CDevDiagExecResults.
     */
    static CDevDiagExecResults* NewL( TUid aRecordId,
                                      CDiagPluginPool& aPluginPool,
                                      RDiagResultsDatabase& aDbSession );

    /**
     * Two-phased constructor.  This creates an execution results object by
     * getting test results from live execution.  The results objects are
     * created from the diagnostics engine's execution plan, and updated as
     * tests complete.
     *
     * @param aRecordId The unique record identifier from the results database
     *                  for this set of results.
     * @param aPluginPool A reference to the plugin pool, which is used for
     *                    getting plugin information.
     * @param aDbSession A reference to an open session with the results
     *                   database server.
     * @param aExecutedUid The uid of the plugin that was executed to produce
     *                     these test results.
     * @param aDiagEngine A pointer to the diagnostics engine, which is only
     *                    used to get the execution plan during live test
     *                    runs.
     * @return A pointer to the newly-created instance of CDevDiagExecResults.
     */
    static CDevDiagExecResults* NewL( TUid aRecordId,
                                      CDiagPluginPool& aPluginPool,
                                      RDiagResultsDatabase& aDbSession,
                                      TUid aExecutedUid,
                                      MDiagEngineCommon* aDiagEngine );

    /**
     * Destructor.
     */
    virtual ~CDevDiagExecResults();

    /**
     * This function is used to set the result for the current execution plan
     * index.  It is only valid for live test executions.  Ownership of the
     * result is transferred.  If called multiple times for the same plugin,
     * each suqsequent call deletes and overwrites the previous result data.
     *
     * @param aResult The test result.
     * @param aStatus The execution status error value.
     */
    void AddEntryL( CDiagResultsDatabaseItem* aResult, TInt aStatus );

    /**
     * Sets the test progress information for the current execution plan
     * index.
     *
     * @param aCurrentStep The current execution step for this plugin.
     * @param aCurrentStep The total number of execution steps for this
     *                     plugin.
     */
    void SetProgressL( TUint aCurrentStep, TUint aTotalSteps );

    /**
     * Retrieves the CResult reference at the specified index.
     *
     * @param aIndex The index of the result to retrieve.
     * @return A reference to the CResult object at the index.
     */
    CResult& operator[] ( TInt aIndex );
    const CResult& operator[] ( TInt aIndex ) const;

    /**
     * Retrieves the CResult reference at the current index.  For live
     * execution runs, this is determined from the test execution plan.  For
     * logged results, this is always the last item.
     *
     * @return A reference to the CResult object.
     */
    CResult& CurrentItemL();
    const CResult& CurrentItemL() const;

    /**
     * Returns the number of result items.
     *
     * @return The number of result items.
     */
    TInt Count() const;

    /**
     * Returns the index of the current result item.  For live execution runs,
     * this is determined from the test execution plan.  For logged results,
     * this is always the last item.
     *
     * @return The index of the current execution item.
     */
    TUint CurrentIndexL() const;

    /**
     * Checks if the diagnostics engine has finished the last test.
     *
     * @return ETrue if the last test has finished and the diagnostics engine
     *         is cleaning up.
     */
    TBool LastTestFinishedL() const;

    /**
     * Returns whether or not the item specified by the argument is (or was)
     * executed as a dependency test.
     *
     * @param aIndex The index of the item to check for dependency execution.
     * @return ETrue if the item is (or was) a dependency, EFalse otherwise.
     */
    TBool IsDependencyL( TInt aIndex ) const;

    /**
     * Returns whether or not the results are from a completed test record.
     *
     * @return ETrue if the results are from a completed test record, EFalse
     *         otherwise.
     */
    TBool IsRecordCompleted() const;

    /**
     * Finalizes live execution results so that they may not be updated.
     *
     * @param aTestingComplete Indicates if all tests in the record were
     *                         executed.
     */
    void Finalize( TBool aTestingComplete );

    /**
     * Returns the total number of steps for all items in the execution
     * results.
     *
     * @return The total number of steps for all items in the execution
     *         results.
     */
    TInt TotalExecutionSteps() const;

    /**
     * Returns the current execution step based on all items in the execution
     * results.
     *
     * @return The current execution step based on all items in the execution
     *         results.
     */
    TInt CurrentExecutionStep() const;

    /**
     * Returns a reference to the plugin that the engine was asked to execute.
     * The engine must be executing plugins for this to be valid.
     *
     * @return A reference to the plugin that the engine was asked to execute.
     */
    const MDiagPlugin& ExecutedPluginL() const;

    /**
     * Returns whether or not the execution is for a single plugin.
     *
     * @return ETrue if the execution is for a single plugin.
     */
    TBool SinglePluginExecutionL() const;

    /**
     * Returns the number of failed tests in this set of results.
     *
     * @return The number of failed tests in this set of results.
     */
    TInt FailedResultsCount() const;
    
    /**
     * Returns the number of passed tests in this set of results.
     *
     * @return The number of passed tests in this set of results.
     */
    TInt PassedResultsCount() const;

    /**
     * Returns whether or not the execution plan's resume index has been
     * reached.  This value indicates if the diagnostics engine is still
     * updating the application with cached test results.
     *
     * @return ETrue if the execution plan's resume index has been reached.
     */
    TBool ResumeIndexReachedL() const;

    /**
     * Returns the UID of the record associated with this set of results.
     *
     * @return The UID of the record associated with this set of results.
     */
    const TUid& RecordId() const;
    
// ADO & Platformization Changes   
    const CDiagResultsDatabaseItem* GetSinglepluginExecutionResult() const;


private: // Data Types

    /**  Enum to indicate what type of results data this object contains. */
    enum TResultsType
        {
        ETypeExecution,
        ETypeLog
        };


private: // New Functions

    /**
     * Default constructor.
     *
     * @param aRecordId The unique record identifier from the results database
     *                  for this set of results.
     * @param aPluginPool A reference to the plugin pool, which is used for
     *                    getting plugin information.
     * @param aExecutedUid The uid of the plugin that was executed to produce
     *                     these test results.  This is ignored for logged
     *                     test results.
     * @param aDiagEngine A pointer to the diagnostics engine, which is only
     *                    used to get the execution plan during live test
     *                    runs.  This must be NULL for logged test results.
     */
    CDevDiagExecResults( TUid aRecordId,
                         CDiagPluginPool& aPluginPool,
                         RDiagResultsDatabase& aDbSession,
                         TUid aExecutedUid = KNullUid,
                         MDiagEngineCommon* aDiagEngine = NULL );

    /**
     * Two-phased constructor.
     */
    void ConstructL();


private: // Data

    /**
     * The type of results data contained by this object.
     */
    TResultsType iType;

    /**
     * The array of results data.
     */
    RPointerArray< CResult > iResults;

    /**
     * The results database UID that corresponds to this test record.
     */
    TUid iRecordId;

    /**
     * The plugin pool, used to get information about test plugins.
     */
    CDiagPluginPool& iPluginPool;

    /**
     * A reference to an open database session.  This object does not close
     * the database session; it may only use it and create subsessions.
     */
    RDiagResultsDatabase& iDbSession;

    /**
     * A pointer to the diagnostics engine, used only to get the execution
     * plan for live test exections.
     * Not own.
     */
    MDiagEngineCommon* iDiagEngine;

    /**
     * The total number of test steps in the entire execution.
     */
    TInt iTotalSteps;

    /**
     * The total number of test steps in the entire execution.
     */
    TInt iCompletedSteps;

    /**
     * The test step that the currently-executing plugin is on.  This is not
     * combined with "iCompletedSteps" because plugins may be suspended and
     * restarted before they finally complete.
     */
    TInt iCurrentPluginStep;

    /**
     * The UID of the plugin executed to produce these results.  This is
     * obtained from the test record during construction.
     */
    TUid iExecutedPlugin;

    /**
     * The status of the record that corresponds to these test results.  A
     * record is fully completed only when all tests in it have been executed,
     * or the user decides not to resume it after suspending it.  This data is
     * obtained from the Finalize() function for live executions and from the
     * database for logged results.
     */
    TBool iCompleted;

    /**
     * The number of failed tests in these test results.  This data is
     * obtained from the AddEntryL() function for live executions and from the
     * database for logged results.
     */
    TInt iFailedCount;
    
    /**
      * The number of passed tests in these test results.  This data is
      * obtained from the AddEntryL() function for live executions and from the
      * database for logged results.
      */
     TInt iPassedCount;

    /**
     * The index value of the last test plugin in the results array.  This is
     * used to check if the last test has finished.
     */
    TInt iLastTestIndex;

    };

#endif // PDEXECUTIONRESULTS_H
