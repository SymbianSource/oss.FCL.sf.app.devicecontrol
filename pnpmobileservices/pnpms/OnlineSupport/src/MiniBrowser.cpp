/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mini web browser for downloading settings files
*
*/


// INCLUDE FILES
#include <in_sock.h>
#include <uri8.h>
#include <http.h>
#include <CWPEngine.h>
#include <commdbconnpref.h>
#include <etelmm.h>
#include <basched.h>            // For KLeaveExit
#include <AknGlobalNote.h> 
#include <avkon.rsg>            // For R_AVKON_SOFTKEYS_CLOSE

#include "MiniBrowser.h"
#include "OnlineSupportLogger.h"
#include "NHeadWrapperParser.h"
#include "SupportedContentTypes.h"
#include "VersionStrings.h"

// 50 secs time-out
const TInt KTimeOut( 50000000 );
const TInt KInitialDataBufferSize( 2048 );
const TInt KInitialReportUrlBufferSize( 512 );
// maximum length of the info message (currently not supported)
//const TInt KMaxMessageLength(256);
//_LIT( KMiniBrowserPanic, "CMiniBrowser" );
_LIT8( KHttpProtString, "HTTP/TCP" );
_LIT8( KNokiaHeadWrapper, "application/vnd.nokia.headwrapper" );

_LIT( KAmpersand, "&" );
_LIT( KQuestionMark, "?" );
_LIT( KStatus, "Status=" );

CMiniBrowser* CMiniBrowser::NewLC(
    MMiniBrowserObserver& aObserver,
    RSocketServ& aSockerServ,
    RConnection& aConnection )
    {
    CMiniBrowser* self = new (ELeave) CMiniBrowser( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aSockerServ, aConnection );
    return self;
    }

CMiniBrowser* CMiniBrowser::NewL(
    MMiniBrowserObserver& aObserver,
    RSocketServ& aSockerServ,
    RConnection& aConnection )
    {
    CMiniBrowser* self = NewLC( aObserver, aSockerServ, aConnection );
    CleanupStack::Pop( self );
    return self;
    }

CMiniBrowser::~CMiniBrowser()
    {
    LOGSTRING( "CMiniBrowser::~CMiniBrowser" );
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }

    delete iUri;
    delete iPeriodic;
    delete iResponseData;
    delete iReportUrl;
    iTransaction.Close();
    iSession.Close();

    LOGSTRING( "CMiniBrowser::~CMiniBrowser - done" );
    }

CMiniBrowser::CMiniBrowser( MMiniBrowserObserver& aObserver ) :
    iObserver( aObserver ),
    iContentType( EContentTypeUnknown ),
    iProvisioningStatus( THttpProvStates::EStatusUnknown )
    {
    }

void CMiniBrowser::ConstructL( RSocketServ& aSockerServ, RConnection& aConnection )
    {
    LOGSTRING( "CMiniBrowser::ConstructL" );

    iUri = HBufC8::NewL(1);
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    iResponseData = HBufC8::NewL( KInitialDataBufferSize );
    iReportUrl = HBufC8::NewL( KInitialReportUrlBufferSize );

    // Opens session using protocol HTTP/TCP
    LOGSTRING( "CMiniBrowser: iSession.OpenL" );
    iSession.OpenL( KHttpProtString );
    LOGSTRING( "CMiniBrowser: iSession.OpenL - done" );
    iSessionStringPool = iSession.StringPool();

    RHTTPConnectionInfo connectionInfo = iSession.ConnectionInfo();

    // Set SocketServ and Connection explicitly as we do not want to show
    // the AP selection list to the user when making submit for the request
    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EHttpSocketServ,
        RHTTPSession::GetTable() ),
        aSockerServ.Handle() );
    connectionInfo.SetPropertyL(
        iSessionStringPool.StringF( HTTP::EHttpSocketConnection,
        RHTTPSession::GetTable() ),
        reinterpret_cast< TInt > ( &aConnection ) );
    LOGSTRING( "CMiniBrowser::ConstructL - done" );
    }

