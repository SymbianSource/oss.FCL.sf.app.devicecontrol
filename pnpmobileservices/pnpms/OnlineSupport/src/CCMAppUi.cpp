/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CCMAppUi class
*                methods
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include <apgcli.h>                     // for RApaLsSession
#include <commdb.h>                     // for CCommsDatabase
#include <cdbpreftable.h>               // for CCommsDbConnectionPrefTableView
#include <AknGlobalNote.h>              // for CAknGlobalNote
#include <AknGlobalConfirmationQuery.h> // for CAknGlobalConfirmationQuery
#include <escapeutils.h>
#include <etelmm.h>
#include <textresolver.h>
#include <sysutil.h>
#include <httpfiltercommonstringsext.h>
#include <onlinesupport.rsg>            // has to be system include.
#include <centralrepository.h>          // link against centralrepository.lib
#include <browseruisdkcrkeys.h>         // for KCRUidBrowser
#include <CoreApplicationUIsSDKCRKeys.h>// for KCRUidCoreApplicationUIs
//#include <BrowserUiInternalCRKeys.h>    //  BrowserUiSDKCRKeys.h file can replace this header file
#include <webutilsinternalcrkeys.h>     // for the profile strings

#include "OnlineSupportLogger.h"
#include "CCMAppUi.h"
#include "CCMRegistrationMonitor.h"
#include "CCMSocketsEngine.h"
#include "OnlineSupport.hrh"
#include "PnpUtilImpl.h"
#include "CCMDocument.h"
#include "MiniBrowser.h"
#include "DeviceParameterDefs.h"
#include "VersionStrings.h"
#include "OnlineSupportPrivateCRKeys.h" // Own central repository keys
#include <PnpProvUtil.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS

// UID of the application
const TUid KUidConfigurationManager = { 0x10204338 }; 
// TimeoutValue for the RegistrationMonitor,
// Registration monitor itself has a time-out of 15 seconds
const TInt KMonitoringTimeOut = 20000000;
// TimeoutValue for the ConnectionTest
const TInt KConnectionTimeOut = 35000000;
// Timeout after the startupOperation will continue after ConstructL()
const TInt KStartUpTimeOut = 20;

// 0.1s Timeout for closing the app after the user has selected "Cancel" to
// ConfirmationDialogL or to iWaitDialog.
// See comments on TApplicationStatus (CCMAppUi.h)
const TInt KExitTimeOut = 100000;

_LIT( KInitAccess, "initAccess" );

_LIT( KSlash,"/");

// Help portal 
_LIT(KHelpPortal,".help-portal.com");

// Nokia
_LIT( KServerNokia,"nokia" );
_LIT( KNokia,"pnpms.nokia.com");

// HTTP
_LIT(KHttp, "http://");

// Default server string
_LIT(KDefaultServerString,"www");

// Default page string
_LIT(KDefaultPageString,"page1");

_LIT( KMNC0, "mnc0" );
_LIT( KMCC, "mcc" );
_LIT( KGprs, "gprs" );
_LIT( KDot, "." );

const TInt KMaxAPNLength = 256;

_LIT( KDefaultRoamingName, "PNP Default Roaming" );

_LIT( KInitAccessRoamingName, "PNP InitAccess Roaming" );

const TInt KMaxSilentModeRedirectCount(3);


// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================
// None


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CCMAppUI::CCMAppUi
// -----------------------------------------------------------------------------
//
CCMAppUi::CCMAppUi():
    iAccessPoint(0),
    iAutoStart( EFalse),
    iTokenOverride( EFalse ),
    iStartingBrowser( EFalse ),
    iRedirectsLeft( KMaxSilentModeRedirectCount ),
    iInitAccessOn(KErrNone)
    {
    }

