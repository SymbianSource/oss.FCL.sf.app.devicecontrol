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
* Description:  This is the model class for the Browser Test Plugin, which
*                tests HTTP functionality.
*
*/


// System Include Files
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <centralrepository.h>              // CRepository
#include <cmmanager.h>                      // RCmManager
#include <cmdestination.h>                  // RCmDestination
#include <cmconnectionmethod.h>             // RCmConnectionMethod
#include <cmpluginpacketdatadef.h>          // Connection Manager Defs
#include <cmconnectionmethoddef.h>          // Connection Manager Defs
#include <es_sock.h>                        // RSocketServ, RConnection
#include <stringpool.h>                     // RStringPool
#include <httpstringconstants.h>            // HTTP Strings
#include <http/rhttpconnectioninfo.h>       // RHTTPConnectionInfo
#include <http/rhttpsession.h>              // RHTTPSession
#include <http/rhttptransaction.h>          // RHTTPTransaction
#include <http/thttpevent.h>                // THTTPEvent
#include <uri8.h>                           // TUriParser8
#include <logwrap.h>                        // CLogEvent
#include <logcli.h>                         // CLogFilter
#include <DiagResultsDatabaseItem.h>        // CDiagResultsDatabaseItem
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <diaglogeventeraser.h>             // CDiagLogEventEraser
#include <commdb.h>
// User Include Files
#include "diagbrowserpluginmodel.h"         // CDiagBrowserPluginModel
#include "diagbrowserpluginmodelobserver.h" // CDiagBrowserPluginModelObserver
#include "diagbrowserpluginprivatecrkeys.h" // CenRep keys
#include "diagbrowserplugin.pan"            // Panic
#include "diagbrowserplugin.hrh"

