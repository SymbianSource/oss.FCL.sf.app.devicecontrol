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
* Description:  PnP Paos filter implementation
*
*/


#include <e32std.h>
#include <http.h>
#include <apgcli.h>             // for RApaLsSession
#include <apacmdln.h>           // for CApaCommandLine
#include <centralrepository.h>  // link against centralrepository.lib

#include "PnpPaosFilter.h"
#include "PnpPaosLogger.h"
#include "PnpPaosXml.h"
#include "PnpPaosFilterPrivateCRKeys.h"



_LIT8( KPaosContentType, "application/vnd.paos+xml" );
_LIT8( KPnpPaosFilterName, "PnpPaosFilter" );
_LIT8( KPaosHeaderName, "PAOS" );
_LIT8( KPaosHeader, "ver=\"urn:liberty:paos:2003-08\"; urn:\"http://pnpms.nokia.com/signkey\"" );
_LIT8( KDummyResponse, "<!-- Dummy -->" );
_LIT8( KHttp, "http://" );
_LIT8( KHttps, "https://" );

CEComFilter* CPnpPaosFilter::InstantiateL( TAny* aSession )
    {
    LOGSTRING("CPnpPaosFilter::InstantiateL()");
    // Cast TAny to RHTTPSession
    RHTTPSession* pSession = static_cast<RHTTPSession*>(aSession);
    CPnpPaosFilter* filter = CPnpPaosFilter::NewL( pSession );
    LOGSTRING("CPnpPaosFilter::InstantiateL() - done");
    return filter;
    }

CPnpPaosFilter* CPnpPaosFilter::NewL( RHTTPSession* pSession )
    {
    LOGSTRING("CPnpPaosFilter::NewL()");
    CPnpPaosFilter* pNew = new (ELeave) CPnpPaosFilter( pSession);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    CleanupStack::Pop();
    LOGSTRING("CPnpPaosFilter::NewL() - done");
    return pNew;
    }

CPnpPaosFilter::CPnpPaosFilter( RHTTPSession* pSession ) :
    iTransactionId(-1),
    iSession( pSession ),
    iMakePaosResponse( EFalse )
#ifndef __SERIES60_ 
    ,iHdcPaosPostDone( EFalse )
#endif
    {
    LOGSTRING("constructor CPnpPaosFilter()");
    }

void CPnpPaosFilter::ReadAllowedDomainsL()
    {
    LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL()");
    RLibrary library;
    const TUidType hdcuid( KNullUid, KNullUid, KHdcUtilDllUid );
    TInt result = library.Load( KHdcDllFileName, hdcuid );
    LOGSTRING2( "Hdc Library load result: %i", result );
    // If there is no HDC installed, cannot add allowed HDC domains
    if( result == KErrNone )
        {
        CleanupClosePushL( library );
        LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL() 2");
        // Function at ordinal 1 is NewLC
        TLibraryFunction entry = library.Lookup(1);
        // Call the function to create new hdc dll object
        LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL() 3");
        CHdcToPaosInterface* hdcUtil = ( CHdcToPaosInterface* ) entry();
        hdcUtil->TrustedDomainsL( iTrustedHdcDomains );
        LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL() 4");
        CleanupStack::PopAndDestroy( hdcUtil );
        CleanupStack::PopAndDestroy(); // library.Close()
        LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL() 5");
        }

    // PnP trusted domains could be handled as in HDC case, by maintaining
    // a list of trusted domains as browser bookmarks (changeable only 
    // in a Device Managament session, not by the user)
    //AllowedDomainsArray pnpDomains = pnpUtil->AllowedDomainsL();
    LOGSTRING("CPnpPaosFilter::ReadAllowedDomainsL() - done");
    }
    
void CPnpPaosFilter::ReadAllowedPnpDomainsL()
    {
    LOGSTRING("CPnpPaosFilter::ReadAllowedPnpDomainsL()");
    // Create PnP-MS home domain filter
    // Read allowed pnp domains from cenrep
    CRepository* repository = CRepository::NewLC( KCRUidPnpPaosFilter );
    
    TBuf<KMaxURLLength> buffer;
    TUint32 domainNbr;
    TrustedDomain domain;
    for (domainNbr = KPnPPaosFilterHomeDomain1; domainNbr <= KPnPPaosFilterHomeDomainLast; domainNbr++)
        {
        User::LeaveIfError( repository->Get( domainNbr, buffer ) );
        if(buffer.Length() > 0)
            {
            domain.Copy( buffer );
            iTrustedPnpDomains.Append( domain );
            }
        }
    CleanupStack::PopAndDestroy( repository );
    LOGSTRING("CPnpPaosFilter::ReadAllowedPnpDomainsL() - done");
    }

