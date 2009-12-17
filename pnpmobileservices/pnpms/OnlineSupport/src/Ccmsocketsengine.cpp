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
* Description:  Class for the DNS query
*
*/




// INCLUDE FILES
#include <in_sock.h> 
#include <commdbconnpref.h>
#include <http.h>
#include <commdb.h>
#include <ApDataHandler.h>        // for CApDataHandler
#include <ApAccessPointItem.h>    // for EApHasProxySettings
#include <ApUtils.h>            // for CApUtils
#include <basched.h>            // for KLeaveExit
#include "CCMSocketsEngine.h"
#include "MCMSocketsEngineNotifier.h"
#include "OnlineSupportLogger.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

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
// CCMSocketsEngine::CCMSocketsEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CCMSocketsEngine::CCMSocketsEngine( MCMSocketsEngineNotifier& aNotifier ) 
    : CActive( CActive::EPriorityStandard ),
    iSocketsEngineNotifier( aNotifier ),
    iConnectionOpen( EFalse ),
    iResolverExsists( EFalse ),
    iLeaveOpen( EFalse ),
    iProxyPort(0),
    iProxyDefined( EFalse ),
    iHttpSessionOpen( EFalse ),
    iTransactionOpen( EFalse ),
    iIapId(0)
    {
    }


