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
* Description:  This is the declaration of the Browser Test Plugin Model.
*
*/


#ifndef DIAGBROWSERPLUGINMODEL_H
#define DIAGBROWSERPLUGINMODEL_H

// System Include Files
#include <e32base.h>                        // CActive
#include <commdbconnpref.h>                 // TCommDbConnPref
#include <es_sock.h>                        // RSocketServ, RConnection
#include <http/rhttpsession.h>              // RHTTPSession
#include <http/rhttptransaction.h>          // RHTTPTransaction
#include <http/mhttptransactioncallback.h>  // MHTTPTransactionCallback
#include <DiagResultsDatabaseItem.h>        // CDiagResultsDatabaseItem
#include <diaglogeventeraserobserver.h>     // MDiagLogEventEraserObserver
#include <AknWaitDialog.h> 
// Forward Declarations
class MDiagBrowserPluginModelObserver;
class CPeriodic;
class CDiagLogEventEraser;
class RCmConnectionMethod;

// Local Constants
const TInt KMaxStringLength = ( 512 );

/**
 *  Diagnostics Browser Test Plugin Model.
 *  This class defines the model for the browser test plugin.  This class is
 *  used to validate end-to-end browser (HTTP) functionality.
 *
 *  @lib euser.lib
 *  @lib commonengine.lib
 *  @lib centralrepository.lib
 *  @lib cmmanager.lib
 *  @lib commdb.lib
 *  @lib inetprotutil.lib
 *  @lib http.lib
 *  @lib esock.lib
 *  @lib flogger.lib
 *  @lib logcli.lib
 *  @lib diagframework.lib
 */