void CPnpPaosFilter::ConstructL()
    {
    LOGSTRING("CPnpPaosFilter::ConstructL()");

    iSessionStringPool = iSession->StringPool();
    iFilterName = iSessionStringPool.OpenFStringL( KPnpPaosFilterName );
    TInt err( KErrNone );
    TRAP( err, iPnpPaosXml = CPnpPaosXml::NewL() );
    LOGSTRING2( "iPnpPaosXml err: %i", err );

    TRAP( err, ReadAllowedDomainsL() );
    if( err != KErrNone )
        {
        LOGSTRING2( "Could not read allowed domains for HelpDeskConnect PAOS: %i", err );
        }
        
    TRAP( err, ReadAllowedPnpDomainsL() );
    if( err != KErrNone )
        {
        LOGSTRING2( "Could not read allowed domains for PnP PAOS: %i", err );
        }


    LOGSTRING("CPnpPaosFilter::ConstructL() add filters");

    // Add the filter to the queue and register for selected incoming and
    // outgoing events.
    iSession->FilterCollection().AddFilterL(
        *this,
        THTTPEvent::ESubmit,              // Transaction event
        RStringF(),                       // Any header field
        KAnyStatusCode,                   // HTTP status code
        MHTTPFilter::EClientFilters,      // Position in filter chain
        //MHTTPFilter::EProtocolHandler,      // Position in filter chain
        iFilterName );                    // Filter name

    iSession->FilterCollection().AddFilterL(
        *this,
        THTTPEvent::EGotResponseHeaders,
        RStringF(),
        200,
        MHTTPFilter::EClientFilters,      // Position in filter chain
        //MHTTPFilter::EProtocolHandler,      // Position in filter chain
        iFilterName );

    iSession->FilterCollection().AddFilterL(
        *this,
        THTTPEvent::EGotResponseBodyData,
        RStringF(),
        200,
        MHTTPFilter::EClientFilters,      // Position in filter chain
        //MHTTPFilter::EProtocolHandler,      // Position in filter chain
        iFilterName );

    iSession->FilterCollection().AddFilterL(
        *this,
        THTTPEvent::EResponseComplete,
        RStringF(),
        200,
        MHTTPFilter::EClientFilters,      // Position in filter chain
        //MHTTPFilter::EProtocolHandler,      // Position in filter chain
        iFilterName );

    LOGSTRING("CPnpPaosFilter::ConstructL() - done");
    }

CPnpPaosFilter::~CPnpPaosFilter()
    {
    LOGSTRING("CPnpPaosFilter::~CPnpPaosFilter()");

    if( iLoadCount )
        {
        // As we're already in a destructor, MHFUnload must not delete us again
        iLoadCount = -1;
        if (iSession)
            {
            // Removes all registrations of this filter:
            iSession->FilterCollection().RemoveFilter( iFilterName );
            }
        }
    iFilterName.Close();
    iTrustedPnpDomains.Close();

    delete iPaosPostUrl;
    delete iPnpPaosXml;

    // RArray must be closed before destructing
    iTrustedHdcDomains.Close();

    // we do not own iSessionStringPool, we do not close it.
    // we do not own iSession

    LOGSTRING("CPnpPaosFilter::~CPnpPaosFilter() - done");
    }

void CPnpPaosFilter::HostFromUriL( CUri8* aUri )
    {
    LOGSTRING( "Original URI:" );
    LOGTEXT( aUri->Uri().UriDes() );

    /** The userinfo component specifier */
    aUri->RemoveComponentL( EUriUserinfo );
    LOGTEXT( aUri->Uri().UriDes() );
    /** The path component specifier */
    aUri->RemoveComponentL( EUriPath );
    LOGTEXT( aUri->Uri().UriDes() );
    /** The query component specifier */
    aUri->RemoveComponentL( EUriQuery );
    LOGTEXT( aUri->Uri().UriDes() );
    /** The fragment component specifier */
    aUri->RemoveComponentL( EUriFragment );
    LOGTEXT( aUri->Uri().UriDes() );

    // Only EUriHost (The host component specifier) and
    // EUriPort (The port component specifier) are left
    }