// -----------------------------------------------------------------------------
// CCMSocketsEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::ConstructL()
    {
    // Open channel to Socket Server
    User::LeaveIfError( iSocketServ.Connect() );
    CActiveScheduler::Add(this);
    iProxyIp = HBufC::NewL(1);
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCMSocketsEngine* CCMSocketsEngine::NewL( MCMSocketsEngineNotifier& aNotifier)
    {
    LOGSTRING( "Enter to CCMSocketsEngine::NewL " );
    CCMSocketsEngine* self = new( ELeave ) CCMSocketsEngine( aNotifier );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    LOGSTRING( "Exit from CCMSocketsEngine::NewL " );
    return self;
    }

    
// Destructor
CCMSocketsEngine::~CCMSocketsEngine()
    {
    LOGSTRING( "Enter to CCMSocketsEngine::~CCMSocketsEngine()" );
    if( IsActive() )
        {
        Cancel();
        }

    LOGSTRING( "CCMSocketsEngine::~CCMSocketsEngine() - 2" );

    if( iTransactionOpen )
        {
        LOGSTRING("Canceling transaction");
        iTransaction.Cancel();
        LOGSTRING("Closing transaction");
        iTransaction.Close();
        iTransactionOpen = EFalse;
        LOGSTRING("closed");
        }
    if( iHttpSessionOpen )
        {
        LOGSTRING("Closing session");
        iSession.Close();
        iHttpSessionOpen = EFalse;
        LOGSTRING("closed");
        }
    delete iProxyIp;
    if ( !iLeaveOpen )
        {
        LOGSTRING( "CCMSocketsEngine::~CCMSocketsEngine() - 3" );
        if( iConnectionOpen )
            {
            LOGSTRING("Closing connection");
            iConnection.Close();
            iConnectionOpen = EFalse;
            LOGSTRING("closed");
            }
        LOGSTRING( "CCMSocketsEngine::~CCMSocketsEngine() - 4" );
        iSocketServ.Close();
        }
    LOGSTRING( "Exit from CCMSocketsEngine::~CCMSocketsEngine()" );
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::Connect( const TDesC& aAddress )
// starts the DNS query for the address
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::ConnectL( TUint32 aAp, const TDesC& aAddress )
    {
    LOGSTRING( "CCMSocketsEngine::ConnectL" );
    LOGSTRING2( "Enter iEngine.Connect() %i", aAp );

    if ( IsActive() )
        {
        Cancel();
        }

    TCommDbConnPref prefs;
    if( aAp == 0 )
        {
        prefs.SetDialogPreference(ECommDbDialogPrefPrompt);
        }
    else
        {
        prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
        }
    prefs.SetIapId( aAp );

#ifdef __WINS__ // do not try to connect on the emulator
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
#else
    // Open connection
    TInt err;
    err = iConnection.Open( iSocketServ );
    if (err != KErrNone) // error occured
        {
        LOGSTRING2( "iConnection.Open err: %i", err );
        User::Leave( err );
        }
    iConnectionOpen = ETrue;
    iConnection.Start( prefs, iStatus );
#endif

    iEngineStatus = EStatusConnecting;
    iWapPage = aAddress;
    SetActive();

    LOGSTRING( "Exit from CCMSocketsEngine::Connect(). " );
    }


void CCMSocketsEngine::ConnectL( const TDesC& aAddress )
    {
    LOGSTRING( "CCMSocketsEngine::ConnectL. " );
     if ( IsActive() )
        {
        Cancel();
        }

#ifdef __WINS__ // do not try to connect on the emulator
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
#else
    // Open connection
    TInt err;
    err = iConnection.Open( iSocketServ );
    if (err != KErrNone) // error occured
        {
        LOGSTRING2( "iConnection.Open err: %i", err );
        User::Leave( err );
        }
    iConnectionOpen = ETrue;
    iConnection.Start( iStatus );
#endif

    iEngineStatus = EStatusConnecting;
    iWapPage = aAddress;
    SetActive();

    LOGSTRING( "Exit ftom CCMSocketsEngine::Connect(). " );
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::Disconnect()
// Disconnects the engine
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::Disconnect()
    {
    LOGSTRING( "CCMSocketsEngine::Disconnect()" );
    if ( IsActive() )
        {
        LOGSTRING("Canceling");
        Cancel();
        LOGSTRING("Canceled");
        }
    if( iTransactionOpen )
        {
        LOGSTRING("Closing transaction");
        iTransaction.Close();
        iTransactionOpen = EFalse;
        LOGSTRING("closed");
        }
    if( iHttpSessionOpen )
        {
        LOGSTRING("Closing session");
        iSession.Close();
        iHttpSessionOpen = EFalse;
        LOGSTRING("closed");
        }
    if ( iEngineStatus == EStatusResolving || iEngineStatus == EStatusReady )
        {
        if( iResolverExsists )
            {
            LOGSTRING( "CCMSocketsEngine::Disconnect() resolver exists" );
            iResolver.Cancel();
            iResolver.Close();
            iResolverExsists = EFalse;
            }
        iEngineStatus = EStatusUnknown;
        }
    if( iConnectionOpen )
        {
        LOGSTRING("Closing connection");
        iConnection.Close();
        iConnectionOpen = EFalse;
        LOGSTRING("closed");
        }

    iEngineStatus = EStatusUnknown;
    LOGSTRING( "CCMSocketsEngine::Disconnect() - done" );
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::SetOpen()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::SetOpen( TBool aOpen )
    {
    iLeaveOpen = aOpen;
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::DoCancel()
// Cancels the DNS query
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::DoCancel()
    {
    LOGSTRING( "CCMSocketsEngine::DoCancel()" );
    if( iTransactionOpen )
        {
        LOGSTRING("completing requeststatus");
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        LOGSTRING("Closing transaction");
        iTransaction.Close();
        iTransactionOpen = EFalse;
        LOGSTRING("closed");
        }
    if( iHttpSessionOpen )
        {
        LOGSTRING("Closing session");
        iSession.Close();
        iHttpSessionOpen = EFalse;
        LOGSTRING("closed");
        }
    if( iResolverExsists )
        {
        LOGSTRING( "iResolver.Cancel()" );
        iResolver.Cancel();
        LOGSTRING( "iResolver.Close()" );
        iResolver.Close();
        iResolverExsists= EFalse;
        }
    if( iConnectionOpen )
        {
        LOGSTRING("Closing connection");
        iConnection.Close();
        iConnectionOpen = EFalse;
        LOGSTRING("closed");
        }
    iEngineStatus = EStatusUnknown;
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::RunL()
// Handles object’s request completion event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCMSocketsEngine::RunL()
    {
    LOGSTRING( "CCMSocketsEngine::RunL()" );
    if ( iStatus.Int() != KErrNone )
        {
        iSocketsEngineNotifier.ConnectionResultL( iStatus.Int() );
        }
    else
        {
        switch( iEngineStatus )
            {
            case EStatusUnknown:
                LOGSTRING( "EStatusUnknown" );
            case EStatusConnecting:
                {
                LOGSTRING( "EStatusConnecting" );
#ifdef __WINS__ // do not try to connect on the emulator
                iSocketsEngineNotifier.ConnectionResultL( iStatus.Int() );
                iEngineStatus = EStatusReady;
                break;
#else
                _LIT( KIapId, "IAP\\Id" );
                User::LeaveIfError( iConnection.GetIntSetting( KIapId, iIapId ) );
                const TBool proxyDefined = CheckForProxyL( iIapId );

                if( proxyDefined )
                    {
                    MakeHttpHeadRequestL( *iProxyIp, iProxyPort );
                    iStatus = KRequestPending;
                    SetActive();
                    }
                else
                    {                
                    // Initiate a DNS query
                    LOGSTRING( "opening resolver, using udp protocol" );
                    TInt errorCode = iResolver.Open( iSocketServ, KAfInet, KProtocolInetUdp, iConnection );
                    //LOGSTRING( "opening resolver, using icmp protocol" );
                    //TInt errorCode = iResolver.Open( iSocketServ, KAfInet, KProtocolInetIcmp, iConnection );
                    //LOGSTRING( "opening resolver, using tcp protocol" );
                    //TInt errorCode = iResolver.Open( iSocketServ, KAfInet, KProtocolInetTcp, iConnection );
                    
                    if ( errorCode != KErrNone )
                        {
                        LOGSTRING( "Error while opening resolver" );
                        iSocketsEngineNotifier.ConnectionResultL( errorCode );
                        break;
                        }
                    else
                        {
                        LOGSTRING( "Resolver exists" );
                        iResolverExsists = ETrue;
                        // DNS request for name resolution
                        LOGSTRING( "Querying url:" );
                        LOGTEXT( iWapPage );
                        iResolver.GetByName( iWapPage, iNameEntry, iStatus );
                        SetActive();
                        iEngineStatus = EStatusResolving;
                        }
                    }
                break;
#endif
                }
            case EStatusMakingHttpHeadRequest:
                LOGSTRING( "EStatusMakingHttpHeadRequest" );
                iSocketsEngineNotifier.ConnectionResultL( iStatus.Int() );
                iEngineStatus = EStatusReady;
                break;
            case EStatusResolving:
                LOGSTRING( "EStatusResolving" );
                iSocketsEngineNotifier.ConnectionResultL( iStatus.Int() );
                iEngineStatus = EStatusReady;
                break;
            case EStatusReady:
                LOGSTRING( "EStatusReady" );
            default:
                break;
            };
        }   
        //iSocketsEngineNotifier.ConnectionResultL( iStatus.Int() );
    LOGSTRING( " Exit from CCMSocketsEngine::runL " );
    }

// -----------------------------------------------------------------------------
// CCMSocketsEngine::ReturnIAPID()
// Returns IAPID set when user selects access point before launching browser
// 
// -----------------------------------------------------------------------------
//

TUint32 CCMSocketsEngine::ReturnIAPID()
{
	
	return iIapId;
}


// -----------------------------------------------------------------------------
// CCMSocketsEngine::RunError()
// Handles leaves from RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCMSocketsEngine::RunError(TInt aError)
    {
    LOGSTRING2( "CCMSocketsEngine::RunError: %i", aError );
    if( aError == KLeaveExit )
        {
        return aError;
        }
    iSocketsEngineNotifier.UnhandledLeave( aError );
    return KErrNone;
    }


TBool CCMSocketsEngine::CheckForProxyL( const TUint32 aIapId )
    {
    LOGSTRING("CCMSocketsEngine::CheckForProxyL")

    // Check for proxy usage
    CCommsDatabase* commDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commDb );

    CApDataHandler* dataHandler = CApDataHandler::NewLC( *commDb );
    CApAccessPointItem* apItem = CApAccessPointItem::NewLC();
    CApUtils* utils = CApUtils::NewLC( *commDb );

    TUint32 id = utils->WapIdFromIapIdL( aIapId );
    dataHandler->AccessPointDataL( id, *apItem );

    TBool hasProxySettings;
    User::LeaveIfError( apItem->ReadBool( EApHasProxySettings, hasProxySettings ) );

    if( hasProxySettings )
        {
        LOGSTRING("HasProxySettings, reading proxy params");

        // Ownership not transferred
        const HBufC* proxyIp = apItem->ReadConstLongTextL( EApProxyServerAddress );
        delete iProxyIp;
        iProxyIp = 0;
        iProxyIp = HBufC::NewL( proxyIp->Length() );
        iProxyIp->Des().Copy( *proxyIp );
        LOGTEXT( *iProxyIp );

        TUint32 temp(0);
        apItem->ReadUint( EApProxyPortNumber, temp );
        iProxyPort = temp;
        }

    CleanupStack::PopAndDestroy( utils );
    CleanupStack::PopAndDestroy( apItem );
    CleanupStack::PopAndDestroy( dataHandler );
    CleanupStack::PopAndDestroy( commDb );

    LOGSTRING("CCMSocketsEngine::CheckForProxyL - done")
    return hasProxySettings;
    }

void CCMSocketsEngine::MakeHttpHeadRequestL(
    const TDesC& aProxyAddress,
    const TUint aProxyPort )
    {
    LOGSTRING( "CCMSocketsEngine::MakeHttpHeadRequestL" );
    _LIT8( KHttpProtString, "HTTP/TCP" );
    // Opens session using protocol HTTP/TCP
    LOGSTRING( "CCMSocketsEngine: iSession.OpenL" );
    iSession.OpenL( KHttpProtString );
    iHttpSessionOpen = ETrue;
    LOGSTRING( "CCMSocketsEngine: iSession.OpenL - done" );
    iSessionStringPool = iSession.StringPool();

    RHTTPConnectionInfo connectionInfo = iSession.ConnectionInfo();

    // Set SocketServ and Connection explicitly as we do not want to show
    // the AP selection list to the user when making submit for the request
    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
        iSocketServ.Handle() );
    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
        reinterpret_cast< TInt > ( &iConnection ) );
    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ),
        iSessionStringPool.StringF( HTTP::EUseProxy, RHTTPSession::GetTable() ) );

    LOGSTRING("Set Proxy address");
    HBufC8* proxy = HBufC8::NewLC( 100 );
    TPtr8 proxyPtr = proxy->Des();
    proxyPtr.Copy( aProxyAddress );
    proxyPtr.Append( _L8( ":" ) );
    proxyPtr.AppendNum( aProxyPort );
    LOGTEXT( proxyPtr );

    RStringF proxyF = iSessionStringPool.OpenFStringL( *proxy );
    CleanupClosePushL( proxyF );

    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EProxyAddress, RHTTPSession::GetTable() ),
        proxyF );

    CleanupStack::PopAndDestroy(1); // proxyF.Close()
    CleanupStack::PopAndDestroy( proxy );

    TUriParser8 uriParser;
    LOGSTRING( "CCMSocketsEngine uri:" );
    _LIT( KHttp, "http://" );
    TBuf8<128> uri;
    if( iWapPage.Find( KHttp ) != 0 )
        {
        uri.Append( KHttp );
        }
    uri.Append( iWapPage.Left(100) );
    LOGTEXT( uri );
    User::LeaveIfError( uriParser.Parse( uri ) );

    LOGSTRING( "CCMSocketsEngine: iSession.OpenTransactionL" );
    iTransaction = iSession.OpenTransactionL( uriParser, *this, iSessionStringPool.StringF( HTTP::EHEAD, RHTTPSession::GetTable() ) );
    iTransactionOpen = ETrue;
    LOGSTRING( "CCMSocketsEngine: iSession.OpenTransactionL - done" );

    iTransaction.SubmitL();
    LOGSTRING( "CCMSocketsEngine: SubmitL - done" );

    iEngineStatus = EStatusMakingHttpHeadRequest;
    }