#include <devdiagbrowserpluginrsc.rsg>  
// Local Constants
const TInt KConnArrayGranularity =          ( 3 );
const TInt KMicroSecondsInMilliseconds =    ( 1000 );
const TInt KHttpResponseCodeOk =            ( 200 );
#ifdef __WINS__
const TInt KEmulatorDynamicIpApId =         ( 11 );
#endif // __WINS__


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
CDiagBrowserPluginModel* CDiagBrowserPluginModel::NewL(
    MDiagBrowserPluginModelObserver& aObserver )
    {
    LOGSTRING( "CDiagBrowserPluginModel::NewL()" )

    CDiagBrowserPluginModel* self = new( ELeave ) CDiagBrowserPluginModel(
        aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDiagBrowserPluginModel::~CDiagBrowserPluginModel()
    {
    LOGSTRING( "CDiagBrowserPluginModel::~CDiagBrowserPluginModel()" )

    // Stop any outstanding requests.
    Cancel();

    // Free memory reserved by the model.
    delete iTimer;
    delete iLogEraser;
	delete iWaitDialog;
    // Close any open sessions.
    iHttpTransaction.Close();
    iHttpSession.Close();
    iConnection.Close();
    iSocketServ.Close();
    }

// ---------------------------------------------------------------------------
// Initiates the browser test.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::RunBrowserTestL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::RunBrowserTestL()" )

    // Mark the start time for removing the logs.
    iStartTime.UniversalTime();
    iRetryCount = 0;
    iTimerCalled = 0;
    // Read Central Repository keys.
    if ( !ReadCenRepKeysL() )
        {
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        return;
        }

    /* Read APN data.
    GetConnectionMethodL( iPrimaryConnection );
    GetConnectionMethodL( iSecondaryConnection );*/
    //iTimeoutValue = 30000;
    // Open the connection.
    if ( !OpenAndStartConnectionL() )
        {
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        return;
        }
    }

// ---------------------------------------------------------------------------
// Returns the total number of execution steps.
// ---------------------------------------------------------------------------
TUint CDiagBrowserPluginModel::TotalSteps()
    {
    LOGSTRING( "CDiagBrowserPluginModel::TotalSteps()" )
    return EDiagBrowserStateMax;
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagBrowserPluginModel::CDiagBrowserPluginModel(
    MDiagBrowserPluginModelObserver& aObserver )
:   CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    LOGSTRING( "CDiagBrowserPluginModel::CDiagBrowserPluginModel()" )
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::ConstructL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::ConstructL()" )

    // Open the connection to the Socket Server.
    User::LeaveIfError( iSocketServ.Connect() );
    User::LeaveIfError( iConnection.Open( iSocketServ ) );
    idialogOn = EFalse; idialogDismissed = EFalse;
    }

// ---------------------------------------------------------------------------
// Reads and stores the browser test's Central Repository Keys.
// ---------------------------------------------------------------------------
//
TBool CDiagBrowserPluginModel::ReadCenRepKeysL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::ReadCenRepKeysL()" )

    // Create a Central Repository object.
    CRepository* repository = CRepository::NewLC(
        KCRUidDiagBrowserTestPlugin );

    // Get the data.
   /* if ( repository->Get( KDiagBrowserApnPrimary,
                          iPrimaryConnection.iApn ) != KErrNone ||
         repository->Get( KDiagBrowserApnSecondary,
                          iSecondaryConnection.iApn ) != KErrNone ||
         repository->Get( KDiagBrowserProxyAddress,
                          iProxyAddress ) != KErrNone ||
         repository->Get( KDiagBrowserProxyPort,
                          iProxyPort ) != KErrNone ||*/
         if( repository->Get( KDiagBrowserResponseTimeout,
                          iTimeoutValue ) != KErrNone ||
        repository->Get( KDiagBrowserUri,
                          iUri ) != KErrNone )
        {
        LOGSTRING( "CDiagBrowserPluginModel::ReadCenRepKeysL() - CenRep read"
                   L" failed." )

        CleanupStack::PopAndDestroy( repository );
        return EFalse;
        }

   /* LOGSTRING3(
        "CDiagBrowserPluginModel::ReadCenRepKeysL() - Primary APN '%S'"
        L" Secondary APN '%S'",
        &iPrimaryConnection.iApn,
        &iSecondaryConnection.iApn )

    LOGSTRING3(
        "CDiagBrowserPluginModel::ReadCenRepKeysL() - Proxy '%S:%d'",
        &iProxyAddress,
        iProxyPort )*/

    LOGSTRING3(
        "CDiagBrowserPluginModel::ReadCenRepKeysL() - Timeout %d, Uri '%S'",
        iTimeoutValue,
        &iUri )

    CleanupStack::PopAndDestroy( repository );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Finds the connection method that corresponds to the data supplied in the
// argument.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::GetConnectionMethodL(
    TBrowserConnectionMethodData& aData )
    {
    LOGSTRING( "CDiagBrowserPluginModel::GetConnectionMethodL()" )

    // Look for matching connection methods.
    RCmManager connManager;
    connManager.OpenLC();

    // Get a list of all the destinations.
    RArray< TUint32 > connArray = RArray< TUint32 >( KConnArrayGranularity );
    CleanupClosePushL( connArray );
    connManager.AllDestinationsL( connArray );
    
    // Look through all the destinations' connection methods and see if any of
    // them match the CenRep data.
    for ( TInt i = 0; i < connArray.Count() && !aData.iIsValid ; i++ )
        {
        RCmDestination destination = connManager.DestinationL(
            connArray[ i ] );
        CleanupClosePushL( destination );

        LOGSTRING4( "CDiagBrowserPluginModel::GetConnectionMethodL() -"
                    L" Destination %d / %d, %d method(s)",
                    i + 1,
                    connArray.Count(),
                    destination.ConnectionMethodCount() )

        // Don't use any hidden destinations, because they are not visible to
        // the user.
        if ( destination.IsHidden() )
            {
            LOGSTRING( "CDiagBrowserPluginModel::GetConnectionMethodL() -"
                       L" Skipping hidden destination" )
            CleanupStack::PopAndDestroy(); // destination
            continue;
            }
        for ( TInt j = 0;
              j < destination.ConnectionMethodCount() && !aData.iIsValid;
              j++ )
            {
            RCmConnectionMethod method = destination.ConnectionMethodL( j );
            CleanupClosePushL( method );
            
            // This call is trapped because not all connection methods support
            // the data we're querying, and the leave should not be propagated
            // up because that would not be a reason for test failure.
            TRAP_IGNORE( CheckConnectionMatchL( method, aData ) )
            CleanupStack::PopAndDestroy(); // method
            }

        CleanupStack::PopAndDestroy(); // destination
        }

    // Get a list of all the uncategorized connection methods.
    connArray.Reset();
    connManager.ConnectionMethodL( connArray );
    LOGSTRING2( "CDiagBrowserPluginModel::GetConnectionMethodL() -"
                L" Checking %d uncategorized method(s)",
                connArray.Count() )

    // Look through all the uncategorized connection methods and see if any
    // of them match match the CenRep data.
    for ( TInt i = 0; i < connArray.Count() && !aData.iIsValid; i++ )
        {
        RCmConnectionMethod method = connManager.ConnectionMethodL(
            connArray[ i ] );
        CleanupClosePushL( method );

        // This call is trapped because not all connection methods support
        // the data we're querying, and the leave should not be propagated
        // up because that would not be a reason for test failure.
        TRAP_IGNORE( CheckConnectionMatchL( method, aData ) )
        CleanupStack::PopAndDestroy(); // method
        }

    CleanupStack::PopAndDestroy(); // connArray
    CleanupStack::PopAndDestroy(); // connManager
    }