// -----------------------------------------------------------------------------
// CCMAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCMAppUi::ConstructL()
    {
    LOGSTRING( "Enter to CCMAppUi::ConstrtuctL" );
    // Send to background
    BaseConstructL(ENoScreenFurniture);
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KUidConfigurationManager );
    task.SendToBackground();
    
    HideApplicationFromFSW();

    iPreferencesChanged = EFalse;
    iFirstStartInHomeNetwork = EFalse;
    iNetworkStatusOk = EFalse; 
    iNetworkStatus = CCMRegistrationMonitor::EStatusUnknown;
    iApplicationStatus = EAppStatusInactive;
    iInitAccessPointUsed = EFalse;
    
    iWaitDialogProgressVal = 1;

    LOGSTRING( "BaseConstructOk" );
    
    //this->StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT);
    this->StatusPane()->MakeVisible( EFalse );

    CEikonEnv::Static()->AddForegroundObserverL( *this );

    TRAPD( err, iRepository = CRepository::NewL( KCRUidConfigurationManager ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "Central repository init: %i", err );
        User::Leave( err );
        }
    err = GetIntValue(KOnlineSupportInitAccessOn,iInitAccessOn);
    if( KErrNone == err && iInitAccessOn == KErrNone)
    	{//Fetaure off
    	iConnectingVia = EConnectingViaInit;
    	}
    else //feature off or Issue in reading cenrep key
    	{
    	iConnectingVia = EConnectingViaNull;
    	}
    // The default values; these might change later in ProcessCommandParametersL
    SetServerString( KDefaultServerString );
    SetPageString( KDefaultPageString );
    SetConnectReason( EConnectReasonUnknown );

    FetchProductModelAndSwVersionL();

    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iEngine = CCMSocketsEngine::NewL( *this );
    iRegistrationMonitor = CCMRegistrationMonitor::NewL( *this );
    iApplicationStatus = EAppStatusStartingUp;
    LOGSTRING( "Exit from ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CCMAppUI::IsNetworkConnectionsAllowedL
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::IsNetworkConnectionsAllowedL()
    {
    LOGSTRING( "Enter to CCMAppUi::IsNetworkConnectionsAllowedL()" );
    TBool networkConnectionsAllowed( EFalse );
    TInt allowed(0);
    CRepository* coreAppUisCr = CRepository::NewLC( KCRUidCoreApplicationUIs );
    User::LeaveIfError( coreAppUisCr->Get( KCoreAppUIsNetworkConnectionAllowed, allowed ) );
    CleanupStack::PopAndDestroy( coreAppUisCr );
    if( allowed == ECoreAppUIsNetworkConnectionAllowed )
        {
        LOGSTRING( "ConnectionAllowed" );
        networkConnectionsAllowed = ETrue;
        }
    else
        {
	// display dialog that operation that possible in offline mode
   	HBufC* string = StringLoader::LoadLC( R_OFFLINE_MODE );
        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL( globalNote );
        globalNote->ShowNoteL( EAknGlobalErrorNote , *string );
        CleanupStack::PopAndDestroy(2 );
	}

    LOGSTRING( "Exit from CCMAppUi::IsNetworkConnectionsAllowedL()" );
    return networkConnectionsAllowed;
    }

// Destructor
// -----------------------------------------------------------------------------
// CCMAppUI::~CCMAppUi
// -----------------------------------------------------------------------------
//
CCMAppUi::~CCMAppUi()
    {
    LOGSTRING( "Enter ~CCMAppUI" );

    CEikonEnv::Static()->RemoveForegroundObserver( *this );

    if (iTimer)
        {
        iTimer->Cancel();
        }
    delete iTimer;
    LOGSTRING( "Timer deleted" );

    if (iRegistrationMonitor)
        {
        iRegistrationMonitor->StopMonitoring();
        }
    delete iRegistrationMonitor;
    LOGSTRING( "Monitor deleted" );

    delete iRepository;

    delete iMiniBrowser;
    // Must be deleted after iMiniBrowser since iMiniBrowser uses RSocketServ and
    // RConnection instances of iEngine
    delete iEngine;
    
    if (iCommonDialogMonitor)
        {
        iCommonDialogMonitor->Cancel();
        delete iCommonDialogMonitor;
        }
    if (iGlobalConfirmationQuery)
        {
        delete iGlobalConfirmationQuery;
        }
    if( iGlobalWaitNote )
        {
        //iGlobalWaitNote->CancelNoteL(iWaitNoteId);
        delete iGlobalWaitNote;
        }

    LOGSTRING( "Exit ~CCMAppUI" );
    }



// -----------------------------------------------------------------------------
// CCMAppUI::ConfirmationDialogL
// Activates Confirmation dialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::ConfirmationDialogL()
    {
    LOGSTRING("CCMAppUi::ConfirmationDialogL");
    if( !iAutoStart ) 
        {
        HBufC* msgText = CEikonEnv::Static()->AllocReadResourceLC(
            R_CONFMANAGER_CONFIRMATION ); 
        
        if (iCommonDialogMonitor)
        {
            iCommonDialogMonitor->Cancel();
            delete iCommonDialogMonitor;
            iCommonDialogMonitor = NULL;
        }
        // instantiate the active object CGlobalConfirmationObserver
        iCommonDialogMonitor = CCMDialogMonitor::NewL( *this );
        iCommonDialogMonitor->SetDialogMode( CCMDialogMonitor::EConfirmation );
        // SetActive
        iCommonDialogMonitor->Start();
        // iGlobalConfirmationQuery is an instance of the global
        // confirmation query class CAknGlobalConfirmationQuery.
        // show the global confirmation query, set the iStatus variable
        // of the active object as a parameter.
        if (!iGlobalConfirmationQuery)
            {
            iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
            }
        
        iGlobalConfirmationQuery->ShowConfirmationQueryL(
            iCommonDialogMonitor->iStatus,
            *msgText,
            R_AVKON_SOFTKEYS_YES_NO__YES,
            R_QGN_NOTE_QUERY_ANIM);
            
        CleanupStack::PopAndDestroy( msgText );
        }
    }


// -----------------------------------------------------------------------------
// CCMAppUI::ErrorNoteConnectionL()
// Activates error note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::ErrorNoteConnectionL()
    {
    LOGSTRING( "CCMAppUi::ErrorNoteConnectionL" );
    if( !iAutoStart ) 
        {
        HBufC* msgText = CEikonEnv::Static()->
            AllocReadResourceLC( R_CONFMANAGER_ERRORNOTE ); 

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL( globalNote );
        globalNote->ShowNoteL( EAknGlobalErrorNote , *msgText );
        CleanupStack::PopAndDestroy( 2 );
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::ErrorNoteUnknownL
// -----------------------------------------------------------------------------
//
void CCMAppUi::ErrorNoteUnknownL( TInt aError )
    {
    LOGSTRING2( "CCMAppUi::ErrorNoteUnknownL %i", aError );
    if( !iAutoStart ) 
        {
        CTextResolver* textResolver = CTextResolver::NewLC();
        TPtrC errMsg = textResolver->ResolveErrorString( aError );
        HBufC* errMsgBuf = HBufC::NewLC( errMsg.Length() );
        errMsgBuf->Des().Append( errMsg );

        CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
        CleanupStack::PushL( globalNote );
        globalNote->ShowNoteL( EAknGlobalErrorNote , *errMsgBuf );
        
        CleanupStack::PopAndDestroy( globalNote );
        CleanupStack::PopAndDestroy( errMsgBuf );
        CleanupStack::PopAndDestroy( textResolver );
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::ProcessCommandParametersL
// 2 params
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/, TFileName& /*aDocumentName*/ )
    {
    LOGSTRING( "CCMAppUi::ProcessCommandParametersL 2 params" );
    return( ETrue );
    }

// -----------------------------------------------------------------------------
// CCMAppUI::ProcessCommandParametersL
// 3 params
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/, TFileName& /*aDocumentName*/, const TDesC8& aTail )
    {
    LOGSTRING( "CCMAppUi::ProcessCommandParametersL 3 params" );
    LOGSTRING( "aTail:" );
    LOGTEXT( aTail );
    if( !IsNetworkConnectionsAllowedL() )
        {
        // network connections not allowed
        LOGSTRING( "Network connections not allowed" );
        //DelayedExit();
        Exit();
        return ETrue;
        }

    if( aTail.Length() == 0 )
        {
        LOGSTRING( "no params" )
        }
    else
        {
        // aTail is in 8-bit format, cast
        // (The format was changed from 16-bit to 8-bit because of 3.0 porting;
        // there is not CApaCommandLine::SetFullCommandLine in 3.0, SetTailEndL
        // is used instead and it uses 8-bit format)
        HBufC* paramsBuf = HBufC::NewLC( aTail.Length() );
        TPtr paramsBufPtr = paramsBuf->Des();
        paramsBufPtr.Copy( aTail );
        TLex lexx( *paramsBuf );
        TChar getchar;

        for( ;; )
            {
            lexx.SkipSpace();
            lexx.Mark();
            TPtrC token = lexx.NextToken();

            if( token.Length() == 0 )
                {
                LOGSTRING( "token length == 0");
                break;
                }
            else if( token.Length() == 1 )
                {
                LOGSTRING( "token length == 1");
                break;
                }

            // s"www"
            // 012345

            if( token[1] == '\"' && token[token.Length()-1] != '\"' )
                {
                do
                    {
                    getchar = lexx.Get();
                    } while( (getchar != '\"') && (getchar != 0) );
                token.Set( lexx.MarkedToken() );
                }

            LOGSTRING( "token:" );
            LOGTEXT( token );

            TChar param = token[0];
            TPtrC paramVal;
            if( token[1] == '\"' )
                {
                if( token[2] == '\"' )
                    {
                    // just quotes, e.g. q""
                    }
                else
                    {
                    paramVal.Set( token.Mid(2,token.Length()-3) );
                    }
                }
            else
                {
                paramVal.Set( token.Mid(1) );
                }
            
            LOGSTRING( "paramVal:" );
            LOGTEXT( paramVal );

            switch( param )
                {
                case 's':
                    {
                    LOGSTRING( "setting server to:" );
                    LOGTEXT( paramVal );
                    SetServerString( paramVal );
                    break;
                    }
                case 'p':
                    {
                    LOGSTRING( "setting page to:" );
                    LOGTEXT( paramVal );
                    SetPageString( paramVal );
                    break;
                    }
                case 'r':
                    {
                    TLex lexx2( paramVal );
                    TInt reason = 0;
                    lexx2.Val(reason);
                    // Store connect reason
                    switch(reason)
                        {
                        case 0:
                            SetConnectReason(CCMAppUi::EConnectReasonUnknown);
                            LOGSTRING( "Unknown connection reason" );
                            break;
                        case 1:
                            SetConnectReason(CCMAppUi::EConnectReasonFirstBoot);
                            LOGSTRING( "Connection reason first boot" );
                            break;
                        case 2:
                            SetConnectReason(CCMAppUi::EConnectReasonNewSim);
                            LOGSTRING( "Connection reason new sim" );
                            break;
                        case 3:
                            SetConnectReason(CCMAppUi::EConnectReasonUserTriggered);
                            LOGSTRING( "Connection reason user triggered" );
                            break;
                        case 4:
                            SetConnectReason(CCMAppUi::EConnectReasonApplicationTriggered);
                            LOGSTRING( "Connection reason application triggered" );
                            break;
                        case 5:
                            SetConnectReason(CCMAppUi::EConnectReasonRemoteTriggered);
                            LOGSTRING( "Connection reason remote triggered" );
                            break;
                        default:
                            LOGSTRING( "Invalid connect reason parameter" );
                            break;
                        }
                    break;
                    }
                case 'q':
                    {
                    LOGSTRING( "setting query string to:" );
                    LOGTEXT( paramVal );
                    SetQueryString( paramVal );
                    break;
                    }
                case 'a':
                    {
                    LOGSTRING( "setting autostart to:" );
                    TLex lexx2( paramVal );
                    TInt autostart = 0; // Default is "off"
                    TInt hiddenEnabled(0);
                    TInt err(KErrNone);
                    lexx2.Val( autostart );
                    LOGTEXT( paramVal );
                    switch( autostart )
                        {
                        case 0:
                            iAutoStart = EFalse;
                            break;
                        case 1:
                            // check that autostart mode is allowed
                            err = GetIntValue( KOnlineSupportHiddenEnabled, hiddenEnabled );
                            if (err != KErrNone)
                                {
                                LOGSTRING2( "GetIntValue(KOnlineSupportHiddenEnabled) errorcode %i", err  );
                                }
                                
                            if(hiddenEnabled == 1)
                                {
                                iAutoStart = ETrue;
                                }
                            else
                                {
                                LOGSTRING( "Autostart not allowed" );
                                iAutoStart = EFalse;
                                }
                            break;
                        default:
                            iAutoStart = EFalse;
                            break;
                        }
                    break;
                    }
                case 't':
                    {
                    LOGSTRING( "setting TOKEN override to:" );
                    TLex lexx2( paramVal );
                    TInt tokenOverride = 0; // Default is "off"
                    lexx2.Val( tokenOverride );
                    LOGTEXT( paramVal );
                    switch( tokenOverride )
                        {
                        case 0:
                            iTokenOverride = EFalse;
                            break;
                        case 1:
                            iTokenOverride = ETrue;
                            break;
                        default:
                            iTokenOverride = EFalse;
                            break;
                        }
                    break;
                    }
                default:
                    {
                    LOGSTRING( "commandline parse error, unrecognized param indentifier" );
                    break;
                    }
                }
            lexx.SkipCharacters();    

            }
        LOGSTRING( "cmdline parsing done" );

        CleanupStack::PopAndDestroy( paramsBuf );
        } // else

       
    if( iAutoStart )
        {
        // do not show confirmation dialog, but call "ok"-button handler
        CommonDialogDismissedL(CCMDialogMonitor::EConfirmation, EAknSoftkeyYes);
        }
    else
        {
        // show confirmation dialog to user, after user has accepted or cancelled
        // CommonDialogDismissedL -function is called
        ConfirmationDialogL();

        }

    
    //StartTimeOut( KStartUpTimeOut );

    //TRAP_IGNORE( WaitDialogL( iWaitDialogProgressVal ) );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::DelayedExit
// -----------------------------------------------------------------------------
//
void CCMAppUi::DelayedExit()
    {
    LOGSTRING("CCMAppUi::DelayedExit - begin");
    StartTimeOut( KExitTimeOut );
    iApplicationStatus = EAppStatusClosing;

    // In case doing a delayed exit, iWaitDialog has not been deleted yet
    // (except if delayed exit is being done via iWaitDialog "Cancel" selection
    // -> see DialogDismissedL)
    /*if( iWaitDialog && !iWaitDialogProcessFinished )
        {
        LOGSTRING( "iWaitDialog->ProcessFinishedL" );
        iWaitDialogProcessFinished = ETrue;
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        LOGSTRING( "iWaitDialog->ProcessFinishedL - done" );
        }*/

    LOGSTRING("DelayedExit - done");
    }

// -----------------------------------------------------------------------------
// CCMAppUI::TimeOutCallBack
// -----------------------------------------------------------------------------
//
TInt CCMAppUi::TimeOutCallBack( TAny* aCCMAppUi )
    {
    TRAPD( err, ( ( CCMAppUi* )aCCMAppUi )->DoTimeOutCallBackL() );
    // DoTimeOutCallBackL might have called Exit()
    if( err == KLeaveExit )
        {
        User::Leave( KLeaveExit );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::DoTimeOutCallBackL
// -----------------------------------------------------------------------------
//
void CCMAppUi::DoTimeOutCallBackL()
    {
    LOGSTRING( "CCMAppUi::DoTimeOutCallBackL()" );
    LOGSTRING2( "application status %i", iApplicationStatus  )
    if( iTimer->IsActive() ) 
        {
        iTimer->Cancel();
        }

    switch( iApplicationStatus )
        {
        case EAppStatusMonitoring:
            LOGSTRING("CCMAppUi calls StopMonitoring ");
            iRegistrationMonitor->StopMonitoring();
            iApplicationStatus = EAppStatusInactive;
            //ErrorNoteNetworkL();
            ErrorNoteConnectionL();
            //DelayedExit();
            Exit();
            break;
        case EAppStatusConnecting:
            LOGSTRING("CCMAppUi calls disconnect");
            iEngine->Disconnect();
            iApplicationStatus = EAppStatusInactive;
            ConnectionResultL( KErrTimedOut );
            break;
        case EAppStatusWaitingDisconnect:
            LOGSTRING("Connection will be disconnected");
            iEngine->Disconnect();
            iApplicationStatus = EAppStatusInactive;
            break;
        case EAppStatusStartingUp:
            LOGSTRING("Starting up");
            StartUpOperationL();
            break;
        case EAppStatusClosing:
            LOGSTRING("EAppStatusClosing");
            LOGSTRING("Restore settings");
            // Exit immediately, a call to DelayedExit has been made
            // (and the exit timeout has already been passed)
            Exit(); // Leaves with KLeaveExit
            break;
        default:
            LOGSTRING("Restore settings");
            //DelayedExit();
            Exit();
            break;
        };

    LOGSTRING( "DoTimeOutCallBack - done" );
    }

// -----------------------------------------------------------------------------
// CCMAppUI::InitiateNextConnectionTestL()
// -----------------------------------------------------------------------------
//
void CCMAppUi::InitiateNextConnectionTestL()
    {
    LOGSTRING( "CCMAppUi::InitiateNextConnectionTestL" );
    TInt err( KErrNone );

    if( iNetworkStatus != CCMRegistrationMonitor::EHomenetwork &&
        iNetworkStatus != CCMRegistrationMonitor::ERoaming )
        {
        LOGSTRING( "InitiateNextConnectionTestL, network NOT ok" );
        if( iGlobalWaitNote )
            {
            iGlobalWaitNote->CancelNoteL(iWaitNoteId);
            delete iGlobalWaitNote;
            iGlobalWaitNote = NULL;
            }
        /*if( iWaitDialog && !iWaitDialogProcessFinished )
            {
            LOGSTRING("iWaitDialog->ProcessFinishedL()");
            iWaitDialogProcessFinished = ETrue;
            iWaitDialog->ProcessFinishedL();
            }*/
        if( iConnectReason == CCMAppUi::EConnectReasonFirstBoot )
            {
            LOGSTRING( "...startupstart" );
            //Roaming information to confmanager's ini file
            if ( iNetworkStatusOk == EFalse )
                {
                StoreIntValue( KOnlineSupportRoaming, EFalse );
                }
            Exit();
            //DelayedExit();
            return;
            }
        else // menu start, show error note
            {
            LOGSTRING( "ErrorNoteConnectionL" );
            //ErrorNoteNetworkL();
            ErrorNoteConnectionL();
            Exit();
            //DelayedExit();
            return;
            }
        }
    if( iConnectingVia == EConnectingViaNull && iInitAccessOn )
        {
        iConnectingVia = EConnectingViaInit;
        if( TryInitAccessL() ) 
            {
            return;
            }
        }
    if( iConnectingVia == EConnectingViaInit )
        {
        iConnectingVia = EConnectingViaDefault;
        __UHEAP_MARK;
        TBool value( EFalse );
        TRAP( err, value = TryDefaultAccessL() );
        __UHEAP_MARKEND;
        if( value )
            {
            return;
            }
        }
    if( iConnectingVia == EConnectingViaDefault )
        {
        iConnectingVia = EConnectingViaUser;
        TBool value( EFalse );
        TRAP( err, value = TryUserAccessL() );
        if( value )
            {
            LOGSTRING( "User access test initiated" );
            return;
            }
        else
            {
            LOGSTRING2( "TryUserAccessL err: %i", err );
            LOGSTRING( "ErrorNoteConnectionL" )
            ErrorNoteConnectionL();
            LOGSTRING( "Exit" )
            //DelayedExit();
            Exit();
            }
        }
    }    

// -----------------------------------------------------------------------------
// CCMAppUI::TryInitAccessL()
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::TryInitAccessL()
    {
    LOGSTRING( "CCMAppUi::TryInitAccessL()" );
    if( iInitAccessOn )
    	{
    TUint32 ap = InitAccessPointL();
    iInitAccessPointUsed = ETrue;

    if ( ap != 0 )
        {
        TBuf<255> domain;
        AppendDomainL( domain );
        LOGSTRING( "Starting Init access test" );
        iEngine->ConnectL( ap, domain );
        
        StartTimeOut( KConnectionTimeOut );
        iApplicationStatus = EAppStatusConnecting;
        return( ETrue );
        }

    LOGSTRING( "Init access failed in db" );
    return( EFalse );
    	}
    else
    	{
    	LOGSTRING( "CCMAppUi::TryInitAccessL() InitAccess feature off " );
    	return EFalse;
    	}
    }

// -----------------------------------------------------------------------------
// CCMAppUI::StartTimeOut
// -----------------------------------------------------------------------------
//
void CCMAppUi::StartTimeOut( const TInt aTimeOut )
    {
    LOGSTRING2( "CCMAppUi::StartTimeOut: %i", aTimeOut );
    // EAppStatusClosing means we are already waiting for the timer to
    // trigger application exit, no need for a new time-out in that case
    if( iApplicationStatus != EAppStatusClosing )
        {
        TCallBack callBack( TimeOutCallBack, this );
        if( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        iTimer->Start( aTimeOut, aTimeOut, callBack );
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::TryDefaultAccessL()
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::TryDefaultAccessL()
    {
    LOGSTRING( "CCMAppUi::TryDefaultAccessL" );
    TUint32 ap = CheckDefaultAccessPointL();
    iInitAccessPointUsed = EFalse;

    if ( ap != 0 )
        {
        TBuf<255> domain;
        AppendDomainL( domain );
        LOGSTRING( "Starting default access test" );
        iEngine->ConnectL( ap, domain );

        StartTimeOut( KConnectionTimeOut );
        iApplicationStatus = EAppStatusConnecting;
        return( ETrue );
        }

    LOGSTRING( "Default access failed in db" );
    return( EFalse );
    }


// -----------------------------------------------------------------------------
// CCMAppUI::TryUserAccessL()
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::TryUserAccessL()
    {
    LOGSTRING( "CCMAppUi::TryUserAccessL" );
    TInt accessPoints = CheckSettingsL();
    iInitAccessPointUsed = EFalse;

    // initAccess and browser default is tested, if more ap then ask from user
    if ( !iAutoStart )
        {
        	if( (iInitAccessOn && accessPoints > 2) || !iInitAccessOn)
        	{
        	  LOGSTRING( "Starting user access test" );

        		TBuf<255> domain;
        		AppendDomainL( domain );
        		iEngine->ConnectL( 0, domain );         

        		StartTimeOut( KConnectionTimeOut );
        		iApplicationStatus = EAppStatusConnecting;

        		return( ETrue );
        	}
        }	

    LOGSTRING( "User access test not started" );
    return( EFalse );
    }

/// -----------------------------------------------------------------------------
// CCMAppUI::CheckSettingsL
// Cheks if there is wap setting available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCMAppUi::CheckSettingsL()
    {
    CCommsDatabase*    theDb;
    CCommsDbTableView* theView;
    TInt settings( 0 );
    TInt errorCode( KErrNone );
    theDb = CCommsDatabase::NewL( );
    CleanupStack::PushL( theDb );
    theView = theDb->OpenTableLC( TPtrC( IAP ) );
    errorCode = theView->GotoFirstRecord();
    if ( errorCode != KErrNotFound )
        {
        settings = 1;
        while ( errorCode != KErrNotFound )
            {
            errorCode = theView->GotoNextRecord();
            if ( errorCode == KErrNone )
                {
                settings++;
                }
            }
        }
    CleanupStack::PopAndDestroy( 2 );
    return settings;    
    }

// -----------------------------------------------------------------------------
// CCMAppUI::AppendDomainL
// -----------------------------------------------------------------------------
//
void CCMAppUi::AppendDomainL( TDes& aUrl )
    {
    if( iServer.Compare( KServerNokia ) == 0 )
        {
        if( aUrl.MaxLength() < aUrl.Length() + KNokia().Length() )
            {
            User::Leave( KErrArgument );
            }
        aUrl.Append( KNokia );
        }
    else
        {
        if( aUrl.MaxLength() < aUrl.Length() + iServer.Length() + KHelpPortal().Length() )
            {
            User::Leave( KErrArgument );
            }
        aUrl.Append( iServer );
        aUrl.Append( KHelpPortal );
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::OpenWapBrowserL
// Opens WAP browser
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::OpenWapBrowserL()
    {
    LOGSTRING( "Enter to CCMAppUi::OpenWapBrowserL" );
    iWapPage.Zero();

    DeleteServiceActivationInfoL();
    
    CPnpUtilImpl* pnpUtil = CPnpUtilImpl::NewLC();

    LOGSTRING( "OpenWapBrowserL 2" );

    if( !iAutoStart )
        {
        // Help portal address and type
        // 4 means Url address. (see Browser API documentation)
        _LIT ( KAddressType, "4 " ); 
        iWapPage.Copy( KAddressType );
        }

    LOGSTRING( "OpenWapBrowserL 3" );

    iWapPage.Append( KHttp );
    AppendDomainL( iWapPage );
    iWapPage.Append( KSlash );

    LOGSTRING( "OpenWapBrowserL 4" );
    iWapPage.Append( iPage );
    iWapPage.Append( KQuestionMark );

#ifdef __WINS__
    RMobilePhone::TMobilePhoneNetworkInfoV1 homeNetworkInfo;
    homeNetworkInfo.iCountryCode.Copy( _L("244") );
    homeNetworkInfo.iNetworkId.Copy( _L("05") );
#else
    const RMobilePhone::TMobilePhoneNetworkInfoV1& homeNetworkInfo = iRegistrationMonitor->GetHomeNetworkInfo();
#endif
    // Home MCC
    //iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KSimMccParameter );
    iWapPage.Append( homeNetworkInfo.iCountryCode );
    pnpUtil->SetHomeMccL( homeNetworkInfo.iCountryCode );

    LOGSTRING( "OpenWapBrowserL 5" );
    // Home MNC
    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;
    pnpUtil->FormatMncCodeL( homeNetworkInfo.iCountryCode, homeNetworkInfo.iNetworkId, formattedMnc );
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KSimMncParameter );
    iWapPage.Append( formattedMnc );
    pnpUtil->SetHomeMncL( formattedMnc );

    LOGSTRING( "OpenWapBrowserL 6" );
#ifdef __WINS__
    RMobilePhone::TMobilePhoneNetworkInfoV1 currentNetworkInfo;
    currentNetworkInfo.iCountryCode.Copy( _L("244") );
    currentNetworkInfo.iNetworkId.Copy( _L("05") );
#else
    const RMobilePhone::TMobilePhoneNetworkInfoV1& currentNetworkInfo = iRegistrationMonitor->GetCurrentNetworkInfo();
#endif
    // current MCC
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KCurrentMccParameter );
    iWapPage.Append( currentNetworkInfo.iCountryCode );
    pnpUtil->SetNetworkMccL( currentNetworkInfo.iCountryCode );

    LOGSTRING( "OpenWapBrowserL 7" );
    // current MNC
    TRAPD( err, pnpUtil->FormatMncCodeL( currentNetworkInfo.iCountryCode, currentNetworkInfo.iNetworkId, formattedMnc ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "FormatMncCodeL err: %i", err );
        pnpUtil->SetNetworkMncL( currentNetworkInfo.iNetworkId );
        }
    else
        {
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KCurrentMncParameter );
        iWapPage.Append( formattedMnc );
        pnpUtil->SetNetworkMncL( formattedMnc );
        }

    LOGSTRING( "OpenWapBrowserL 8" );
    //pnpUtil->StoreAccessPoint( iAccessPoint ); // not needed anymore

    // Init AP, use token
    LOGSTRING( "iServer:" );
    LOGTEXT( iServer );

	// AppendTokenAndNonceVal
	AppendTokenAndNonceValL();

     // Connection reason
    LOGSTRING( "connection reason" );
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KConnectReasonParameter );
    if( iConnectReason == EConnectReasonUnknown )
        {
        iWapPage.AppendNum( (TInt)EConnectReasonUserTriggered );
        }
    else
        {
        iWapPage.AppendNum( (TInt)iConnectReason );
        }   

    // PnpVersion
    TBuf<KMaxVersionStringLength> pnpversion;
    User::LeaveIfError( pnpUtil->Version( pnpversion ) );
    LOGSTRING( "PnPVersion" );
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KPnpVersionParameter );
    iWapPage.Append( pnpversion );
    LOGSTRING( "PopAndDestroy pnpUtil" )
    CleanupStack::PopAndDestroy( pnpUtil );

    // help query string
    if( iQuery.Length() > 0 )
        {
        LOGSTRING( "query" );
        HBufC16* param = EscapeUtils::EscapeEncodeL(iQuery,EscapeUtils::EEscapeUrlEncoded);
        if(param)
            {
            LOGSTRING( "converted:" );
            LOGTEXT( *param );
            iWapPage.Append( KParameterDelimeter );
            iWapPage.Append( KQueryParameter );
            iWapPage.Append( *param );
            delete param;
            }

        }

    

    // Append Device Information to URL
    
    AppendDeviceInfoL();
    
    
    // Operator name, service provider
    const RMobilePhone::TMobilePhoneServiceProviderNameV2& serviceProviderName = iRegistrationMonitor->GetServiceProviderName();
    if( serviceProviderName.iSPName.Length() > 0 )
        {
        LOGSTRING( "spname" );
        HBufC16* param = EscapeUtils::EscapeEncodeL( serviceProviderName.iSPName, EscapeUtils::EEscapeUrlEncoded );
        if(param)
            {
            LOGSTRING( "converted:" );
            LOGTEXT( *param );
            iWapPage.Append( KParameterDelimeter );
            iWapPage.Append( KOperatorNameParameter );
            iWapPage.Append( *param );
            delete param;
            }
        }
    LOGTEXT( currentNetworkInfo.iShortName );
    LOGTEXT( currentNetworkInfo.iLongName );

    // Current operator name
    if( currentNetworkInfo.iLongName.Length() > 0 )
        {
        LOGSTRING( "current operator name" );
        HBufC16* param = EscapeUtils::EscapeEncodeL( currentNetworkInfo.iLongName, EscapeUtils::EEscapeUrlEncoded );
        if(param)
            {
            LOGSTRING( "converted:" );
            LOGTEXT( *param );
            iWapPage.Append( KParameterDelimeter );
            iWapPage.Append( KCurrentOperatorNameParameter );
            iWapPage.Append( *param );
            delete param;
            }
        }

    //Append WebUtils Params to URL
    AppendWebUtilParamsL();
    
    
    
    // MSISDN
    const RMobileONStore::TMobileONEntryV1& ownNumberInfo = iRegistrationMonitor->GetOwnNumberInfo();
    if( ownNumberInfo.iNumber.iTelNumber.Length() )
        {
        LOGSTRING( "ownNumberInfo.iNumber.iTelNumber" );
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KMSISDNParameter );
        iWapPage.Append( ownNumberInfo.iNumber.iTelNumber );
        }

    // Phone alias
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KPhoneAlias );
    iWapPage.Append( KPhoneAliasValue );

	//Create online help based on Interactive or NonInteractive feature
    OnlineHelpL();

    LOGSTRING( "Exit from OpenWapBrowser" );
    }
 
 
// -----------------------------------------------------------------------------
// CCMAppUI::DeleteServiceActivationInfoL
//
// -----------------------------------------------------------------------------
//
   
void CCMAppUi::DeleteServiceActivationInfoL()
{
	
	// Reset values for service activation. Service activation should not be supported 
    // by online support since WDS Global server URL is hardcoded (help-portal server 
    // connecting to WDS) if service activation required for onlinesupport comment below
    // statements for PnpProvUtil.

    CPnpProvUtil* prov = CPnpProvUtil::NewLC();
    
    const TUint32 uidval = 0;
    prov->SetApplicationUidL(uidval);
	
	
    TBuf<2> buf(_L(""));
    prov->SetProvAdapterAppIdL(buf);

    CleanupStack::PopAndDestroy();
	
}


// -----------------------------------------------------------------------------
// CCMAppUI::AppendTokenAndNonceValL
//
// -----------------------------------------------------------------------------
//
   
void CCMAppUi::AppendTokenAndNonceValL()
{


	CPnpUtilImpl *pnpUtil = CPnpUtilImpl::NewLC();

	    
    // TOKEN has to be sent:
    // - When connecting through "initAccess" to www.help-portal.com/page1
    // - When connecting through the fallback APN to *.help-portal.com EXCEPT to www.help-portal.com
    //     -- This feature is needed for testing of provisioning over HTTP
    // TOKEN should NOT be sent:
    // - When connecting through "initAccess" to any other URL
    // - When connecting through the fallback APN to pnpms.nokia.com

    // Changed:
    // Token override allows now giving token even when not connecting through initAccess,
    // so there is no need to allow TOKEN for demo purposes if token override is not on

    if( iTokenOverride    // token override set on commandline params
        ||
        // No token override:
        ( iInitAccessPointUsed
          && iServer.Compare( KDefaultServerString ) == 0 ) // connecting to www.help-portal.com
        )
        {
        // Add Token
        LOGSTRING( "token" );
        TInt validitytime = pnpUtil->GetTokenValidityTime();
        TInt tokenvalue = 0;
        TInt err = pnpUtil->CreateNewToken( validitytime, tokenvalue );
        if( err != KErrNone )
            {
            LOGSTRING2( "CreateNewToken returned: %i", err );
            User::Leave( err );
            }
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KTokenParameter );
        iWapPage.AppendNum( tokenvalue );
        // Token validity 
        LOGSTRING( "token validity" );
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KTokenValidityTimeParameter );
        iWapPage.AppendNum( validitytime );
        }

   
    // The KEYINFO and NONCE should be SENT:
    // - in all cases, also to "*help-portal.com" and through "initAccess".
 
    // Nonce
    LOGSTRING( "nonce" );
    // validity time not used
    TBuf8<KNonceLength> nonce8;
    TBuf<KNonceLength> nonce;
    TRAPD( err, pnpUtil->CreateNewNonceL( 0, nonce8 ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "CreateNewNonceL leave code: %i", err );
        User::Leave( err );
        }
    nonce.Copy( nonce8 );

    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KNonceParameter );
    iWapPage.Append( nonce );

    TBuf8<KMaxKeyInfoLength> keyInfo8;
    TBuf<KMaxKeyInfoLength> keyInfo;
    pnpUtil->GetKeyInfoL( keyInfo8 );
    keyInfo.Copy( keyInfo8 );

    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KKeyInfoParameter );
    iWapPage.Append( keyInfo );

	CleanupStack::PopAndDestroy();
	
}