void CPnpPaosFilter::MHFRunL( RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent)
    {
    LOGSTRING3( "CPnpPaosFilter::MHFRunL( TxnId: %d, event: %d )", aTransaction.Id(), aEvent.iStatus );
    switch( aEvent.iStatus )
        {
        case THTTPEvent::ESubmit:
            {
            LOGSTRING( "CPnpPaosFilter::MHFRunL:ESubmit");

            // Generate as little overhead as possible:

            // If this is the case maybe PAOS requests should be allowed only from certain URLs...
            // Now only nokia.com or help-portal.com are allowed URLs
            CUri8* uri = CUri8::NewLC( aTransaction.Request().URI() );
            HostFromUriL( uri );
            TPtrC8 host = uri->Uri().UriDes();

            // Check if a PAOS query is allowed from the domain
            
            if( IsPaosHomeDomainL( host ) )
                {
                LOGSTRING("PnP Mobile Services Allows PAOS-requests from the domain");
                RHTTPHeaders headers = aTransaction.Request().GetHeaderCollection();
                AddPaosHeadersL( headers );
                // From HTTP stack documentation:
                // It's important to note that a filter object is per-session, and so might be shared by several 
                // transactions. This means that if you have per-transaction state that you need to store, it 
                // must be stored in the transaction's property set. DO NOT STORE ANY PER-TRANSACTION INFORMATION 
                // IN THE FILTER OBJECT. 

                iMakePaosResponse = ETrue;
                }
            else
                {		    				
                if( host.Size() <= 255) 	   
                    { 
                    if( iTrustedHdcDomains.Find( host ) != KErrNotFound )
                        {
                        LOGSTRING("HelpDeskConnect Allows PAOS-requests from the domain");
                        RHTTPHeaders headers = aTransaction.Request().GetHeaderCollection();
                        AddPaosHeadersL( headers );
                        iMakePaosResponse = ETrue;
                        }
                    else
                        {
                        iMakePaosResponse = EFalse;
                        }
                    }
                } 
            CleanupStack::PopAndDestroy( uri );
            break;
            }
        case THTTPEvent::EGotResponseHeaders:
            {
            LOGSTRING( "CPnpPaosFilter::MHFRunL:EGotResponseHeaders" );

            if( !iMakePaosResponse ) break;

            RHTTPHeaders headers = aTransaction.Response().GetHeaderCollection();
            if( IsPaosContentTypeL( headers ) )
                {
                iTransactionId = aTransaction.Id();

                delete iPaosPostUrl;
                iPaosPostUrl = 0;
                iPaosPostUrl = CUri8::NewL( aTransaction.Request().URI() );
                HostFromUriL( iPaosPostUrl );
                }
            else
                {
                iMakePaosResponse = EFalse;
                }
            break;
            }
        case THTTPEvent::EGotResponseBodyData:
            {
            LOGSTRING( "CPnpPaosFilter::MHFRunL:EGotResponseBodyData" );

            // Check if we are allowed to make a PAOS response
            if( iMakePaosResponse && aTransaction.Id() == iTransactionId )
                {
                if( !iPnpPaosXml ) 
                    {
                    User::Leave( KErrGeneral );
                    }
                TBool lastPart( EFalse );
                // Received another body data chunk
                MHTTPDataSupplier* pBody = aTransaction.Response().Body();
                if( pBody )
                    {
                    lastPart = iPnpPaosXml->CollectResponseBodyL( *pBody );
                    }

                // check if more data is expected
                if( lastPart )
                    {
                    PostPaosResponseL( aTransaction );
                    }
                }
#ifndef __SERIES60_ 
            // check if expecting a HDC trigger file
            else if( iHdcPaosPostDone )
                {
                // Received another body data chunk
                MHTTPDataSupplier* pBody = aTransaction.Response().Body();
                if( pBody )
                    {
                    iPnpPaosXml->CollectResponseBodyL( *pBody );
                    }
                }
#endif
            break;
            }
        case THTTPEvent::EResponseComplete:
            {
            LOGSTRING( "CPnpPaosFilter::MHFRunL:EResponseComplete" );

#ifndef __SERIES60_ 
            // if( contenttype == trigger )
            if( iHdcPaosPostDone )
                {
                HandleHdcTriggerL();
                aTransaction.Cancel( THTTPFilterHandle::ECurrentFilter );
                }
#endif
            break;
            }
        default:
            {
            // We ignore other events. We shouldn't receive other events though.
            break;
            }
        }
    LOGSTRING( "CPnpPaosFilter::MHFRunL - done" );
    }