// ---------------------------------------------------------------------------
// Checks if the connection method matches the data supplied in the argument.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::CheckConnectionMatchL(
    const RCmConnectionMethod& aMethod,
    TBrowserConnectionMethodData& aData ) const
    {
    LOGSTRING( "CDiagBrowserPluginModel::CheckConnectionMatchL()" )

    HBufC* name = aMethod.GetStringAttributeL(
        CMManager::ECmName );
    CleanupStack::PushL( name );

#ifdef __WINS__
    HBufC* apn = HBufC::NewL( 1 );
#else
    HBufC* apn = aMethod.GetStringAttributeL(
        CMManager::EPacketDataAPName );
#endif // __WINS__
    CleanupStack::PushL( apn );

    TInt type = aMethod.GetIntAttributeL(
        CMManager::ECmBearerType );

    TBool hidden = aMethod.GetBoolAttributeL(
        CMManager::ECmHidden );

    HBufC* proxy = aMethod.GetStringAttributeL(
        CMManager::ECmProxyServerName );
    CleanupStack::PushL( proxy );

    TInt port = aMethod.GetIntAttributeL(
        CMManager::ECmProxyPortNumber );

    TUint apId = aMethod.GetIntAttributeL(
        CMManager::ECmIapId );

    LOGSTRING4(
        "CDiagBrowserPluginModel::CheckConnectionMatchL() - CM"
        L" Name '%S', Type '0x%08x', Hidden '%d'", name, type, hidden )

    LOGSTRING5( "CDiagBrowserPluginModel::CheckConnectionMatchL() - CM"
                L" APN '%S', Proxy '%S:%d', Id %d", apn, proxy, port, apId )

#ifdef __WINS__
    if ( apId == KEmulatorDynamicIpApId )
#else
    if ( apn->Compare( aData.iApn ) == 0 &&
         proxy->Compare( iProxyAddress ) == 0 &&
         port == iProxyPort &&
         type == KUidPacketDataBearerType &&
         !hidden )
#endif // __WINS__
        {
        aData.iApId = apId;
        aData.iMethodName.Copy( *name );
        aData.iIsValid = ETrue;

        LOGSTRING2( "CDiagBrowserPluginModel::CheckConnectionMatchL() - Found"
                    L" AP with id %d.", aData.iApId )
        }

    CleanupStack::PopAndDestroy( proxy );
    CleanupStack::PopAndDestroy( apn );
    CleanupStack::PopAndDestroy( name );
    }