// -----------------------------------------------------------------------------
// CCMAppUI::AppendSWandProductVersionL
//
// -----------------------------------------------------------------------------
//
  
void CCMAppUi::AppendDeviceInfoL()
{

	const RMobilePhone::TMobilePhoneIdentityV1& phoneIdentity = iRegistrationMonitor->GetPhoneIdentity();

    const TInt KManufLength = 128;

    // Device type (for example dType=NOKIAE60)
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KDeviceTypeParameter );
    
    _LIT(KManufacturer, "Nokia");    

    TBuf<KManufLength> manfacturer;
    manfacturer.Append(phoneIdentity.iManufacturer);

    TBuf<KManufLength> model;
    
    if(manfacturer.MatchF(KManufacturer)==KErrNotFound)
	 model.Append( phoneIdentity.iManufacturer );	


    model.Append( phoneIdentity.iModel );
    
    HBufC16* param_manf = EscapeUtils::EscapeEncodeL(model, EscapeUtils::EEscapeUrlEncoded );
    iWapPage.Append( *param_manf );
    delete param_manf;
    

    //iWapPage.Append( phoneIdentity.iModel );

    // Add symbian os version, for example: dType=NOKIAE60(Symbian 9.1)
    LOGSTRING("Reading OS versions");
    TVersionBuf s60Version;
    TVersionBuf symbianVersion;
    TRAPD( err, TVersionStrings::GetUserAgentVersionsL( s60Version, symbianVersion ) );
    LOGSTRING2( "Error: %i", err );
    if( err == KErrNone )
        {
        TBuf<KMaxVersionBufLength> symbian;
        symbian.Copy( symbianVersion );
        iWapPage.Append( KParenthesisOpen );
        iWapPage.Append( KSymbianOs );
        iWapPage.Append( symbian );
        iWapPage.Append( KParenthesisClose );
        }


	if( iSwVersion.Length() > 0 )
        {
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KSW );
	HBufC16* param_swversion = EscapeUtils::EscapeEncodeL(iSwVersion, EscapeUtils::EEscapeUrlEncoded );
	iWapPage.Append( *param_swversion );
	delete param_swversion;
	}
	LOGTEXT(iSwVersion);

    // Product model
    if( iProductModel.Length() > 0 )
        {
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KHW );
        iWapPage.Append( iProductModel );
        }	
}