void CMiniBrowser::HttpGetL( const TDesC& aUri )
    {
    DoHttpGetL( aUri, EFalse, KNullDesC, 0 );
    }

void CMiniBrowser::HttpGetL( const TDesC& aUri, const TDesC& aProxyAddress,
    const TUint aProxyPort )
    {
    DoHttpGetL( aUri, ETrue, aProxyAddress, aProxyPort );
    }

void CMiniBrowser::DoHttpGetL(
    const TDesC& aUri,
    const TBool aUseProxy,
    const TDesC& aProxyAddress,
    const TUint aProxyPort )
    {
    LOGSTRING( "CMiniBrowser::DoHttpGetL" );

    ResetResponseDataL();

    delete iUri;
    iUri = 0;
    iUri = HBufC8::NewL( aUri.Length() );
    iUri->Des().Copy( aUri );

    TUriParser8 uriParser;

    // In case URL is not valid the transaction will fail with code KErrHttpInvalidUri
    // uriParser does not leave in all invalid uri cases
#ifdef _DEBUG
    LOGSTRING( "CMiniBrowser::HttpGetL uri:" );
    //LOGTEXT( aUri );
    for( TInt i(0); i < aUri.Length(); i += 128 )
        {
        LOGTEXT( aUri.Right( aUri.Length() - i ) );
        }
#endif
    User::LeaveIfError( uriParser.Parse( *iUri ) );

    RHTTPConnectionInfo connectionInfo = iSession.ConnectionInfo();

    // The default value is HTTP::EDoNotUseProxy
    if( aUseProxy )
        {
        LOGSTRING("CMiniBrowser: using proxy");
        connectionInfo.SetPropertyL(
            iSessionStringPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ),
            iSessionStringPool.StringF( HTTP::EUseProxy, RHTTPSession::GetTable() ) );

        LOGSTRING("Set Proxy address")
        HBufC8* proxy = HBufC8::NewLC( aProxyAddress.Length() + 1 + 16 );
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
        }
    else
        {
        LOGSTRING("CMiniBrowser: NOT using proxy");
        connectionInfo.SetPropertyL(
            iSessionStringPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ),
            iSessionStringPool.StringF( HTTP::EDoNotUseProxy, RHTTPSession::GetTable() ) );
        }

    LOGSTRING( "CMiniBrowser: iSession.OpenTransactionL" );
    iTransaction = iSession.OpenTransactionL( uriParser, *this,
        iSessionStringPool.StringF( HTTP::EGET, RHTTPSession::GetTable() ) );
    LOGSTRING( "CMiniBrowser: iSession.OpenTransactionL - done" );


    // Set user agent string (we have our own user agent string definition,
    // not Mozilla compatible)
    HBufC8* userAgent = NULL;
    TRAPD( err, userAgent = GetUserAgentStringL() );
    if( err != KErrNone )
        {
        LOGSTRING2( "Error in GetUserAgentStringL: %i", err );
        }
    else
        {
        CleanupStack::PushL( userAgent );
        LOGTEXT( *userAgent );
        RHTTPHeaders headers = iTransaction.Request().GetHeaderCollection();
        RStringF str = iSessionStringPool.OpenFStringL( *userAgent );
        CleanupClosePushL( str );
        headers.SetFieldL(
            iSessionStringPool.StringF( HTTP::EUserAgent, RHTTPSession::GetTable() ),
            str );
        CleanupStack::PopAndDestroy(); // str.Close()
        CleanupStack::PopAndDestroy( userAgent );
        }


    TCallBack callBack( TimeOutCallBack, this );
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }
    iPeriodic->Start( KTimeOut, KTimeOut, callBack );

    iTransaction.SubmitL();
    LOGSTRING( "CMiniBrowser::HttpGetL - done" );
    }