void CCMSocketsEngine::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent &aEvent )
    {
    LOGSTRING( "CCMSocketsEngine::MHFRunL" );

    switch( aEvent.iStatus )
        {
        case THTTPEvent::ESubmit:
            {
            LOGSTRING( "CCMSocketsEngine::MHFRunL:ESubmit" );
            LOGTEXT( aTransaction.Request().URI().UriDes() );
            break;
            }
        case THTTPEvent::EGotResponseHeaders:
            {
            LOGSTRING( "CCMSocketsEngine::MHFRunL:EGotResponseHeaders" );
            // Success
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            break;
            }
        case THTTPEvent::EFailed:
            LOGSTRING( "CCMSocketsEngine::MHFRunL:EFailed" );
            // Failed
            User::Leave( KErrGeneral );
            break;
        default:
            {
            LOGSTRING2( "CCMSocketsEngine::MHFRunL:event: %i", aEvent.iStatus );
            break;
            }
        }
    // Keep compiler happy
    (void)aTransaction;
    LOGSTRING( "CCMSocketsEngine::MHFRunL - done" );
    }

TInt CCMSocketsEngine::MHFRunError( TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/ )
    {
    LOGSTRING2( "CCMSocketsEngine::MHFRunError %i", aError );
    if( iTransactionOpen )
        {
        LOGSTRING("Closing transaction");
        iTransaction.Close();
        iTransactionOpen = EFalse;
        LOGSTRING("closed");
        }
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
    return KErrNone;
    }

//  End of File  