// -----------------------------------------------------------------------------
// CCMAppUI::LaunchMiniBrowserL()
//
// -----------------------------------------------------------------------------
//
  
void CCMAppUi::LaunchMiniBrowserL()
{
		// create a mini browser for downloading provisioned settings
        // from a supporting service
        // mini browser has to be a member object as it has to exist
        // during the async http request-response cycle
        LOGSTRING2( "opening mini-browser, using AP: %i", iEngine->IapId() );
        if( !iMiniBrowser )
            {
            RSocketServ& socketServ = iEngine->SocketServ();
            RConnection& connection = iEngine->Connection();
            iMiniBrowser = CMiniBrowser::NewL( *this, socketServ, connection );
            }

        LOGSTRING( "Calling HttpGetL" );
        if( iEngine->ProxyDefined() )
            {
            iMiniBrowser->HttpGetL( iWapPage, iEngine->ProxyIp(), iEngine->ProxyPort() );
            }
        else
            {
            iMiniBrowser->HttpGetL( iWapPage );
            }
}



// -----------------------------------------------------------------------------
// CCMAppUI::LaunchS60WebBrowser()
//
// -----------------------------------------------------------------------------
//
void CCMAppUi::LaunchS60WebBrowserL()
{
	LOGSTRING2( "opening browser, using AP: %i", iAccessPoint );
        iEngine->Disconnect();
        
		// Append access point
        
		#if ( !defined (__WINS__) && !defined (__WINSCW) ) 
			iAccessPoint = iEngine->ReturnIAPID();
			iWapPage.Append( KSpace );
        	iWapPage.AppendNumUC( iAccessPoint );
		#endif
	
		

        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp( KCRUidBrowser );

        if ( task.Exists() )
            {
            TBuf8<1024> wapPage;
            wapPage.Copy( iWapPage.Left( 1024 ) );
            LOGSTRING( "OpenWapBrowserL sending message to Browser" );
            task.SendMessage( TUid::Uid( 0 ), wapPage ); // Uid is not used
            }
        else
            {
            LOGSTRING( "OpenWapBrowserL Launching Browser" );
            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            CleanupClosePushL( appArcSession );
            TThreadId tid;
            LOGSTRING( "before startDocument" );
	    LOGTEXT(iWapPage);
	
            appArcSession.StartDocument( iWapPage, KCRUidBrowser, tid );
            LOGSTRING( "Document started" );
            CleanupStack::PopAndDestroy( &appArcSession );
            }

        // Browser was started, we are free to exit
        iStartingBrowser = ETrue;
        //DelayedExit();
        Exit();
}