class CDiagBrowserPluginModel : public CActive,
                                public MHTTPTransactionCallback,
                                public MDiagLogEventEraserObserver,
                                public MProgressDialogCallback
    {

public: // New functions

    /**
    * Two-phased constructor.
    *
    * @param aObserver The model observer to report events to.
    * @return An instance of CDiagBrowserPluginModel.
    */
    static CDiagBrowserPluginModel* NewL(
        MDiagBrowserPluginModelObserver& aObserver );

    /**
    * Destructor.
    */
    ~CDiagBrowserPluginModel();

    /**
    * Initiates the browser test.
    */
    void RunBrowserTestL();

    /**
    * Returns the total number of execution steps.
    *
    * @return The total number of execution steps.
    */
    static TUint TotalSteps();


private: // Data types

    /**
    * Enumerates the states in the browser test.  These are also used to
    * report test progress information.
    */
    enum TDiagBrowserState
        {
        EDiagBrowserStateInitial = 0,
        EDiagBrowserStateConnectingBearer,
        EDiagBrowserStateConnectingServer,
        EDiagBrowserStateErasingLog,
        EDiagBrowserStateMax
        };

    /**
    * Holds information about the connection methods used in the test.
    */
    struct TBrowserConnectionMethodData
        {
        /** The access point of this connection method. */
        TBuf< KMaxStringLength > iApn;

        /** The name of this connection method. */
        TBuf< KMaxStringLength > iMethodName;

        /** The access point id of this connection method. */
        TUint iApId;

        /** Indicates if this connection method contains valid data. */
        TBool iIsValid;

        /** Indicates if this connection method was attempted. */
        TBool iUsed;
        };


private: // New functions

    /**
    * The default constructor.
    */
    CDiagBrowserPluginModel( MDiagBrowserPluginModelObserver& aObserver );

    /**
    * Two-phased constructor.
    */
    void ConstructL();

    /**
    * Reads and stores the browser test's Central Repository Keys.
    *
    * @return ETrue if the operation succeeded, EFalse otherwise.
    */
    TBool ReadCenRepKeysL();

    /**
    * Finds the connection method that corresponds to the data supplied in the
    * argument.  Updates the argument object with the connection method data.
    *
    * @param aData The connection method data to search with and update.
    */
    void GetConnectionMethodL( TBrowserConnectionMethodData& aData );

    /**
    * Checks if the connection method matches the data supplied in the
    * argument.  Updates the argument object with the connection method data.
    *
    * @param aMethod The connection method to compare with.
    * @param aData The connection method data to search with and update.
    */
    void CheckConnectionMatchL( const RCmConnectionMethod& aMethod,
                                TBrowserConnectionMethodData& aData ) const;

    /**
    * Starts or resets the connection timeout timer.
    */
    void StartConnectionTimerL();

    /**
    * Updates the plugin's state and reports progress to the observer.
    *
    * @param aTargetState The new state to transition to.
    */
    void SetStateL( TDiagBrowserState aTargetState );

    /**
    * Callback function for handling timer events.
    *
    * @param aPtr A pointer to the instance of the browser test model.
    * @return An error code for the CPeriodic framework.
    */
    static TInt TimerExpired( TAny* aPtr );

    /**
    * Opens the network connection.  May be called multiple times if a retry
    * is necessary.
    *
    * @return ETrue if the operation succeeded, EFalse otherwise.
    */
    TBool OpenAndStartConnectionL();

    /**
    * Handles ending the test by deleting the logs, if necessary, and
    * informing the observer.
    *
    * @param aResult The test result to complete with.
    */
    void HandleTestEndL( CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Retries the connection on the secondary access point, if available, or
    * fails the test.
    */
    void RetryConnectionL();
    
    /**
    *	Wait Dialog
    *
    */
    void ShowProgressNoteL();
    void TimerCleanUp();

private: // from MProgressDialogCallback
        
    /**
    * Called when the progress dialog is dismissed.
    *
    * @param aButtonId - indicate which button made dialog to dismiss
    */
    virtual void DialogDismissedL( TInt aButtonId );

private: // From base class CActive

    /**
    * From CActive.
    * Handles active object completion events.
    */
    virtual void RunL();

    /**
    * From CActive.
    * Handles leaves in the active object's RunL function.
    */
    virtual TInt RunError( TInt aError );

    /**
    * From CActive.
    * Handle active object cancel event.
    */
    virtual void DoCancel();


private: // From base class MHTTPTransactionCallback

    /**
    * From MHTTPTransactionCallback.
    * Called when an event occurs on an HTTP transaction.
    *
    * @param aTransaction The transaction the event occured on.
    * @param aEvent The event that occurred.
    */
    virtual void MHFRunL( RHTTPTransaction aTransaction,
                          const THTTPEvent& aEvent );

    /**
    * From MHTTPTransactionCallback.
    * Called when an error occurs on an HTTP transaction.
    *
    * @param aError The error code.
    * @param aTransaction The transaction the error occured on.
    * @param aEvent The event that occurred.
    * @return An error code indicating if this error was handled.
    */
    virtual TInt MHFRunError( TInt aError,
                              RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent );


private: // From base class MDiagLogEventEraserObserver

    /**
    * This function is called to check if the log event needs to be deleted.
    *
    * @param aEvent The event to check.
    * @return ETrue if the log event should be deleted, EFalse otherwise.
    */
    virtual TBool IsEventToBeDeleted( const CLogEvent& aEvent );
    
    /**
    * This function is called to notify that the erase operation is complete.
    *
    * @param aError Error code to indicate erase failure, or KErrNone.
    */           
    virtual void CompleteEventEraseL( TInt aError );


private: // Data

    /** The socket server session used by the test. */
    RSocketServ iSocketServ;

    /** The network connection used by the test. */
    RConnection iConnection;

    /** The HTTP session used by the test. */
    RHTTPSession iHttpSession;

    /** The HTTP transaction used by the test. */
    RHTTPTransaction iHttpTransaction;

    /** The connection preferences, used to establish the data connection. */
    TCommDbConnPref iConnectionPrefs;

    /** The timeout timer.  Own. */
    CPeriodic* iTimer;

    /** The log eraser, used to remove the data usage from the OS log. Own. */
    CDiagLogEventEraser* iLogEraser;

    /** The state of the test. */
    TDiagBrowserState iState;

    /** The time testing started.  Used to filter logs for deletion. */
    TTime iStartTime;

    /** The primary connection method's data. */
    TBrowserConnectionMethodData iPrimaryConnection;

    /** The secondary connection method's data. */
    TBrowserConnectionMethodData iSecondaryConnection;

    /** The address of the proxy server to use (from CenRep). */
    TBuf< KMaxStringLength > iProxyAddress;

    /** The port of the proxy server to use (from CenRep). */
    TInt iProxyPort;

    /** The timeout value to use (from CenRep). */
    TInt iTimeoutValue;

    /** The web page to retrieve (from CenRep). */
    TBuf< KMaxStringLength > iUri;

    /** The test result to return. Used to hold the result while deleting logs
    * asynchronously.
    */
    CDiagResultsDatabaseItem::TResult iResult;

    /** The model's observer, where events are reported. */
    MDiagBrowserPluginModelObserver& iObserver;
    // Retrycount
    TInt iRetryCount,iTimerCalled;
    
    /**
    *
    * Wait Dialog
    *
    */
    
    CAknWaitDialog*      iWaitDialog;
    TBool idialogOn,idialogDismissed;
    };

#endif // DIAGBROWSERPLUGINMODEL_H