HBufC8* CMiniBrowser::GetUserAgentStringL()
    {
    LOGSTRING("CMiniBrowser::GetUserAgentStringL");

    _LIT8( KUserAgentName, "OnlineSupport");
    _LIT8( KSlash, "/");
    _LIT8( KSpace, " ");
    _LIT8( KSymbianOS, "SymbianOS");
    _LIT8( KS60, "Series60");
    _LIT8( KQuestionMark, "?" );
    _LIT( KPhoneModuleName, "Phonetsy.tsy");
    _LIT( KPhoneName, "DefaultPhone");
    const TChar lineFeedChar( 0xA );

    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    User::LeaveIfError( telServer.LoadPhoneModule( KPhoneModuleName ) );
    RMobilePhone phone;
    User::LeaveIfError( phone.Open( telServer, KPhoneName ) );
    CleanupClosePushL( phone );
    LOGSTRING("CMiniBrowser::GetUserAgentStringL 2");

    RMobilePhone::TMobilePhoneIdentityV1 phoneIdentity;
    TRequestStatus status( KRequestPending );
    phone.GetPhoneId( status, phoneIdentity );
    User::WaitForRequest( status );


    // parse sw version from the string
    // sw version is assumed to be string between first "V" and "\n"
    LOGSTRING("Reading sw version info");
    TBuf<KSysUtilVersionTextLength> versions;
    User::LeaveIfError( SysUtil::GetSWVersion( versions ) );
    LOGTEXT( versions );

    // SW version
    LOGSTRING("Locating sw version from the string");
    TInt begin(0);
    TInt len(0);
    // Assume sw version is in the first row (use line feed as a token separator)
    TBuf8<KSysUtilVersionTextLength> sw;
    TVersionStrings::TokenLocationL( versions, lineFeedChar, 0, sw.MaxLength(), begin, len );
    if( len <= 2 )
        {
        User::Leave( KErrNotFound );
        }

    _LIT(KVerStrStart,"V ");
    TInt pos1 = versions.Find(KVerStrStart);
    if (pos1 != KErrNotFound)
        {
        sw.Copy( versions.Mid( begin + 2, len - 2 ) ); // remove "V "
        }
    else
        {
        // SW version does not always start with "V "
        sw.Copy( versions.Mid( begin, len ) );
        }
    
    LOGTEXT( sw );


    // Series 60 and Symbian OS versions
    LOGSTRING("Reading OS versions");
    TVersionBuf s60Version;
    s60Version.Copy( KQuestionMark );
    TVersionBuf symbianVersion;
    symbianVersion.Copy( KQuestionMark );
    TRAPD( err, TVersionStrings::GetUserAgentVersionsL( s60Version, symbianVersion ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "Error: %i", err );
        }

    LOGSTRING("Series60 version");
    LOGTEXT( s60Version );
    LOGSTRING("SymbianOS version");
    LOGTEXT( symbianVersion );


    LOGSTRING("CMiniBrowser::GetUserAgentStringL 3");
    // The string is of format:
    // OnlineSupport <manufacturer><model>/<sw> SymbianOS/<symbian_os_version> S60/<S60_version>
    HBufC8* agent = HBufC8::NewLC(
        KUserAgentName().Length() +
        phoneIdentity.iManufacturer.Length() +
        phoneIdentity.iModel.Length() +
        sw.Length() +
        KSymbianOS().Length() +
        symbianVersion.Length() +
        KS60().Length() +
        s60Version.Length() +
        3 * KSlash().Length() +      /* three slasles in the string */
        3 * KSpace().Length()        /* three spaces in the string */
        );
    LOGSTRING("CMiniBrowser::GetUserAgentStringL 3.5");
    TPtr8 agentPtr = agent->Des();
    agentPtr.Append( KUserAgentName );
    agentPtr.Append( KSpace);
    agentPtr.Append( phoneIdentity.iManufacturer );
    agentPtr.Append( phoneIdentity.iModel );
    agentPtr.Append( KSlash );
    agentPtr.Append( sw );
    agentPtr.Append( KSpace);
    agentPtr.Append( KSymbianOS );
    agentPtr.Append( KSlash);
    agentPtr.Append( symbianVersion );
    agentPtr.Append( KSpace);
    agentPtr.Append( KS60 );
    agentPtr.Append( KSlash);
    agentPtr.Append( s60Version );

    LOGSTRING("CMiniBrowser::GetUserAgentStringL 4");
    CleanupStack::Pop( agent );
    CleanupStack::PopAndDestroy(); // phone.Close()
    CleanupStack::PopAndDestroy(); // telServer.Close()
    LOGSTRING("CMiniBrowser::GetUserAgentStringL - done");
    return agent;
    }