// -----------------------------------------------------------------------------
// CCMAppUI::AppendWebUtilParamsL()
//
// -----------------------------------------------------------------------------
//

void CCMAppUi::AppendWebUtilParamsL()
{
    // language
    // HttpFilterCommonStringsExt::GetLanguageString is introduced in S60 2.6
    RStringPool stringPool;
    LOGSTRING("open string pool");
    stringPool.OpenL( HttpFilterCommonStringsExt::GetLanguageTable() );
    CleanupClosePushL( stringPool );
    LOGSTRING("GetLanguageString");
    RStringF languageString = HttpFilterCommonStringsExt::GetLocaleLanguage( stringPool );
    CleanupClosePushL( languageString );
    LOGSTRING("GetLanguageString - done");
    LOGTEXT( languageString.DesC() );
    const TPtrC8& language = languageString.DesC();
    LOGSTRING("languageString.DesC - done");
    LOGTEXT( language );
    if( language.Length() )
        {
        const TInt KMaxLanguageLength(5);
        TBuf<KMaxLanguageLength> lang;
        lang.Copy( language.Left( KMaxLanguageLength ) );
        iWapPage.Append( KParameterDelimeter );
        iWapPage.Append( KLanguageParameter );
        iWapPage.Append( lang );
        }
    CleanupStack::PopAndDestroy(); // languageString.Close
    CleanupStack::PopAndDestroy(); // stringPool.Close

    // User Agent profile
    // Example value: "http://nds1.nds.nokia.com/uaprof/N6600r100.xml"
    LOGSTRING( "uaProf" );
    // Assume 128 chars is big enough
    TBuf<128> uaProf;
    CRepository* cenRep = CRepository::NewL( KCRUidWebUtils );
    CleanupStack::PushL( cenRep );
    TInt err = cenRep->Get( KWebUtilsUaProf, uaProf );
    CleanupStack::PopAndDestroy( cenRep );
    LOGSTRING2( "cenRep->Get err: %i", err );
    LOGTEXT( uaProf );
    if( uaProf.Length() )
        {
        HBufC16* param = EscapeUtils::EscapeEncodeL( uaProf, EscapeUtils::EEscapeUrlEncoded );
        LOGSTRING( "converted:" );
        LOGTEXT( *param );
        if( param )
            {
            iWapPage.Append( KParameterDelimeter );
            iWapPage.Append( KUAProfParameter );
            iWapPage.Append( *param );
            delete param;
            }
        }

        }

// -----------------------------------------------------------------------------
// CCMAppUI::OnlineHelpL()
//
// -----------------------------------------------------------------------------
//

void CCMAppUi::OnlineHelpL()
{
    // Interactive option (silent mode)
    iWapPage.Append( KParameterDelimeter );
    iWapPage.Append( KInteractiveSessionParameter );
    if( iAutoStart )
        {
        iWapPage.Append( KNonInteractiveSession );
        }
    else
        {
        iWapPage.Append( KInteractiveSession );
        }

#ifdef _DEBUG
    for( TInt i(0); i < iWapPage.Length(); i += 128 )
        {
        LOGTEXT( iWapPage.Right( iWapPage.Length() - i ) );
        }
#endif

    if( iAutoStart )
        {
        	LaunchMiniBrowserL();
        }
    else
        {
        	LaunchS60WebBrowserL();
        }
}
      
// -----------------------------------------------------------------------------
// CCMAppUI::MiniBrowserRequestDoneL
// Called after MiniBrowser has handled a request for an URL
// (a call to HttpGetL resulted a successful http request-response)
// -----------------------------------------------------------------------------
//
void CCMAppUi::MiniBrowserRequestDoneL()
    {
    LOGSTRING( "CCMAppUi::MiniBrowserRequestDoneL" );

    // Leaves from here propagate to MiniBrowserUnhandledLeave (which causes app exit)

    // Check we have not exceed the limit for maximum number of redirects
    if( iRedirectsLeft <= 0 )
        {
        User::Leave( KErrOverflow );
        }

    iRedirectsLeft--;

    // Leaves with KErrNotFound if there is no valid report URL
    HBufC* reportUrl = iMiniBrowser->GetReportUrlLC();
    LOGSTRING( "Calling HttpGetL" );
    if( iEngine->ProxyDefined() )
        {
        iMiniBrowser->HttpGetL( *reportUrl, iEngine->ProxyIp(), iEngine->ProxyPort() );
        }
    else
        {
        iMiniBrowser->HttpGetL( *reportUrl );
        }
    CleanupStack::PopAndDestroy( reportUrl );
    }

  