// ---------------------------------------------------------------------------
// Starts or resets the connection timeout timer.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::StartConnectionTimerL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::StartConnectionTimerL()" )

    // Create the timer, or cancel an existing timer.
    if ( !iTimer )
        {
        iTimer = CPeriodic::NewL( EPriorityHigh );
        }
    else
        {
        iTimer->Cancel();
        }

    // Start the timer.
    iTimer->Start(
        TTimeIntervalMicroSeconds32(
            iTimeoutValue * KMicroSecondsInMilliseconds ),
        TTimeIntervalMicroSeconds32( 0 ),
        TCallBack( TimerExpired, this ) );
    iTimerCalled++;
    }

// ---------------------------------------------------------------------------
// Updates the plugin's state and reports progress to the observer.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPluginModel::SetStateL( TDiagBrowserState aTargetState )
    {
    LOGSTRING3( "CDiagBrowserPluginModel::SetStateL( %d ) - iState %d",
                aTargetState,
                iState )

    // Check for bad state transitions.
    switch ( aTargetState )
        {
        case EDiagBrowserStateConnectingBearer:
            __ASSERT_ALWAYS( iState == EDiagBrowserStateInitial,
                             Panic( EDiagBrowserPluginBadState ) );
            break;

        case EDiagBrowserStateConnectingServer:
            __ASSERT_ALWAYS( iState == EDiagBrowserStateConnectingBearer,
                             Panic( EDiagBrowserPluginBadState ) );
            break;

        case EDiagBrowserStateErasingLog:
            __ASSERT_ALWAYS( iState != EDiagBrowserStateInitial,
                             Panic( EDiagBrowserPluginBadState ) );
            break;

        default:
            Panic( EDiagBrowserPluginBadState );
        }

    // Update the state.  Send progress to the observer.
    iState = aTargetState;
    iObserver.TestProgressL( aTargetState );
    }

// ---------------------------------------------------------------------------
// Callback function for handling timer events.
// ---------------------------------------------------------------------------
//
TInt CDiagBrowserPluginModel::TimerExpired( TAny* aPtr )
    {
    LOGSTRING2( "CDiagBrowserPluginModel::TimerExpired( 0x%x )", aPtr )

    __ASSERT_ALWAYS( aPtr, Panic( EDiagBrowserPluginInternal ) );

    CDiagBrowserPluginModel* myThis =
        static_cast< CDiagBrowserPluginModel* >( aPtr );

    // Stop the timer to prevent further undesired callbacks.
    myThis->iTimer->Cancel();
    myThis->TimerCleanUp();
    // Try to continue test execution by retrying the secondary connection.
    //TRAPD( error, myThis->RetryConnectionL() );
    return KErrNone;
    }
void CDiagBrowserPluginModel::TimerCleanUp()
{
	    
	if (iState == EDiagBrowserStateConnectingBearer)
	{
		if(iTimerCalled<=1)
		StartConnectionTimerL();
		else
		iConnection.Stop( RConnection::EStopAuthoritative );
	}
	else if (iState == EDiagBrowserStateConnectingServer)
	iHttpTransaction.Cancel();
	
}
// ---------------------------------------------------------------------------
// Opens the network connection.  This may be called multiple times if a
// retry is necessary.
// ---------------------------------------------------------------------------
TBool CDiagBrowserPluginModel::OpenAndStartConnectionL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::OpenAndStartConnectionL()" )

    // Check if there is a valid connection to use.
    /*if ( !iPrimaryConnection.iIsValid && !iSecondaryConnection.iIsValid )
        {
        LOGSTRING( "CDiagBrowserPluginModel::OpenAndStartConnectionL() - No"
                   L" valid connections." )
        return EFalse;
        }
        */

    // Reset the connection.
    iHttpTransaction.Close();
    iHttpSession.Close();
    iConnection.Stop();
    iState = EDiagBrowserStateInitial;
	
    iConnectionPrefs.SetDialogPreference(ECommDbDialogPrefPrompt);//ECommDbDialogPrefDoNotPrompt ECommDbDialogPrefPrompt
    LOGSTRING("SetDialogPreference( ECommDbDialogPrefPrompt)");
    iConnectionPrefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    //iConnectionPrefs.SetIapId( iapID );
    //iConnectionPrefs.SetBearerSet(ECommDbBearerGPRS);
    iConnection.Start( iConnectionPrefs, iStatus );
    SetActive();
	ShowProgressNoteL();
    // Start the timer and set the state to notify progress.
    StartConnectionTimerL();
    SetStateL( EDiagBrowserStateConnectingBearer );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Handles ending the test by deleting the logs, if necessary, and informing
