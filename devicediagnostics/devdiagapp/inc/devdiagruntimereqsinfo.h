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
* Description:  .
*
*/


#ifndef DEVDIAGRUNTIMEREQSINFO_H
#define DEVDIAGRUNTIMEREQSINFO_H

// System Include Files
#include <e32base.h>                // CActive
#include <e32def.h>                 // TInt, TUint, TInt64
#include <e32std.h>                 // TTime, TCallBack
#include <etel.h>                   // RTelServer
#include <etel3rdparty.h>           // CTelephony Packages
#include <etelmm.h>                 // RMobilePhone

// Forward Declarations
class RFs;
class RDiagResultsDatabase;

/**
 *  Phone Doctor Application Runtime Requirements Information
 *  This class checks the current status of diagnostics requirements,
 *  which must be satisfied before the Phone Doctor Application may run tests.
 *
 *  @lib euser.lib
 *  @lib centralrepository.lib
 */
class CDevDiagRuntimeReqsInfo : public CActive
    {

public: // Data Types

    /**
     * Error codes indicating the results of the runtime requirements checks.
     */
     enum TResult
        {
        EResultOk = 0,       
        EResultErrLowDisk,
        EResultErrLowBattery,        
        };


public: // New Functions

    /**
     * Two-phased constructor.
     *
     * @param aFs A reference to a connected file system object.
     * @param aDb A reference to a connection results database session.
     * @param aCallback A general callback that indicates when checking the
     *                  runtime requirements has finished.
     * @return A pointer to a new instance of CDevDiagRuntimeReqsInfo.
     */
    static CDevDiagRuntimeReqsInfo* NewL( RFs& aFs,
                                     RDiagResultsDatabase& aDb,
                                     TCallBack aCallback );

    /**
    * Destructor.
    */
    virtual ~CDevDiagRuntimeReqsInfo();

    /**
     * Checks if the diagnostics runtime requirements are satisfied.  This
     * is an asynchronous request which will complete with a callback.
     *
     * @param aResumingTests If ETrue, then runtime checks which do not apply
     *                       to resumed test execution will be skipped.
     */
    void CheckRuntimeRequirementsL( TBool aResumingTests );

    /**
     * Returns the runtime requirements result, which indicates if the runtime
     * requirements were satisfied.
     *
     * @return The runtime requirements result.
     */
    TResult Result() const;

    /**
     * Displays the dialog associated with the result of the runtime checks.
     *
     * @return The command the dialog is dismissed with.
     */
    TInt DisplayDialogL() const;


private: // Data Types

    /**
     * The state of the runtime requirements.  This is used to handle
     * completion of the asynchronous requests.
     */
    enum TState
        {
        EStateInitial = 0,
        EStateCheckingBattery,   
        EStateComplete
        };


private: // New Functions

    /**
     * C++ constructor.
     *
     * @param aFs A reference to a connected file system object.
     * @param aDb A reference to a connection results database session.
     * @param aCallback A general callback that indicates when checking the
     *                  runtime requirements has finished.
     */
    CDevDiagRuntimeReqsInfo( RFs& aFs,
                        RDiagResultsDatabase& aDb,
                        TCallBack aCallback );

    /**
     * Two-phased constructor.
     */
    void ConstructL();

    /**
     * Reads the runtime requirements values from the central repository and
     * stores them to the object.
     */
    void ReadCentralRepositoryValuesL();

   
    /**
     * Checks if there is enough free estimated disk space to log test
     * results.  Updates the member data with the disk space information.
     */
    void CheckDiskSpaceL();

    /**
     * Checks if the phone's battery level is adequate to run tests.  Updates
     * the member data with the battery level information.
     */
    void CheckBatteryLevelL();
    
    /**
     * This is a utility function to handle the common checking for completion
     * of the runtime requirements checks.  If a runtime check failed, this
     * will set the object active so that the runtime checking completes.
     *
     * @return ETrue if checking is done, EFalse if checking should continue.
     */
    TBool CheckCompletion();

    /**
     * Utility function to display a message dialog associated with the
     * specified resource.
     *
     * @param aResource The resource of the message query dialog to display.
     * @return The button pressed to dismiss the dialog.
     */
    static TInt DisplayMessageQueryDialogL( TInt aResource );

    
private: // From base class CActive

    /**
     * From CActive.
     * This function is called when an active request completes.
     */
    virtual void RunL();

    /**
     * From CActive.
     * This function is called to cancel any outstanding asynchronous
     * requests.
     */
    virtual void DoCancel();
    
		/**
     * From CActive.
     * This function is called when RunL leaves
     * 
     */
    virtual TInt RunError( TInt aError );

private: // Data

    /**
     * A reference to a connected file server session, used to check the disk
     * space.
     */
    RFs& iFs;

    /**
     * A reference to a connected results database session, used to get
     * information about the maximum executions.
     */
    RDiagResultsDatabase& iDb;

    /**
     * A general callback that indicates when checking the runtime
     * requirements has finished.
     */
    TCallBack iCallback;
    
     /**
     * A telephony object used to check some of the runtime requirements
     * checks.
     * Own.
     */
    CTelephony* iTelephony;

    /**
     * A battery info object used to satisfy the runtime requirement check for
     * minimum battery level.
     */
    CTelephony::TBatteryInfoV1 iBatteryInfo;

    /**
     * A package object for the battery information.
     */
    CTelephony::TBatteryInfoV1Pckg iBatteryInfoPackage;

    /**
     * Indicates if the runtime requirements are being checked for resumed
     * test execution.  If ETrue, then runtime checks which do not apply to
     * resumed test execution will be skipped.
     */
    TBool iResumingTests;

    /**
     * The runtime requirements status, which will contain error code values
     * to indicate if the runtime requirements are satisfied.
     */
    TResult iResult;

    /**
     * The state, which is used when handling asynchronous requests.
     */
    TState iState;

    /**
     * The current battery level.
     */
    TInt iBatteryLevel;

    /**
     * The minimum required battery level.
     */
    TInt iMinBatteryLevel;

    /**
     * The current amount of free disk space (in bytes).
     */
    TInt64 iDiskSpaceFree;

    /**
     * The minimum required free disk space (in bytes).
     */
    TInt64 iMinDiskSpaceFree;

    
    };

#endif // DEVDIAGRUNTIMEREQSINFO_H