TInt CMiniBrowser::TimeOutCallBack( TAny* aMiniBrowser)
    {
    TRAPD( err, ( ( CMiniBrowser* ) aMiniBrowser )->DoTimeOutCallBackL() );
    if( err == KLeaveExit )
        {
        User::Leave( KLeaveExit );
        }
    return err;
    }

void CMiniBrowser::DoTimeOutCallBackL()
    {
    LOGSTRING( "CMiniBrowser::DoTimeOutCallBackL" );
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }

    TRAPD( err, iObserver.MiniBrowserErrorL( KErrTimedOut ) );
    if( err != KErrNone )
        {
        iObserver.MiniBrowserUnhandledLeave( err );
        }
    if( err == KLeaveExit )
        {
        User::Leave( err ); 
        }

    LOGSTRING( "CMiniBrowser::DoTimeOut - done" );
    }

void CMiniBrowser::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent &aEvent )
    {
    LOGSTRING( "CMiniBrowser::MHFRunL" );
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }

    switch( aEvent.iStatus )
        {
        case THTTPEvent::ESubmit:
            {
            LOGSTRING( "CMiniBrowser::MHFRunL:ESubmit" );
            LOGTEXT( aTransaction.Request().URI().UriDes() );

            LOGSTRING( "CMiniBrowser: reset ResponseData" );
            ResetResponseDataL();
            break;
            }
        case THTTPEvent::EGotResponseHeaders:
            {
            LOGSTRING( "CMiniBrowser::MHFRunL:EGotResponseHeaders" );
            THTTPHdrVal value;
            TInt err = aTransaction.Response().GetHeaderCollection().GetField(
                iSessionStringPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() ), 0, value );

            if( err == KErrNone )
            {
                LOGSTRING("Content type:");
                const TDesC8& contentType = value.StrF().DesC();
                LOGTEXT( contentType );
                if( contentType.Compare( KNokiaHeadWrapper ) == 0 )
                    {
                    iContentType = EContentTypeNokiaHeadWrapper;
                    }
                else
                    {
                    iContentType = EContentTypeUnknown;
                    }
            }

            TCallBack callBack( TimeOutCallBack, this );
            if( iPeriodic->IsActive() )
                {
                iPeriodic->Cancel();
                }
            iPeriodic->Start( KTimeOut, KTimeOut, callBack );
            break;
            }
        case THTTPEvent::EGotResponseBodyData:
            {
            LOGSTRING( "CMiniBrowser::MHFRunL:EGotResponseBodyData" );
            MHTTPDataSupplier* pBody = aTransaction.Response().Body();
            if( pBody )
                {
                TPtrC8 dataChunk;
                pBody->GetNextDataPart( dataChunk );
                // Log
                LOGSTRING( "CMiniBrowser: MHTTPDataSupplier:" );
#ifdef _DEBUG
                for( TInt i(0); i < dataChunk.Length(); i += 128 )
                    {
                    LOGTEXT( dataChunk.Right( dataChunk.Length() - i ) );
                    }
#endif
                AppendResponseDataL( dataChunk );
                pBody->ReleaseData();
                }
            LOGSTRING( "CMiniBrowser::MHFRunL:EGotResponseBodyData - 2" );

            TCallBack callBack( TimeOutCallBack, this );
            if( iPeriodic->IsActive() )
                {
                iPeriodic->Cancel();
                }
            iPeriodic->Start( KTimeOut, KTimeOut, callBack );
            break;
            }
        case THTTPEvent::EResponseComplete:
            {
            LOGSTRING( "CMiniBrowser::MHFRunL:EResponseComplete" );
            break;
            }
        case THTTPEvent::ESucceeded:
            {
            LOGSTRING( "CMiniBrowser::MHFRunL:ESucceeded" );

            if( iContentType == EContentTypeNokiaHeadWrapper )
                {
                TRAPD( err, ParseAndSaveProvisionedSettingsL() );
                if( err != THttpProvStates::EStatusOk )
                    {
                    User::Leave( err );
                    }
                }
            else if( iContentType == EContentTypeUnknown )
                {
                TRAPD( err, ParseAndSaveProvisionedSettingsL() );
                if( err != THttpProvStates::EStatusOk )
                    {
                    User::Leave( err );
                    }
                }
            iObserver.MiniBrowserRequestDoneL();
            }
            break;
        case THTTPEvent::EFailed:
            LOGSTRING( "CMiniBrowser::MHFRunL:EFailed" );
            ResetResponseDataL();
            User::Leave( KErrGeneral );
            break;
        default:
            {
            LOGSTRING2( "CMiniBrowser::MHFRunL:event: %i", aEvent.iStatus );
            break;
            }
        }

    LOGSTRING( "CMiniBrowser::MHFRunL - done" );
    }