// -----------------------------------------------------------------------------
// CCMAppUI::MiniBrowserErrorL
// -----------------------------------------------------------------------------
//
void CCMAppUi::MiniBrowserErrorL( const TInt aError )
    {
    LOGSTRING2( "CCMAppUi::MiniBrowserErrorL: %i", aError );

    // If there is no valid report url the KErrNotFound from GetReportUrlLC
    // propagates to MiniBrowserUnhandledLeave and causes an Exit
    HBufC* reportUrl = iMiniBrowser->GetReportUrlLC();
    if( iEngine->ProxyDefined() )
        {
        iMiniBrowser->HttpGetL( *reportUrl, iEngine->ProxyIp(), iEngine->ProxyPort() );
        }
    else
        {
        iMiniBrowser->HttpGetL( *reportUrl );
        }
    CleanupStack::PopAndDestroy( reportUrl );
    // Keep compiler happy
    (void)aError;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::MiniBrowserErrorL
// Called if a call to MiniBrowserRequestDoneL or MiniBrowserErrorL leaves
// -----------------------------------------------------------------------------
//
void CCMAppUi::MiniBrowserUnhandledLeave( const TInt aError )
    {
    LOGSTRING2( "CCMAppUi::MiniBrowserUnhandledLeave: %i", aError );
    //DelayedExit();
    Exit();
    // Keep compiler happy
    (void)aError;  
    }

// -----------------------------------------------------------------------------
// CCMAppUI::CheckDefaultAccessPointL()
// Checks defaul access point for the browser
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CCMAppUi::CheckDefaultAccessPointL()
    {
    LOGSTRING( "CCMAppUi::CheckDefaultAccessPointL()" );
    TInt errorCode( KErrNone ) ;
    TUint32 accessPoint;
    TInt defaultAccessPoint( KErrNotFound );
    CCommsDatabase*    theDb;
    CCommsDbTableView* theView;
    CCommsDbTableView* theDefaultRoamingView;

    TUint32 returnValue( 0 );
    TInt selectionMode = 0;

    CRepository* browserRepository = CRepository::NewLC( KCRUidBrowser );
    User::LeaveIfError( browserRepository->Get( KBrowserDefaultAccessPoint, defaultAccessPoint ) );
  
    // Also check with Selection mode if selection mode is always ask then go for User Access   
    User::LeaveIfError(browserRepository->Get( KBrowserAccessPointSelectionMode, selectionMode ));
       
    CleanupStack::PopAndDestroy( browserRepository );
    
    if(selectionMode == 1)
    User::Leave(KErrNotSupported);
    

    theDb = CCommsDatabase::NewL( );
    CleanupStack::PushL( theDb ); 

    theView = theDb->OpenViewMatchingUintLC( TPtrC( WAP_IP_BEARER ),
                                     TPtrC( WAP_ACCESS_POINT_ID ), 
                                     defaultAccessPoint );

    User::LeaveIfError( theView->GotoFirstRecord() );

    theView->ReadUintL( TPtrC( WAP_IAP ),  accessPoint);
    returnValue = accessPoint;
    iAccessPoint = defaultAccessPoint;
    
    CleanupStack::PopAndDestroy( theView );

    // check type
    theView = theDb->OpenViewMatchingUintLC( TPtrC(IAP),
                                             TPtrC(COMMDB_ID),
                                             accessPoint );                
    errorCode = theView->GotoFirstRecord();
    if ( errorCode == KErrNone )
        {
        TUint32 bearerId;
        theView->ReadUintL( TPtrC(IAP_BEARER), bearerId );
        CleanupStack::PopAndDestroy( theView );
        
        theView = theDb->OpenViewMatchingUintLC( TPtrC(OUTGOING_GPRS),
                                                TPtrC(COMMDB_ID),
                                                bearerId );
        errorCode = theView->GotoFirstRecord();
        if ( errorCode == KErrNone )
            {
            TUint32 apType;
            theView->ReadUintL( TPtrC(GPRS_AP_TYPE), apType );
            CleanupStack::PopAndDestroy( theView );
            LOGSTRING2( "apType:%i", apType );
            }
        else
            {
            LOGSTRING2( "error in opening OUTGOING_GPRS:%i", errorCode );
            CleanupStack::PopAndDestroy( theView );
            }
        }
    else
        {
        LOGSTRING2( "error in opening WAP_IAP:%i", errorCode );
        CleanupStack::PopAndDestroy( theView );
        }
    // type check done

    if( iNetworkStatus == CCMRegistrationMonitor::ERoaming )
        {    
        // check the name for roaming support
        CApAccessPointItem* apItem = CApAccessPointItem::NewLC();
        CApDataHandler* apDataHandler = CApDataHandler::NewLC( *theDb );
        apDataHandler->AccessPointDataL( defaultAccessPoint, *apItem );
        const HBufC* apName = apItem->ReadConstLongTextL( EApGprsAccessPointName ); 
        // ownership of apName stays in apItem, so no delete here for apName
        LOGSTRING( "default apn:" );
        LOGTEXT( *apName );
        // check if it ends with ".gprs" == roaming safe
        TBuf<10> dotGprs;
        dotGprs.Append( KDot );
        dotGprs.Append( KGprs );
        TInt offset = apName->Find( dotGprs );
        TInt expectedOffset = apName->Length() - dotGprs.Length();
        if( offset != expectedOffset )
            {
            // we need to use a roaming safe version of the ap
            // check if allready have made it
            LOGSTRING( "Checking for existing KDefaultRoamingName" );
            theDefaultRoamingView = theDb->OpenViewMatchingTextLC( TPtrC( WAP_ACCESS_POINT ),
                                                 TPtrC( COMMDB_NAME ), 
                                                 KDefaultRoamingName );
            errorCode = theDefaultRoamingView->GotoFirstRecord();
            if( errorCode == KErrNone )
                {
                LOGSTRING( "found, removing it" );
                TUint32 defaultRoamingId;
                theDefaultRoamingView->ReadUintL( TPtrC(COMMDB_ID),  defaultRoamingId);
                LOGSTRING( "accesspoint id read done" );
                
                apDataHandler->RemoveAPL( defaultRoamingId );
                }
                
            LOGSTRING( "create it" );
            CApAccessPointItem* newAp = CApAccessPointItem::NewLC();
            
            newAp->CopyFromL( *apItem );

            const RMobilePhone::TMobilePhoneSubscriberId& imsi = iRegistrationMonitor->GetImsi();
            TBuf<KMaxAPNLength> newName;
            newName.Append( *apName );
            newName.Append( KDot );
            newName.Append( KMNC0 );
            newName.Append( imsi.Mid(3,2) );
            newName.Append( KDot );
            newName.Append( KMCC );
            newName.Append( imsi.Mid(0,3) );
            newName.Append( KDot );
            newName.Append( KGprs );
            newAp->SetNamesL( KDefaultRoamingName  );
            newAp->WriteTextL( EApWapAccessPointName, KDefaultRoamingName );
            newAp->WriteLongTextL( EApGprsAccessPointName, newName );
            LOGSTRING( "calling update" );
            TInt retries = KRetryCount;
            TRAPD( err, apDataHandler->CreateFromDataL(*newAp) );
            while( (err == KErrLocked) && retries-- )
                {
                User::After( KRetryWait );
                TRAP( err, apDataHandler->CreateFromDataL(*newAp) );
                }
            if( err != KErrNone )
                {
                User::Leave( err );
                }
            LOGSTRING( "calling update done" );
            
            newAp->ReadUint( EApWapIap, returnValue );
            newAp->ReadUint( EApWapAccessPointID, iAccessPoint );

            LOGSTRING( "update done" );
            CleanupStack::PopAndDestroy( newAp );
            CleanupStack::PopAndDestroy( theDefaultRoamingView );
            }
        CleanupStack::PopAndDestroy( apDataHandler );
        CleanupStack::PopAndDestroy( apItem );
        }
    CleanupStack::PopAndDestroy( theDb );


    LOGSTRING2( "Check default access point returns %i", returnValue );
    LOGSTRING2( "iAccessPoint %i", iAccessPoint );

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::InitAccessPointL()
// Cheks the id of accesspoint which APN is initAccess
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CCMAppUi::InitAccessPointL()
    {
    LOGSTRING( "CCMAppUi::InitAccessPointL()" );
    if(iInitAccessOn)
    	{
    CCommsDatabase*    theDb;

    theDb = CCommsDatabase::NewL();
    CleanupStack::PushL( theDb );

    TUint32 initAP( 0 );

    TBool value( EFalse );
    TRAPD( err, value = CheckInitAccessPointL(*theDb, initAP) );

    if( value == EFalse )
        {
        CreateInitAccessPointL( *theDb );
        TRAP( err, CheckInitAccessPointL( *theDb, initAP ) );
        }
    else
        {
        LOGSTRING( "Init AP was ok" );
        }
        

    CleanupStack::PopAndDestroy( theDb );

    return( initAP );
    }
    else
    	{
    	LOGSTRING( "CCMAppUi::InitAccessPointL Init Access feature off" );
    	return KErrNone;
    	}
    }

// -----------------------------------------------------------------------------
// CCMAppUI::CheckInitAccessPointL()
// Cheks the settings of accesspoint which APN is initAccess
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCMAppUi::CheckInitAccessPointL( CCommsDatabase& aDb, TUint32& aAccessPoint )
    {
    LOGSTRING( "CCMAppUi::CheckInitAccessPointL" ); 
if(iInitAccessOn)
	{
    CCommsDbTableView* theView;
    CCommsDbTableView* iapView;
    CCommsDbTableView* wapIpBearerView;
    TInt errorCode;
    TBool firstTime = ETrue; // crude solution
    TUint32 tableId;

    TBuf<KMaxAPNLength> initAccess;
    initAccess.Append( KInitAccess );

    const RMobilePhone::TMobilePhoneSubscriberId& imsi = iRegistrationMonitor->GetImsi();
    if( iNetworkStatus == CCMRegistrationMonitor::ERoaming )
        {
        initAccess.Append( KDot );
        initAccess.Append( KMNC0 );
        initAccess.Append( imsi.Mid(3,2) );
        initAccess.Append( KDot );
        initAccess.Append( KMCC );
        initAccess.Append( imsi.Mid(0,3) );
        initAccess.Append( KDot );
        initAccess.Append( KGprs );
        }
    
    theView = aDb.OpenViewMatchingTextLC( TPtrC( OUTGOING_GPRS),
                                         TPtrC( GPRS_APN ), 
                                         initAccess );
    errorCode = theView->GotoFirstRecord();

    FOREVER // will exit when no more records found or the first one not found or a valid found
        {
        if( !firstTime )
            {
            LOGSTRING( "trying to get next record" );
            errorCode = theView->GotoNextRecord();
            if( errorCode != KErrNone )
                {
                CleanupStack::PopAndDestroy( theView ); 
                return( EFalse );
                }
            LOGSTRING( "checking next record" );    
            }
        firstTime = EFalse;

        if ( errorCode == KErrNone )
            {
            theView->ReadUintL( TPtrC( COMMDB_ID ), tableId );
            iapView = aDb.OpenViewMatchingUintLC(TPtrC( IAP ),
                                             TPtrC( IAP_SERVICE ), 
                                             tableId );
            errorCode = iapView->GotoFirstRecord();

            if ( errorCode == KErrNone )
                {
                iapView->ReadUintL( TPtrC( COMMDB_ID ), aAccessPoint );
                wapIpBearerView = aDb.OpenViewMatchingUintLC(TPtrC( WAP_IP_BEARER ),
                                             TPtrC( WAP_IAP ), 
                                             aAccessPoint );
                errorCode = wapIpBearerView->GotoFirstRecord();
                if ( errorCode == KErrNone )
                    {
                    wapIpBearerView->ReadUintL( TPtrC( WAP_ACCESS_POINT_ID ), iAccessPoint );
                    }
                else
                    {
                    LOGSTRING( "wapIpBearerView->GotoFirstRecord()" );
                    CleanupStack::PopAndDestroy( wapIpBearerView ); // wapIpBearerView
                    CleanupStack::PopAndDestroy( iapView ); // iapView
                    continue;
                    }
                CleanupStack::PopAndDestroy(); // wapIpBearerView
                }
            else
                {
                LOGSTRING( "iapView->GotoFirstRecord() failed" );
                CleanupStack::PopAndDestroy( iapView ); // iapView
                continue;
                }

            CleanupStack::PopAndDestroy( iapView ); // iapView
            }
        else
            {
            LOGSTRING( "theView->GotoFirstRecord() failed" );
            CleanupStack::PopAndDestroy( theView );
            return( EFalse );
            }

        if( iServer == KDefaultServerString )
            {
            LOGSTRING( "checking that init access not modified" );
            // check also user name, prompt password, password, authentication, homepage
            TBuf8<100> userName;
            theView->ReadTextL( TPtrC(GPRS_IF_AUTH_NAME), userName );
            if( userName.Length() != 0 )
                {
                LOGSTRING( "username length was not 0" );
                continue;
                }
            TBool promptPasswd;
            theView->ReadBoolL( TPtrC(GPRS_IF_PROMPT_FOR_AUTH), promptPasswd );
            if( promptPasswd != EFalse )
                {
                LOGSTRING( "promptPasswd was not EFalse" );
                continue;
                }
            TBuf8<100> passwd;
            theView->ReadTextL( TPtrC(GPRS_IF_AUTH_PASS), passwd );
            if( passwd.Length() != 0 )
                {
                LOGSTRING( "passwd length was not 0" );
                continue;
                }
            LOGSTRING( "init access not modified" );
            }

        CleanupStack::PopAndDestroy( theView );
        return( ETrue );
        }
    }
    else
    	{
    	LOGSTRING( "CCMAppUi::CheckInitAccessPointL Init Access feature off" );
    	return EFalse;
    	}
    }

// -----------------------------------------------------------------------------
// CCMAppUI::CreateInitAccessPointL( CCommsDatabase& aDb )
// Creates the Init Access point to comms db
// -----------------------------------------------------------------------------
//
TUint32 CCMAppUi::CreateInitAccessPointL( CCommsDatabase& aDb )
    {
    if(iInitAccessOn)
    {
    CApDataHandler* apDataHandler = CApDataHandler::NewLC( aDb );
    CApAccessPointItem* apItem = CApAccessPointItem::NewLC();

    TBuf<KMaxAPNLength> initAccess;
    initAccess.Append( KInitAccess );

    if( iNetworkStatus == CCMRegistrationMonitor::ERoaming )
        {
        const RMobilePhone::TMobilePhoneSubscriberId& imsi = iRegistrationMonitor->GetImsi();
        initAccess.Append( KDot );
        initAccess.Append( KMNC0 );
        initAccess.Append( imsi.Mid(3,2) );
        initAccess.Append( KDot );
        initAccess.Append( KMCC );
        initAccess.Append( imsi.Mid(0,3) );
        initAccess.Append( KDot );
        initAccess.Append( KGprs );
        }

    if( iNetworkStatus == CCMRegistrationMonitor::ERoaming )
        {
        apItem->SetNamesL( KInitAccessRoamingName  );
        }
    else
        {
        apItem->SetNamesL( KInitAccess );
        }
    
    apItem->WriteLongTextL( EApGprsAccessPointName, initAccess );

    LOGSTRING( "CreateInitAccessPointL, setting bearer type" );
    apItem->SetBearerTypeL( EApBearerTypeGPRS );
    LOGSTRING( "CreateInitAccessPointL, setting isp type type" );
    apItem->WriteUint( EApIspIspType, EIspTypeInternetOnly );
    LOGSTRING( "CreateInitAccessPointL, creating" );
    apDataHandler->CreateFromDataL( *apItem );

    CleanupStack::PopAndDestroy( apItem );
    CleanupStack::PopAndDestroy( apDataHandler );
    
    return apItem->WapUid();
    	}
    else
    	{
    	LOGSTRING( "CCMAppUi::CreateInitAccessPointL Init Access feature off" );
    	return KErrNone;
    	}
    }

// -----------------------------------------------------------------------------
// CCMAppUI::ReadAccessPointFromPreferencesL()
// Reads the accesspoint value from Connection preferences table
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCMAppUi::ReadAccessPointFromPreferencesL()
    {
    LOGSTRING( "CCMAppUi::ReadAccessPointFromPreferencesL()" );
    TInt errorCode( KErrGeneral );
    TInt returnValue;
    CCommsDatabase*    theDb;
    CCommsDbConnectionPrefTableView* thePrefView;
    CCommsDbTableView* theView;
    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
    CCommsDbConnectionPrefTableView::TCommDbIapBearer bearer;

    theDb = CCommsDatabase::NewL( );
    CleanupStack::PushL( theDb );
    thePrefView = theDb->
        OpenConnectionPrefTableViewOnRankLC( ECommDbConnectionDirectionOutgoing, TUint( 1 ));
    returnValue = thePrefView -> GotoFirstRecord();
    if ( returnValue == KErrNone )
        {
        thePrefView->ReadConnectionPreferenceL( pref );
        bearer = pref.iBearer;
        LOGSTRING2("ReadAccessPointFromPreferencesL: iap %i", bearer.iIapId );
        
        theView = theDb->OpenViewMatchingUintLC( TPtrC( WAP_IP_BEARER ),
                                         TPtrC( WAP_IAP ), 
                                         bearer.iIapId );
        errorCode = theView->GotoFirstRecord();
        if ( errorCode == KErrNone )
            {
            theView->ReadUintL( TPtrC( WAP_ACCESS_POINT_ID ),  iAccessPoint );
            }

        CleanupStack::PopAndDestroy( theView );
        CleanupStack::PopAndDestroy( thePrefView );
        CleanupStack::PopAndDestroy( theDb );
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::CheckIniFileSettings
// cheks what kind of information exsist in configuration managers ini file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::CheckIniFileSettingsL()
    {
    LOGSTRING( "CCMAppUi::CheckIniFileSettingsL()" );
    TInt roaming(0);

    TInt errorCodeRoaming = GetIntValue( KOnlineSupportRoaming, roaming );
    LOGSTRING2("Errorcode roaming %i", errorCodeRoaming );    
    
    if ( errorCodeRoaming == KErrNone )
        {
        iFirstStartInHomeNetwork = ETrue;
        }
    }


TInt CCMAppUi::StoreIntValue( const TUint32 aCrKey, const TInt aValue )
    {
    return iRepository->Set( aCrKey, aValue );
    }

TInt CCMAppUi::StoreStringValue( const TUint32 aCrKey, const TDesC& aValue )
    {
    return iRepository->Set( aCrKey, aValue );
    }

TInt CCMAppUi::GetIntValue( const TUint32 aCrKey, TInt& aValue )
    {
    return iRepository->Get( aCrKey, aValue );
    }

TInt CCMAppUi::GetStringValue( const TUint32 aCrKey, TDes& aValue )
    {
    return iRepository->Get( aCrKey, aValue );
    }


// -----------------------------------------------------------------------------
// CCMAppUI::ShowWaitDialogL
// Shows wait note
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::ShowWaitDialogL()
    {
    LOGSTRING( "Enter to CCMAppUi::ShowWaitDialogL");
    if( !iAutoStart ) 
        {
        HBufC* msgText = CEikonEnv::Static()->
            AllocReadResourceLC( R_CONFMANAGER_WAITNOTE ); 

        if (iCommonDialogMonitor)
            {
            iCommonDialogMonitor->Cancel();
            delete iCommonDialogMonitor;
            iCommonDialogMonitor = NULL;
            }
        // instantiate the active object CGlobalConfirmationObserver
        iCommonDialogMonitor = CCMDialogMonitor::NewL( *this );
        
        iCommonDialogMonitor->SetDialogMode(CCMDialogMonitor::EWait);
        // SetActive
        iCommonDialogMonitor->Start();
        
        if (!iGlobalWaitNote)
            {
            iGlobalWaitNote = CAknGlobalNote::NewL();
            }

        iWaitNoteId = iGlobalWaitNote->ShowNoteL(
            iCommonDialogMonitor->iStatus,
            EAknGlobalWaitNote,
            *msgText );
        
        CleanupStack::PopAndDestroy( msgText );
        }
    LOGSTRING( "Exit from ShowWaitDialogL" );
    }

// -----------------------------------------------------------------------------
// CCMAppUI::StartUpOperationL()
// Shows wait note
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::StartUpOperationL()
    {
    LOGSTRING( "CCMAppUi::StartUpOperationL" );
    CheckIniFileSettingsL();

    iApplicationStatus = EAppStatusMonitoring;
    iRegistrationMonitor->StartMonitoring();

    // Start time-out timer
    StartTimeOut( KMonitoringTimeOut );
    LOGSTRING( "Monitoring started" );
    }

// -----------------------------------------------------------------------------
// CCMAppUi::SetServerString()
// Sets server string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::SetServerString(const TDesC& aServer)
    {
    if (aServer.Length()>KMaxServerString)
        {
        iServer.Copy( TPtrC( aServer.Left( KMaxServerString ) ) );
        }
    else
        {
        iServer.Copy( TPtrC( aServer ) );
        }
    LOGTEXT (iServer);
    }

// -----------------------------------------------------------------------------
// CCMAppUi::SetPageString()
// Sets page string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::SetPageString(const TDesC& aPage)
    {
    if (aPage.Length() > KMaxPageString)
        {
        iPage = TPtrC( aPage.Left( KMaxPageString ) );
        }
    else
        {
        iPage = TPtrC( aPage );
        }
    LOGTEXT (iPage);
    }

// -----------------------------------------------------------------------------
// CCMAppUi::SetConnectReason()
// Sets connect reason
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::SetConnectReason(TConnectReason aReason)
    {
    iConnectReason = aReason;
    }

// -----------------------------------------------------------------------------
// CCMAppUi::SetQueryString()
// Sets query string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::SetQueryString(const TDesC& aQuery)
    {
    if ( aQuery.Length() > KMaxQueryString )
        {
        iQuery = TPtrC( aQuery.Left( KMaxQueryString ) );
        }
    else
        {
        iQuery = TPtrC( aQuery );
        }
    LOGTEXT (iQuery);
    }

// -----------------------------------------------------------------------------
// CCMAppUI::ConnectionResultL
// -----------------------------------------------------------------------------
//
void CCMAppUi::ConnectionResultL( const TInt aErrorCode )
    {
    if( iApplicationStatus == EAppStatusClosing )
        {
        return;
        }
        
    if( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }
        
    iApplicationStatus = EAppStatusInactive;

    LOGSTRING( "CCMAppUi::ConnectionResultL" );
    LOGSTRING2( "iConnectingVia:%i", iConnectingVia );
    LOGSTRING2( "aErrrorCode:%i", aErrorCode );

    
    if( iConnectingVia == EConnectingViaUser ) // this was the last access point to test
        {
        if( aErrorCode != KErrNone )
            {
            LOGSTRING( "ErrorNoteConnectionL" );
            ErrorNoteConnectionL();
            LOGSTRING( "RestoreSettings done" );
            //DelayedExit();
            Exit();
            return;
            }
        else
            {
            ReadAccessPointFromPreferencesL();
            }
        }

    if( aErrorCode == KErrNone )
        {
        if( iGlobalWaitNote )
            {
            iGlobalWaitNote->CancelNoteL(iWaitNoteId);
            delete iGlobalWaitNote;
            iGlobalWaitNote = NULL;
            }
        /*        
        if( iConnectReason == CCMAppUi::EConnectReasonFirstBoot )
            {
            TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
            TApaTask task = taskList.FindApp( KUidConfigurationManager );
            if( iWaitDialog && !iWaitDialogProcessFinished )
                {
                LOGSTRING("iWaitDialog->ProcessFinishedL()");
                iWaitDialogProcessFinished = ETrue;
                iWaitDialog->ProcessFinishedL();
                }
            
            
            task.BringToForeground();
            }

        // this test succeeded, open the browser
        LOGSTRING( "Test succeeded, closing waitdialog" )
        if( iWaitDialog && !iWaitDialogProcessFinished )
            {
            LOGSTRING("iWaitDialog->ProcessFinishedL()");
            iWaitDialogProcessFinished = ETrue;
            iWaitDialog->ProcessFinishedL();
            }*/
        OpenWapBrowserL();
        }
    else
        {
        // failed, continue to next test
        iEngine->Disconnect();
        InitiateNextConnectionTestL();
        LOGSTRING( "InitiateNextConnectionTestL done" );
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::UnhandledLeave
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::UnhandledLeave( TInt aErrorCode )
    {
    LOGSTRING2( "CCMAppUi::UnhandledLeave: %i", aErrorCode );

    TRAP_IGNORE( ErrorNoteUnknownL( aErrorCode ) );
    //DelayedExit();
    Exit();
    }

// -----------------------------------------------------------------------------
// CCMAppUI::StatusChanged
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::RegistrationMonitoringDoneL( CCMRegistrationMonitor::TNetworkStatus aNetworkStatus )
    {    
    LOGSTRING2( "CCMAppUi::RegistrationMonitoringDoneL %i", aNetworkStatus );

    iNetworkStatus = aNetworkStatus;

    switch( aNetworkStatus )
        {
        case CCMRegistrationMonitor::EStatusUnknown:
        case CCMRegistrationMonitor::ERequestCancelled:
        case CCMRegistrationMonitor::ERoaming:
        case CCMRegistrationMonitor::EHomenetwork:
            {
            iNetworkStatusOk = ETrue;
            if( iApplicationStatus == EAppStatusClosing )
                {
                return;
                }   
            if( iTimer->IsActive() )
                {
                iTimer->Cancel();
                }
            iRegistrationMonitor->StopMonitoring(); 
            break;
            }
        default:
            break;
        };
    LOGSTRING( "Status changed" );
    InitiateNextConnectionTestL();
    }

// -----------------------------------------------------------------------------
// CCMAppUI::RegistrationReportErrorL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::RegistrationReportErrorL( const CCMRegistrationMonitor::TMonitoringStatus aStatus, const TInt aError )
    {
    LOGSTRING( "CCMAppUi::RegistrationReportErrorL " );
    LOGSTRING2( "Registration monitor failed: %i", aStatus );
    LOGSTRING2( "error: %i", aError );
    LOGSTRING2( "iApplicationStatus: %i", iApplicationStatus );

    if( iApplicationStatus != EAppStatusClosing )
        {
        iApplicationStatus = EAppStatusInactive;
        if( aError == KErrCancel )
            {
            //ErrorNoteNetworkL();
            ErrorNoteConnectionL();
            }
        else
            {
            ErrorNoteUnknownL(aError);
            }
        //DelayedExit();
        Exit();
        }
    // Keep compiler happy
    (void)aStatus;
    }

// -----------------------------------------------------------------------------
// CCMAppUI::DynInitMenuPaneL
// Takes care of menu handling.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/
        )
    {
    }

// -----------------------------------------------------------------------------
// CCMAppUI::HandleCommandL
// Handles commands
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::HandleCommandL( TInt  /*aCommand*/ )
    {
    }

// -----------------------------------------------------------------------------
// CCMAppUI::HandleKeyEventL
// Handles key events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCMAppUi::HandleKeyEventL(
        const TKeyEvent& /*aKeyEvent*/, // aKeyEvent Event to handled.
        TEventCode /*aType*/)           // aType Type of the key event.
    {
    return EKeyWasNotConsumed;
    }


// -----------------------------------------------------------------------------
// CCMAppUI::WaitDialogDismissedL
// Handles key events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::WaitDialogDismissedL( TInt aButtonId )
    {
    LOGSTRING2( "CCMAppUi::WaitDialogDismissedL %i", aButtonId );
    LOGSTRING2( "iApplicationStatus %i", iApplicationStatus );

    if( iApplicationStatus == EAppStatusClosing )
        {
        LOGSTRING("Already closing");
        return;
        }

    if( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }

    if( aButtonId == EAknSoftkeyCancel /*EConfmSoftkeyCancel*/
        || aButtonId == EAknSoftkeyExit )
        {
        // User pressed Cancel to iWaitDialog
        LOGSTRING("EConfmSoftkeyCancel");

        // iWaitDialog is already being destroyed, do not destroy again
        //iWaitDialogProcessFinished = ETrue;

        iEngine->Disconnect();
        //DelayedExit();
        Exit();
        return;
        }

    if ( iApplicationStatus == EAppStatusMonitoring )
        {
        LOGSTRING("EAppStatusMonitoring");
        //DelayedExit();
        Exit();
        }
    else if ( iApplicationStatus == EAppStatusConnecting )
        {
        LOGSTRING("EAppStatusConnecting");
        iEngine->Disconnect();
        //DelayedExit();
        Exit();
        }
    }

// -----------------------------------------------------------------------------
// CCMAppUI::HandleGainingForeground
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::HandleGainingForeground()
    {
    LOGSTRING( "CCMAppUi::HandleGainingForeground" );
    #ifndef _DEBUG // if urel -> hide application in silent mode
    if( iAutoStart )
        {
        // prevent user from switching to ServiceHelp application
        // when using silent mode by always sending ServiceHelp to background
        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp( KUidConfigurationManager );
        if( task.Exists() )
            {
            task.SendToBackground();
            }
        }
    #endif
    }

// -----------------------------------------------------------------------------
// CCMAppUI::HandleLosingForeground
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::HandleLosingForeground()
    {
    LOGSTRING( "CCMAppUi::HandleLosingForeground" );
    }
    
void CCMAppUi::CommonDialogDismissedL( const CCMDialogMonitor::TDialogMode aDialog, const TInt aStatusCode )
    {
    LOGSTRING( "CCMAppUi::CommonDialogDismissedL" );
    if( aDialog == CCMDialogMonitor::EConfirmation )
        {
        if( aStatusCode == EAknSoftkeyYes )
            {
            StartTimeOut( KStartUpTimeOut );
            ShowWaitDialogL();
            }
        else
            {
            Exit();
            }
        }
    else if ( aDialog == CCMDialogMonitor::EWait )
        {
        // cancel
        WaitDialogDismissedL( aStatusCode );
        }
    }


// -----------------------------------------------------------------------------
// CCMAppUI::FetchProductModelAndSwVersionL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMAppUi::FetchProductModelAndSwVersionL()
    {
    LOGSTRING( "CCMAppUi::FetchProductModelAndSwVersionL - begin" );
    iSwVersion.Zero();
    iProductModel.Zero();

    HBufC* tmpVersion = HBufC::NewLC( KSysUtilVersionTextLength );
    TPtr ptr( tmpVersion->Des() );
    User::LeaveIfError( SysUtil::GetSWVersion( ptr ) );
    LOGTEXT(ptr);

    _LIT(KVerStrStart,"V ");
    _LIT(KVerStrEnd,"\n");

    TInt pos1 = tmpVersion->Find(KVerStrStart);
    TInt pos2 = tmpVersion->Find(KVerStrEnd);
    TInt verlen = ((TDesC)(KVerStrStart)).Length();
    
    if( pos1==KErrNotFound) // Version does not start with "V "
        {
        pos1=0;
        verlen=0;
        }

    if(    (pos1!=KErrNotFound) 
        && (pos2!=KErrNotFound) 
        && (pos2 > (pos1 + verlen) ) 
      )
        {
        pos2 -= (verlen + pos1);
        TPtrC ptrSw = ptr.Mid(pos1+verlen,pos2);
        iSwVersion.Append(ptrSw);
        LOGTEXT( iSwVersion);

        TPtrC ptrSeek(ptr);
        pos1 = ptrSeek.Find(KVerStrEnd);
        if(pos1>=0)
            {
            ptrSeek.Set(ptrSeek.Mid(pos1+1));
            pos1 = ptrSeek.Find(KVerStrEnd);
            if( pos1 >= 0 )
                {
                ptrSeek.Set(ptrSeek.Mid(pos1+1));
                pos1 = ptrSeek.Find(KVerStrEnd);
                if( pos1 < 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1));
                    iProductModel.Append(ptrSeek);
                    }
                else if( pos1 > 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1,pos1-1));
                    iProductModel.Append(ptrSeek);
                    }
                LOGTEXT(iProductModel);
                }
            }
        
        }

    CleanupStack::PopAndDestroy();
    LOGSTRING( "CCMAppUi::FetchProductModelAndSwVersionL - end" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


// End of File  