void CPnpPaosFilter::PostPaosResponseL( RHTTPTransaction& aTransaction )
    {
    // For some mysterious reason the http transaction has to be cancelled here (EGotResponseBodyData),
    // otherwise browser will have the PAOS request concatenated with the resulting
    // html page (or at least the resulting html page shows an extra text "setOfKeys"
    // so it seems to concatenate at least part of the PAOS request)

    // Since transaction is cancelled in EGotResponseBodyData event, there is no
    // point in listening for EResponseComplete event anymore

    CPnpPaosXml::TPaosStates status( CPnpPaosXml::EPaosStatusUnknown );

    TRAPD( err, iPnpPaosXml->ParseL( status ) );
    if( err == KErrNone )
        {
        iMakePaosResponse = EFalse;
        iTransactionId = -1;
        LOGSTRING( "PAOS post URL:" );
        /*
        "responseConsumerURL attribute, with a URL as its value.This URL SHOULD be relative to the URL that
        was requested by the user agent (in the HTTP request that resulted in the creation of the SOAP message). If the
        URL nevertheless is absolute it MUST have http or https as the protocol and SHOULD have a domain that is
        owned by the same party as the owner of the domain in the URL of the HTTP request."
        [Liberty Reverse HTTP binding for SOAP Specification]
        */
        const TDesC8& paosPostUrl = iPnpPaosXml->GetPaosPostUrlL();
        if( paosPostUrl.Find( KHttp ) != KErrNotFound || paosPostUrl.Find( KHttps ) != KErrNotFound )
            {
            LOGSTRING( "Absolute URL" );
            delete iPaosPostUrl;
            iPaosPostUrl = 0;
            TUriParser8 uriParser;
            User::LeaveIfError( uriParser.Parse( paosPostUrl ) );
            iPaosPostUrl = CUri8::NewL( uriParser );
            }
        else
            {
            LOGSTRING( "Relative URL" );
            iPaosPostUrl->SetComponentL( paosPostUrl, EUriPath );
            }
        LOGTEXT( iPaosPostUrl->Uri().UriDes() );

        switch( status )
            {
        case CPnpPaosXml::EPaosStatusUnknown:
            LOGSTRING( "EPaosStatusUnknown" );
            // Do nothing, was: User::Leave( KErrArgument );
            break;
        case CPnpPaosXml::EPaosStatusRequestingPnPKeys:
            {
            LOGSTRING( "EPaosStatusRequestingPnPKeys" );
            CUri8* uri = CUri8::NewLC( aTransaction.Request().URI() );
            HostFromUriL( uri );
            // Also remove scheme
            uri->RemoveComponentL( EUriScheme );
            LOGTEXT( uri->Uri().UriDes() );

            TPtrC8 host = uri->Uri().UriDes();
            // check the domain was an allowed domain of PnP-MS home domain filter
            if( IsPaosHomeDomainL( host ) )
                {
                PaosPostL( aTransaction );
                }
            CleanupStack::PopAndDestroy( uri );
            break;
            }
        case CPnpPaosXml::EPaosStatusRequestingHdcKeys:
            {
            LOGSTRING( "EPaosStatusRequestingHdcKeys" );
            CUri8* uri = CUri8::NewLC( aTransaction.Request().URI() );
            HostFromUriL( uri );
            TPtrC8 host = uri->Uri().UriDes();
            
            TBool isTrusted( EFalse );
            for( TInt i(0); i < iTrustedHdcDomains.Count(); i++ )
                {
                HdcTrustedDomain trustedDomain = iTrustedHdcDomains[i];
                if( host.Find( trustedDomain ) != KErrNotFound )
                    isTrusted = ETrue;
                }
            // check the domain was an allowed domain of HDC home domain filter
            if( isTrusted )
                {
                PaosPostL( aTransaction );
#ifndef __SERIES60_ 
                iHdcPaosPostDone = ETrue;
#endif
                }
#ifdef _DEBUG
            else
                {
                LOGSTRING("Not an allowed domain!");
                LOGTEXT( host );
                LOGSTRING("domains:");
                for( TInt j(0); j < iTrustedHdcDomains.Count(); j++ )
                    {
                    LOGTEXT( iTrustedHdcDomains[j] );
                    }
                }
#endif
            CleanupStack::PopAndDestroy( uri );
            break;
            }
        default:
            {
            // We ignore other events. We shouldn't receive other events though.
            break;
            }
            }
        }
    else
        {
        LOGSTRING2( "Error in ParseL: %i", err );
        }

    LOGSTRING("iPnpPaosXml->ResetPaosRequest()");
    iPnpPaosXml->ResetPaosRequest();
    }

