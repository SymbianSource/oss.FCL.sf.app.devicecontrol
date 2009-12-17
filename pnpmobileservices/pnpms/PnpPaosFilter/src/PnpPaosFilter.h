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
* Description:  PnP Paos filter definitions
*
*/



#ifndef WSPAOSFILTER_H
#define WSPAOSFILTER_H


#include <flogger.h>
//#include <RXMLReader.h>
#include <http/mhttpfilter.h>
#include <http/cecomfilter.h>
#include <http/mhttptransactioncallback.h>
#include <httpstringconstants.h>
#include <http/mhttpdatasupplier.h>    // for MHTTPDataSupplier
//#include <HDCUtil.h>                // for HdcTrustedDomainList
#include "HdcToPaosInterface.h"                // for HdcTrustedDomainList

//const TInt KMaxURLLength(255);
typedef TBuf8<KMaxURLLength> TrustedDomain;
typedef RArray<TrustedDomain> PaosTrustedDomainList;

class CEComFilter;
class CPnpPaosXml;

/**
PnP Mobile Services PAOS filter class. Adds PAOS content type to the outgoing
http GET requests. Catches incoming PAOS requests and responds (with HTTP SOAP 
posts) giving the parameters the server asked for. Only accepts PAOS requests
from certain URLs (for example help-portal.com, nokia.com).

Message exchange pattern:
1. HTTP GET (by the client)
   * includes PAOS headers
2. HTTP 200 (server responds by making a PAOS reques)
   * Content type: application/vnd.paos+xml
   * SOAP envelope
3. HTTP POST
   * Content type: application/vnd.paos+xml
4. HTTP 200

@see Liberty Reverse HTTP binding for SOAP specification 1.1
*/

class CPnpPaosFilter :
    public CEComFilter, 
    public MHTTPFilter,
    public MHTTPDataSupplier
    /* public MHTTPTransactionCallback, to close a transaction
     * and open a new transaction 
     */
    {
public:
    /**
     * This method creates an instance of the filter and
     * installs it to the HTTP session's filter queue.
     * This method is called by the ECOM framework.
     *
     * @param  aSession a handle to the http session.
     * @return pointer to the newly installed filter.
     */
    static CEComFilter* InstantiateL( TAny* aSession );

    static CPnpPaosFilter* NewL( RHTTPSession* pSession );
    virtual ~CPnpPaosFilter();

public: // from MHTTPDataSupplier

    TBool GetNextDataPart( TPtrC8& aDataPart );
    void ReleaseData();
    TInt OverallDataSize();
    TInt Reset();

public:

    /**
     * From MHTTPFilterBase::MHFRunL.
     * Called when the filter's registration conditions are satisfied
     * for events that occur on a transaction.
     * Leaves with KErrNotSupported if making a PAOS request from
     * an untrusted domain. (other but nokia.com and help-portal.com)
     * , KErrArgument if PAOS request could not be parsed.
     *
     * @param aTransaction The transaction that the event has occurred on
     * @param aEvent The event that has occurred
     */
    virtual void MHFRunL( RHTTPTransaction aTransaction,
        const THTTPEvent& aEvent);

    /**
     * From MHTTPFilterBase::MHFRunError.
     * Called when RunL leaves from a transaction event.
     *
     * @param aError The leave code that RunL left with
     * @param aTransaction The transaction that was being processed
     * @param aEvent The Event that was being processed
     * @return KErrNone if the error has been cancelled or the code
     *         of the continuing error otherwise
     */
    virtual TInt MHFRunError( TInt aError,
                              RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent);
    
    
    /**
     * From MHTTPFilter::MHFUnload.
     * Called when the filter is being removed from a session's
     * filter queue.
     *
     * @param aSession The session it's being removed from
     * @param aHandle The filter handle. Complex filters may need to
     *        refer to this to keep track of which particular registration
     *        is being unloaded
     */
    virtual void MHFUnload( RHTTPSession aSession, THTTPFilterHandle aHandle);

    /**
     * From MHTTPFilter::MHFLoad.
     * Called when the filter is being added to the session's
     * filter queue.
     *
     * @param aSession The session it's being added to
     * @param aHandle The filter handle. Complex filters may need to keep
     *        track of this, for instance if generating events in
     *        response to external stimuli.
     */
    virtual void MHFLoad( RHTTPSession aSession, THTTPFilterHandle aHandle );

private:
    CPnpPaosFilter( RHTTPSession* pSession );
    void ConstructL();

private:
    /**
    Read allowed domains into iTrustedHdcDomains
    */
    void ReadAllowedDomainsL();
    
    /**
    Read allowed domains into iTrustedPnpDomains
    */
    void ReadAllowedPnpDomainsL();

    /**
    Add PAOS header and paos content type to accept header.
    */
    void AddPaosHeadersL( RHTTPHeaders& requestHeaders );

    /**
    Append the received data chunk to an internal buffer
    */
    void CollectResponseBodyL( MHTTPDataSupplier& aBody );

    /**
    Removes components EUriUserinfo, EUriPath, EUriQuery and EUriFragment
    from the uri. Only EUriScheme, EUriHost and EUriPort are left.
    */
    void HostFromUriL( CUri8* aUri );

    /**
    Check whether the content type of http headers match to the PAOS content
    type.
    */
    TBool IsPaosContentTypeL( RHTTPHeaders& aHeaders );

    /**
    Read PAOS request from iPnpPaosXml, construct a PAOS response and
    post it using PaosPostL.
    */
    void PostPaosResponseL( RHTTPTransaction& aTransaction );

    /**
    Cancels the current transaction and makes 
    a HTTP Post with iPaosResponse as a body.
    */
    void PaosPostL( RHTTPTransaction& aTransaction );

    /**
    Save HDC trigger response to a file and open it with HelpDeskConnect client
    */
    void HandleHdcTriggerL();

    /**
    Add/Replace a header in RHTTPHeaders
    */
    void SetHttpHeaderL( RHTTPHeaders& aMessage, const HTTP::TStrings aIndex,
        const TDesC8& aString );

    TBool IsPaosHomeDomainL( const TPtrC8 aHost );

private:
    /** A counter for tracking filter usage. */
    TInt iLoadCount;
    /** Transaction id of the current PAOS request. */
    TInt iTransactionId;
    /** Filter name for filter listening for events */
    RStringF iFilterName;
    /** The URL (exluding path info) in the current PAOS request */
    CUri8* iPaosPostUrl;
    /** Parses the received XML documents and constructs a response */
    CPnpPaosXml* iPnpPaosXml;
    /** http session */
    RHTTPSession* iSession;
    /** string pool for storing RStrings */
    RStringPool iSessionStringPool;
    /** Set to ETrue if a request made to nokia.com or help-portal.com
    This member tells if a PAOS request is allowed or not */
    TBool iMakePaosResponse;
#ifndef __SERIES60_ 
    /** 
    Set to ETrue when a HelpDeskConnect PAOS post has been successfully done.
    May be used    to check if a HDC trigger file should be expected next 
    */
    TBool iHdcPaosPostDone;
#endif
    /** Domains that are allowed to make PAOS requests for HDC parameters */
    HdcTrustedDomainList iTrustedHdcDomains;
    
    PaosTrustedDomainList iTrustedPnpDomains;
    };


#endif // WSPAOSFILTER_H