// currently not supported
//void CMiniBrowser::ShowMessageDialogL( const TDesC8& aMessage )
//    {
//    LOGSTRING( "ShowMessageDialogL" );
//    LOGTEXT( aMessage );
//    TInt length = aMessage.Length();
//    if( length > KMaxMessageLength )
//        {
//        length = KMaxMessageLength;
//        }
//    HBufC* message = HBufC::NewLC( length );
//    message->Des().Copy( aMessage.Left( length ) );
//
//    CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
//    CleanupStack::PushL( globalNote );
//    globalNote->SetSoftkeys( R_AVKON_SOFTKEYS_CLOSE );
//    globalNote->ShowNoteL( EAknGlobalInformationNote , *message );
//
//    LOGSTRING( "deleting globalNote" );
//    CleanupStack::PopAndDestroy( globalNote );
//    CleanupStack::PopAndDestroy( message );
//    LOGSTRING( "ShowMessageDialogL - done" );
//    }


void CMiniBrowser::ParseAndSaveProvisionedSettingsL()
    {
    LOGSTRING( "CMiniBrowser::ParseAndSaveProvisionedSettingsL - begin" );
    if( !iResponseData )
        {
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - 2" );
    CNHeadWrapperParser* NHwrParser = CNHeadWrapperParser::NewL( *iResponseData );
    CleanupStack::PushL( NHwrParser );
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - 3" );
    iProvisioningStatus = NHwrParser->Parse();
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - 4" );
    if( iProvisioningStatus != THttpProvStates::EStatusOk )
        {
        LOGSTRING2( "status from parse:%i", iProvisioningStatus ); 
        // Try to get report url anyways, this should succeed at leat in case
        // Nokia HeadWrapper authentication failed while parsing an otherwise valid
        // HeadWrapper document
        GetReportUrlL( *NHwrParser );
        User::Leave( iProvisioningStatus );
        }
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - 5" );
    GetReportUrlL( *NHwrParser );
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - 6" );
    // Get provisioned data
    TPtrC8 content;
    TRAPD( err, content.Set( NHwrParser->GetContentL() ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "err from GetContentL:%i", err );
        iProvisioningStatus = THttpProvStates::EStatusDocumentParsingFailed;
        User::Leave( err );
        }

    LOGSTRING( "Response parsed" ); 

    TPtrC8 contentType;
    TRAP( err, contentType.Set( NHwrParser->GetContentTypeL() ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "err from GetContentTypeL:%i", err );
        iProvisioningStatus = THttpProvStates::EStatusDocumentParsingFailed;
        User::Leave( err );
        }

    if( contentType.Compare( KContentTypeApplicationWapConnectivityWbxml ) == 0 )
        {
        LOGSTRING( "CMiniBrowser: content type application/vnd.wap.connectivity-wbxml" );
        TRAP( err, SaveProvisionedSettingsL( content ) );
        iProvisioningStatus = (THttpProvStates::TProvisioningStatus) err;
        if( iProvisioningStatus != THttpProvStates::EStatusOk )
            {
            LOGSTRING2( "ProvisioningStatus from SaveProvisionedSettingsL:%i", iProvisioningStatus ); 
            User::Leave( iProvisioningStatus );
            }
        }
// currently not supported
//    else if( contentType.Compare( KContentTypeTextPlain ) == 0 )
//        {
//        LOGSTRING( "CMiniBrowser: content type text/plain" );
//        ShowMessageDialogL( content );
//        }
    else
        {
        LOGSTRING( "CMiniBrowser: Unknown content type" );
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    CleanupStack::PopAndDestroy( NHwrParser );
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL - done" );
    }

void CMiniBrowser::GetReportUrlL( const CNHeadWrapperParser& aNHwrParser )
    {
    LOGSTRING( "CMiniBrowser::GetReportUrlL - begin" );
    // Store report URL for later reference
    const TDesC8& reportUrl = aNHwrParser.GetReportUrlL();
    if( iReportUrl->Des().MaxLength() < reportUrl.Length() )
        {
        delete iReportUrl;
        iReportUrl = 0;
        iReportUrl = HBufC8::NewL( reportUrl.Length() );
        }
    TPtr8 reportUrlPtr = iReportUrl->Des();
    reportUrlPtr.Copy( reportUrl );
    LOGTEXT( *iReportUrl );
    LOGSTRING( "CMiniBrowser::GetReportUrlL - end" );
    }

HBufC* CMiniBrowser::GetReportUrlLC()
    {
    LOGSTRING( "CMiniBrowser::GetReportUrlLC - begin" );
    if( iReportUrl && iReportUrl->Length() )
        {
        HBufC* url = HBufC::NewLC(
            iReportUrl->Length() +
            KAmpersand().Length() +
            KQuestionMark().Length() +
            KStatus().Length() );

        TPtr urlPtr = url->Des();
        urlPtr.Copy( *iReportUrl );

        // Add provisioning status to the url if known
        if( iProvisioningStatus != THttpProvStates::EStatusUnknown )
            {
            // Assume there might already be parameters in the URL given
            if( urlPtr.Find( KQuestionMark ) != KErrNotFound )
                {
                urlPtr.Append( KAmpersand );
                }
            else
                {
                urlPtr.Append( KQuestionMark );
                }
            urlPtr.Append( KStatus );
            urlPtr.AppendNum( (TInt) iProvisioningStatus );
            }
        LOGSTRING( "CMiniBrowser::GetReportUrlLC - end" );
        return url;
        }
    else
        {
        LOGSTRING( "CMiniBrowser::GetReportUrlLC Leave KErrNotFound" );
        User::Leave( KErrNotFound );
        }
    return NULL; // Not possible to end up here
    }

void CMiniBrowser::SaveProvisionedSettingsL( const TDesC8& aContent )
    {
    LOGSTRING( "CMiniBrowser::SaveProvisionedSettingsL" );
    TInt status = THttpProvStates::EStatusOk;
    // Create WPEngine
    CWPEngine* wpengine = CWPEngine::NewL();
    CleanupStack::PushL( wpengine );

    LOGSTRING( "wpengine->ImportDocumentL" );   
    TRAPD( err, wpengine->ImportDocumentL( aContent ) );
    User::LeaveIfError( err );

    LOGSTRING( "wpengine->PopulateL" );
    TRAP( err, wpengine->PopulateL() );
    if( err != KErrNone )
        {
        LOGSTRING2( "PopulateL err: %i", err );
        User::Leave( err );
        }
    LOGSTRING( "wpengine->PopulateL success" );

    TInt itemCount = wpengine->ItemCount();
    // Empty messages are not supported
    if( itemCount == 0 )
        {
        LOGSTRING("No items in provisioning message!");
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    for( TInt i(0); i < itemCount; i++ )
        {
        LOGSTRING( "Saving" );
        TRAP( err, wpengine->SaveL( i ) );
        if( wpengine->CanSetAsDefault( i ) )
            {
            LOGSTRING( "Setting as default" );
            TRAP( err, wpengine->SetAsDefaultL( i ) );
            }
        else
            {
            LOGSTRING( "Cannot set as default" );
            }
        if( err != KErrNone )
            {
            LOGSTRING2( "Error while saving: %i", err );
            status = THttpProvStates::EStatusStorageFailed;
            }
        }

    if( status != THttpProvStates::EStatusOk )
        {
        User::Leave( status );
        }
    }

TInt CMiniBrowser::MHFRunError( TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/ )
    {
    LOGSTRING2( "CMiniBrowser::MHFRunError %i", aError );
    iTransaction.Close();
    TRAPD( err, iObserver.MiniBrowserErrorL( aError ) );
    if( err != KErrNone )
        {
        iObserver.MiniBrowserUnhandledLeave( err );
        }
    LOGSTRING( "CMiniBrowser::MHFRunError - done" );
    if( err == KLeaveExit )
        {
        return err;
        }
    return KErrNone;
    }


void CMiniBrowser::AppendResponseDataL( const TDesC8& aData )
    {
    LOGSTRING( "CMiniBrowser::AppendResponseDataL()" );
    if( ( iResponseData->Des().MaxLength() - iResponseData->Length() ) < aData.Length() )
        {
        // ReAlloc
        HBufC8* tempBuf = HBufC8::NewL( iResponseData->Length() + aData.Length() );
        TPtr8 tempBufPtr = tempBuf->Des();
        tempBufPtr.Append( *iResponseData );
        delete iResponseData;
        iResponseData = tempBuf; 
        }

    TPtr8 ptr = iResponseData->Des();
    ptr.Append( aData );
    LOGSTRING( "CMiniBrowser::AppendResponseDataL() - done" );
    }

void CMiniBrowser::ResetResponseDataL()
    {
    LOGSTRING( "CMiniBrowser::ResetResponseDataL()" );
    if( iResponseData && iResponseData->Length() )
        {
        delete iResponseData;
        iResponseData = 0;
        iResponseData = HBufC8::NewL( KInitialDataBufferSize );
        }
    if( iReportUrl && iReportUrl->Length() )
        {
        delete iReportUrl;
        iReportUrl = 0;
        iReportUrl = HBufC8::NewL( KInitialReportUrlBufferSize );
        }
    iContentType = EContentTypeUnknown;
    iProvisioningStatus = THttpProvStates::EStatusUnknown;
    LOGSTRING( "CMiniBrowser::ResetResponseDataL() - done" );
    }

TBool CMiniBrowser::GetNextDataPart( TPtrC8 &aDataPart )
    {
    _LIT8( KEmpty, "");
    aDataPart.Set( KEmpty() );
    return ETrue;
    }

void CMiniBrowser::ReleaseData()
    {
    }

TInt CMiniBrowser::OverallDataSize()
    {
    return KErrNotFound;
    }

TInt CMiniBrowser::Reset()
    {
    return KErrNone;
    }