#ifndef __SERIES60_ 
void CPnpPaosFilter::HandleHdcTriggerL()
    {
    LOGSTRING("Create file");
    iHdcPaosPostDone = EFalse;

    _LIT( KTempDocumentName, "C:\\Temp\\trigger.trg" );

    RFs rfs;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );
    TInt err = rfs.MkDir( KTempDocumentName );
    // The folder may already exist, do not leave in that case
    if( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err ); 
        }

    RFile file;
    User::LeaveIfError( file.Replace( rfs, KTempDocumentName, EFileWrite ) );
    CleanupClosePushL( file );
    LOGSTRING("File created");
    // A HDC PAOS post was made and we expect the response to be a HDC trigger file
    const TPtrC8 hdcTriggerData = iPnpPaosXml->ResponseBodyL();
    file.Write( hdcTriggerData );
    LOGSTRING("File written");
    iPnpPaosXml->ResetPaosRequest();

    CleanupStack::PopAndDestroy(); // rfs
    CleanupStack::PopAndDestroy(); // file

    LOGSTRING("Starting app");
    RApaLsSession appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL( appArcSession );

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    TApaAppInfo info;
    const TUid KHdcUid = { 0x1020433F };
    User::LeaveIfError( appArcSession.GetAppInfo( info, KHdcUid ) );
#ifdef RD_APPS_TO_EXES
    cmdLine->SetExecutableNameL( info.iFullName );
#else
    cmdLine->SetLibraryNameL( info.iFullName );
#endif
    cmdLine->SetCommandL( EApaCommandOpen );
    cmdLine->SetDocumentNameL( KTempDocumentName );
    err = appArcSession.StartApp( *cmdLine );
    if( err != KErrNone )
        {
        LOGSTRING2( "StartApp err %i", err );
        User::Leave( err );
        }
    LOGSTRING("Cancel transaction");
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy(); // appArcSession
    LOGSTRING("done");
    }
#endif 


TBool CPnpPaosFilter::GetNextDataPart( TPtrC8& aDataPart )
    {
    LOGSTRING("CPnpPaosFilter::GetNextDataPart");
    aDataPart.Set( KDummyResponse );
    return ETrue;
    }

void CPnpPaosFilter::ReleaseData()
    {
    LOGSTRING("CPnpPaosFilter::ReleaseData");
    }

TInt CPnpPaosFilter::OverallDataSize()
    {
    LOGSTRING("CPnpPaosFilter::OverallDataSize");
    return KDummyResponse().Length();
    }

TInt CPnpPaosFilter::Reset()
    {
    LOGSTRING("CPnpPaosFilter::Reset");
    return KErrNone;
    }