// the observer.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::HandleTestEndL(
    CDiagResultsDatabaseItem::TResult aResult )
    {
    LOGSTRING2( "CDiagBrowserPluginModel::HandleTestEndL( %d )", aResult )

    // Store the result, to return when log deletion completes.
    iResult = aResult;

    // Delete the logs, if necessary.
    if ( iState != EDiagBrowserStateInitial )
        {
        if ( !iLogEraser )
            {
            iLogEraser = CDiagLogEventEraser::NewL( *this );
            }

        // Set the filter for log erasing.
        TUid eventType;
        TTime now;
        eventType.iUid = KLogPacketDataEventType;
        now.UniversalTime();

        CLogFilter& filter = iLogEraser->Filter();
        filter.SetEventType( eventType );
        filter.SetStartTime( iStartTime );
        filter.SetEndTime( now );

        // Erase the logs from the test, asynchronously.
        SetStateL( EDiagBrowserStateErasingLog );
        iLogEraser->StartAsyncEraseLogsL();
        }
    else
        {
        iObserver.TestEndL( iResult );
        }
	
        
    // clear the dialogue only if dialog is running.    
    if(idialogOn)    
    iWaitDialog->ProcessFinishedL();
    }
// ----------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::ShowVibrationNoteL
//
// Progress Note
// ----------------------------------------------------------------------------