void CPnpPaosFilter::AddPaosHeadersL( RHTTPHeaders& requestHeaders )
    {
    LOGSTRING( "CPnpPaosFilter::AddPaosHeadersL" );

    // Add PAOS header
    THTTPHdrVal paosVer;
    RStringF paosVerStr = iSessionStringPool.OpenFStringL( KPaosHeader );
    CleanupClosePushL( paosVerStr );
    paosVer.SetStrF( paosVerStr );
    RStringF paosStr = iSessionStringPool.OpenFStringL( KPaosHeaderName );
    CleanupClosePushL( paosStr );
    requestHeaders.SetFieldL( paosStr, paosVerStr );
    CleanupStack::PopAndDestroy(); // paosStr.Close()
    CleanupStack::PopAndDestroy(); // paosVerStr.Close()

    // Add PAOS content type to accept header
    THTTPHdrVal acceptHdr;
    requestHeaders.GetField(
        iSessionStringPool.StringF( HTTP::EAccept, RHTTPSession::GetTable() ), 0, acceptHdr );

    if (acceptHdr.Type() == 0x04) //Worst case scenario, if transaction does not have accept header.
    {
        RStringF valStr = iSessionStringPool.OpenFStringL(_L8("*/*"));
   		THTTPHdrVal val(valStr);
    	requestHeaders.SetFieldL(iSessionStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable() ), val);
    	valStr.Close();
   	    requestHeaders.GetField(
 	       iSessionStringPool.StringF( HTTP::EAccept, RHTTPSession::GetTable() ), 0, acceptHdr );

    }


    RStringF acceptStr = acceptHdr.StrF();
    TPtrC8 accept = acceptStr.DesC();
    if( accept.Find( KPaosContentType ) == KErrNotFound )
        {
        HBufC8* acceptBuf = HBufC8::NewLC( KPaosContentType().Length() );
        TPtr8 acceptBufPtr = acceptBuf->Des();
        acceptBufPtr.Append( KPaosContentType );
        SetHttpHeaderL( requestHeaders, HTTP::EAccept, *acceptBuf );

#ifdef LOGGING_ENABLED
        LOGSTRING( "Accept header:" );
            for( TInt i(0); i < acceptBufPtr.Length(); i += 128 )
                {
                TPtrC8 logText = acceptBufPtr.Right( acceptBufPtr.Length() - i );
                LOGTEXT( logText );
                }
#endif
        CleanupStack::PopAndDestroy( acceptBuf );
        }
#ifdef LOGGING_ENABLED
    else
        {
        LOGSTRING( "Accept header already includes PAOS:" );
        for( TInt i(0); i < accept.Length(); i += 128 )
            {
            TPtrC8 logText = accept.Right( accept.Length() - i );
            LOGTEXT( logText );
            }
        }
#endif

    LOGSTRING( "CPnpPaosFilter::AddPaosHeadersL - done" );
    }

TBool CPnpPaosFilter::IsPaosContentTypeL( RHTTPHeaders& aHeaders )
    {
    LOGSTRING( "CPnpPaosFilter::IsPaosContentTypeL" );

    // get the Content-Type string
    RStringF content = iSessionStringPool.StringF( HTTP::EContentType,
                                            RHTTPSession::GetTable() );

    THTTPHdrVal fieldVal;
    // now retrieve the Content-Type field
    if( KErrNone == aHeaders.GetField( content, 0, fieldVal ) )
        {
        TPtrC8 contentType;
        // get the field value
        if( THTTPHdrVal::KStrFVal == fieldVal.Type() )
            {
            contentType.Set( fieldVal.StrF().DesC() );
            LOGSTRING( "ContentType:" );
            LOGTEXT( contentType );
            }
        else if( THTTPHdrVal::KStrVal == fieldVal.Type() )
            {
            contentType.Set( fieldVal.Str().DesC() );
            LOGSTRING( "ContentType:" );
            LOGTEXT( contentType );
            }
        else
            {
            return EFalse;
            }

        if( contentType.Compare( KPaosContentType ) == 0 )
            {
            LOGSTRING("Content type matches");

            // 3.0 does not handle the resulting html right, it is stored by DL manager.
            // This means that it does not recognize the incoming text/html data correctly.
            // So try replacing content type field with text/html, maybe PAOS content type is
            // cached somewhere even if the transaction has been canceled.
            LOGSTRING("Resetting Content type to text/html");
            aHeaders.RemoveField( iSessionStringPool.StringF( HTTP::EContentType , RHTTPSession::GetTable() ) );
            SetHttpHeaderL( aHeaders, HTTP::EContentType, _L8("text/html") );

            return ETrue;
            }
        }
    return EFalse;
    }

void CPnpPaosFilter::PaosPostL( RHTTPTransaction& aTransaction )
    {
    LOGSTRING( "CPnpPaosFilter::PaosPostL" );

    RHTTPRequest request = aTransaction.Request();

    aTransaction.Cancel( THTTPFilterHandle::ECurrentFilter );
    RHTTPHeaders requestHeaders = request.GetHeaderCollection();
    request.RemoveBody();
    // Remove Content-Type header
    requestHeaders.RemoveField( iSessionStringPool.StringF( HTTP::EContentType, RHTTPSession::GetTable() ) );
    // Remove Content-Length header
    requestHeaders.RemoveField( iSessionStringPool.StringF( HTTP::EContentLength, RHTTPSession::GetTable() ) );
    // Remove Host header
    requestHeaders.RemoveField( iSessionStringPool.StringF( HTTP::EHost, RHTTPSession::GetTable() ) );

    // PAOS header
    THTTPHdrVal paosVer;
    RStringF paosVerStr = iSessionStringPool.OpenFStringL( KPaosHeader );
    CleanupClosePushL( paosVerStr );
    paosVer.SetStrF( paosVerStr );
    RStringF paosStr = iSessionStringPool.OpenFStringL( KPaosHeaderName );
    CleanupClosePushL( paosStr );
    requestHeaders.SetFieldL( paosStr, paosVerStr );
    CleanupStack::PopAndDestroy(); // paosStr
    CleanupStack::PopAndDestroy(); // paosVerStr

    // Content headers
    SetHttpHeaderL( requestHeaders, HTTP::EContentType, _L8("application/vnd.paos+xml") );

    request.SetMethod( iSessionStringPool.StringF( HTTP::EPOST, RHTTPSession::GetTable() ) );

    // Set the URI of the request
    request.SetURIL( iPaosPostUrl->Uri() );

    LOGSTRING( "Uri:" );
    LOGTEXT( iPaosPostUrl->Uri().UriDes() );

    // Provide Response
    if( !iPnpPaosXml )
        {
        User::Leave( KErrGeneral );
        }
    request.SetBody( *iPnpPaosXml );

    aTransaction.Cancel();
    aTransaction.SubmitL();
    LOGSTRING( "CPnpPaosFilter::PaosPostL - done" );
    }

void CPnpPaosFilter::SetHttpHeaderL( RHTTPHeaders& aMessage, const HTTP::TStrings aIndex, const TDesC8& aString )
    {
    LOGSTRING( "CPnpPaosFilter::SetHttpHeaderL" );
    
    THTTPHdrVal hdrVal;
    RStringF str = iSessionStringPool.OpenFStringL( aString );
    CleanupClosePushL( str );
    hdrVal.SetStrF( str );
    aMessage.SetFieldL( iSessionStringPool.StringF( aIndex, RHTTPSession::GetTable() ), hdrVal );
    CleanupStack::PopAndDestroy();
    
    LOGSTRING( "CPnpPaosFilter::SetHttpHeaderL - done" );
    }

TInt CPnpPaosFilter::MHFRunError( TInt aError,
                                 RHTTPTransaction aTransaction,
                                 const THTTPEvent& aEvent )
    {
    LOGSTRING( "CPnpPaosFilter::MHFRunError()" );

    LOGSTRING3( "error: %d, event: %d", aError, aEvent.iStatus );
    // If anything left, we've run out of memory or something
    // similarly catastrophic has gone wrong.
    // Remove the body to prevent other client from accessing
    // the contents.
    aTransaction.Response().RemoveBody();
    // Set the transaction to failed
    aTransaction.Fail();
    // Keep compiler happy
    (void)aError;
    (void)aEvent;
    LOGSTRING( "CPnpPaosFilter::MHFRunError() - done" );
    return KErrNone;
    }

void CPnpPaosFilter::MHFLoad(RHTTPSession /*aSession*/,
                                         THTTPFilterHandle /*aHandle*/)
    {
    LOGSTRING("CPnpPaosFilter::MHFLoad");
    iLoadCount++;
    }

void CPnpPaosFilter::MHFUnload(RHTTPSession /*aSession*/,
                                           THTTPFilterHandle /*aHandle*/)
    {
    LOGSTRING("CPnpPaosFilter::MHFUnload");
    if( --iLoadCount > 0 )
        {
        LOGSTRING("CPnpPaosFilter::MHFUnload - done");
        return;
        }
    delete this; // Delete object itself
    LOGSTRING("CPnpPaosFilter::MHFUnload - done");
    }

TBool CPnpPaosFilter::IsPaosHomeDomainL( const TPtrC8 aHost )
    {
    LOGSTRING("CPnpPaosFilter::IsPaosHomeDomainL");
    // check is the host in trusted domains list
    for (TInt i=0; i < iTrustedPnpDomains.Count(); i++)
        {
        if (aHost.Find(iTrustedPnpDomains[i]) != KErrNotFound )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