void CDiagBrowserPluginModel::ShowProgressNoteL()
    {
    if ( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
    
                        
    // WAIT NOTE DIALOG
   iWaitDialog = new (ELeave) CAknWaitDialog(
								(REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), 
								ETrue);
    
	iWaitDialog->PrepareLC(R_BROWSER_PLUGIN_WAIT_NOTE);
	iWaitDialog->SetCallback(this);
	iWaitDialog->RunLD();
	idialogOn = ETrue;
	LOGSTRING("CDiagBrowserPluginModel::ShowProgressNoteL()");

    }

//*********************************************************
// CallBack from AKnWaitDialog when Cancel is pressed
//*********************************************************

void CDiagBrowserPluginModel::DialogDismissedL( TInt aButtonId )
	{
		 LOGSTRING2("CDiagBrowserPluginModel::DialogDismissedL() = %d",aButtonId);
		 
		 if (aButtonId == EBrowserTestCancel && idialogOn) 
		 {
		 // cancel the tests based on which point the dialog is dismissed
		 LOGSTRING2("CDiagBrowserPluginModel::DialogDismissed @ iState= %d",iState);
		 if (iState == EDiagBrowserStateConnectingBearer)
	     iConnection.Stop( RConnection::EStopAuthoritative );
		 else if (iState == EDiagBrowserStateConnectingServer)
		 iHttpTransaction.Cancel();
	     iWaitDialog = NULL;
	     idialogOn = EFalse;
	     idialogDismissed = ETrue;      
		 }
	}    
// ---------------------------------------------------------------------------
// Attempts to continue test execution by retrying with the secondary
// connection.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::RetryConnectionL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::RetryConnectionL()" );
    
    if (iRetryCount < 1 && !idialogDismissed)
    {
           if ( OpenAndStartConnectionL() )
           {
           iRetryCount++;
           return;
           }         
    }
    HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::RunL()
    {
    LOGSTRING( "CDiagBrowserPluginModel::RunL()" )

    __ASSERT_ALWAYS( iState == EDiagBrowserStateConnectingBearer,
                     Panic( EDiagBrowserPluginBadState ) );

    // Stop the timer.
    if ( iTimer )
        {
        iTimer->Cancel();
        }

	if (iStatus == KErrConnectionTerminated )
	{
		HandleTestEndL( CDiagResultsDatabaseItem::ESkipped );
		return;
	}
    // Check if the connection succeeded.
    if ( iStatus != KErrNone )
        {
        LOGSTRING2(
            "CDiagBrowserPluginModel::RunL() - connection failed. Error %d",
            iStatus.Int() )
        LOGSTRING( "retry if possible()" );
        // Retry, if possible.
        //RetryConnectionL();
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        return;
        }

    // Open the HTTP Session.
    iHttpSession.OpenL();

    // Associate the HTTP session with the open connection.
    RStringPool pool = iHttpSession.StringPool();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    connInfo.SetPropertyL( pool.StringF( HTTP::EHttpSocketServ,
                                         RHTTPSession::GetTable() ),
                           THTTPHdrVal( iSocketServ.Handle() ) );
    TInt connectionPtr = reinterpret_cast< TInt >( &iConnection );
    connInfo.SetPropertyL( pool.StringF( HTTP::EHttpSocketConnection,
                                         RHTTPSession::GetTable() ),
                           THTTPHdrVal( connectionPtr ) );

    // Copy the URI to an 8-bit descriptor.  Directly getting an 8-bit
    // descriptor from Central Repository does not seem to work properly.
    TBuf8< KMaxStringLength > uri8;
    uri8.Copy( iUri );

    TUriParser8 uriParser;
    if ( uriParser.Parse( uri8 ) != KErrNone )
        {
        LOGSTRING2(
            "CDiagBrowserPluginModel::RunL() - parsing the uri '%S' failed.",
            &iUri )
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        return;
        }

    // Create an HTTP transaction, setting the model as the callback object.
    iHttpTransaction = iHttpSession.OpenTransactionL( uriParser, *this );
    iHttpTransaction.SubmitL();
    LOGSTRING2( "CDiagBrowserPluginModel::RunL() - HTTP transaction submitted"
                L" for '%S'",
                &iUri )

    // Start the timer and set the state to notify progress.
    StartConnectionTimerL();
    SetStateL( EDiagBrowserStateConnectingServer );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Handles leaves in the active object's RunL function.
// ---------------------------------------------------------------------------
TInt CDiagBrowserPluginModel::RunError( TInt aError )
    {
    LOGSTRING2( "CDiagBrowserPluginModel::RunError( %d )", aError )

    // Try to continue test execution by retrying the secondary connection.
    //TRAPD( error, RetryConnectionL() );
    //return error;
    HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::DoCancel()
    {
    LOGSTRING( "CDiagBrowserPluginModel::DoCancel()" )

    // Does not have an asynchronous cancel, but this functions as Cancel.
    iConnection.Stop( RConnection::EStopAuthoritative );
    }

// ---------------------------------------------------------------------------
// From class MHTTPTransactionCallback.
// Called when an event occurs on an HTTP transaction.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::MHFRunL( RHTTPTransaction aTransaction,
                                  const THTTPEvent& aEvent )
    {
    LOGSTRING2( "CDiagBrowserPluginModel::MHFRunL() - aEvent.iStatus %d",
                aEvent.iStatus )

    __ASSERT_ALWAYS( iState == EDiagBrowserStateConnectingServer,
                     Panic( EDiagBrowserPluginBadState ) );
    __ASSERT_ALWAYS( aTransaction.Id() == iHttpTransaction.Id(),
                     Panic( EDiagBrowserPluginTransaction ) );

    // Failed status.
    if ( aEvent == THTTPEvent::EFailed )
        {
        LOGSTRING( "CDiagBrowserPluginModel::MHFRunL() - Response failed." )
        //RetryConnectionL();
        //return;
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        }

    // Skip non-success / non-failure statuses.
    if ( aEvent != THTTPEvent::EGotResponseHeaders &&
         aEvent != THTTPEvent::EGotResponseBodyData &&
         aEvent != THTTPEvent::EResponseComplete &&
         aEvent != THTTPEvent::ESucceeded &&
         aEvent != THTTPEvent::ERequestComplete )
        {
        LOGSTRING( "CDiagBrowserPluginModel::MHFRunL() - Unused event." )
        return;
        }

    // Stop the request, so we don't get any further callbacks.
    iHttpTransaction.Cancel();

    // Get the HTTP response code and check it.
    RHTTPResponse response = aTransaction.Response();
    if ( response.StatusCode() != KHttpResponseCodeOk )
        {
        LOGSTRING2( "CDiagBrowserPluginModel::MHFRunL() - status code is %d",
                    response.StatusCode() )
        HandleTestEndL( CDiagResultsDatabaseItem::EFailed );
        }
    else
        {
        LOGSTRING(
            "CDiagBrowserPluginModel::MHFRunL() - connection succeeded" )
        HandleTestEndL( CDiagResultsDatabaseItem::ESuccess );
        }
    }

// ---------------------------------------------------------------------------
// From class MHTTPTransactionCallback.
// Called when an error occurs on an HTTP transaction.
// ---------------------------------------------------------------------------
TInt CDiagBrowserPluginModel::MHFRunError( TInt aError,
                                      RHTTPTransaction aTransaction,
                                      const THTTPEvent& /* aEvent */ )
    {
    LOGSTRING2( "CDiagBrowserPluginModel::MHFRunError( %d )", aError )

    __ASSERT_ALWAYS( iState == EDiagBrowserStateConnectingServer,
                     Panic( EDiagBrowserPluginBadState ) );
    __ASSERT_ALWAYS( aTransaction.Id() == iHttpTransaction.Id(),
                     Panic( EDiagBrowserPluginTransaction ) );

    // Try to continue test execution by retrying the secondary connection.
    TRAPD( error, RetryConnectionL() );
    return error;
    }

// ---------------------------------------------------------------------------
// From class MDiagLogEventEraserObserver.
// This function checks if the log event needs to be deleted.
// ---------------------------------------------------------------------------
TBool CDiagBrowserPluginModel::IsEventToBeDeleted( const CLogEvent& aEvent )
    {
    LOGSTRING( "CDiagBrowserPluginModel::IsEventToBeDeleted()" )

    if ( ( iPrimaryConnection.iUsed && aEvent.RemoteParty().Compare(
            iPrimaryConnection.iMethodName ) == 0 ) ||
         ( iSecondaryConnection.iUsed && aEvent.RemoteParty().Compare(
            iSecondaryConnection.iMethodName ) == 0 ) )
        {
        // The name matches, so delete the log entry.
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MDiagLogEventEraserObserver.
// This function notifies that the erase operation is complete.
// ---------------------------------------------------------------------------
void CDiagBrowserPluginModel::CompleteEventEraseL( TInt aError )
    {
    LOGSTRING2(
        "CDiagBrowserPluginModel::CompleteEventEraseL( %d )", aError )

    __ASSERT_ALWAYS( iState == EDiagBrowserStateErasingLog,
                     Panic( EDiagBrowserPluginBadState ) );

    // The error code is ignored, because it's not part of the criteria for
    // the test to pass.  We are deleting logs asynchronously, so the browser
    // portion of the test has already stored a result in iResult.
    iState = EDiagBrowserStateInitial;
    
    // dialog dismissed with Cancel
    
    if (idialogDismissed)
    iResult = CDiagResultsDatabaseItem::ESkipped;
    
    LOGSTRING2( "CDiagBrowserPluginModel::dialogDismissed = %d)", idialogDismissed)
    iObserver.TestEndL( iResult );
    }

// End of File
