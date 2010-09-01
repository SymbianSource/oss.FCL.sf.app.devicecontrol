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
* Description:  Header file CMiniBrowser class
*
*/


#ifndef MINI_BROWSER_H
#define MINI_BROWSER_H

//  INCLUDES
#include <http/mhttptransactioncallback.h>
#include <http/mhttpdatasupplier.h>
#include <http/rhttpsession.h>
#include <http/rhttptransaction.h>


#include "MiniBrowserObserver.h"
#include "HttpProvStates.h"

// FORWARD DECLARATIONS
class CNHeadWrapperParser;
    
// CLASS DECLARATION

/**
*  CMiniBrowser class
*
*  @since 3.1?
*/
class CMiniBrowser :
    public CBase,
    public MHTTPTransactionCallback,
    public MHTTPDataSupplier
    {
public:
    enum TContentType
        {
        EContentTypeUnknown = 0,
        EContentTypeTextPlain,
        EContentTypeNokiaHeadWrapper
        };
    /**
    Standard Symbian 2 phase constructor. The returned object is left
    to the CleanupStack.
    @param    aObserver Listens for events/errors
    @param    aSocketServ RSocketServ. Note! This must be opened until
            the CMiniBrowser object has been destroyed.
    @param    aSocketServ RSocketServ. Note! This must be opened and
            started until the CMiniBrowser object has been destroyed.
    */      
    static CMiniBrowser* NewLC( MMiniBrowserObserver& aObserver,
        RSocketServ& aSocketServ, RConnection& aConnection );

    /**
    Standard Symbian 2 phase constructor.
    @param aObserver Listens for events/errors
    @param    aSocketServ RSocketServ. Note! This must be opened until
            the CMiniBrowser object has been destroyed.
    @param    aSocketServ RSocketServ. Note! This must be opened and
            started until the CMiniBrowser object has been destroyed.
    */      
    static CMiniBrowser* NewL( MMiniBrowserObserver& aObserver, RSocketServ&
        aSocketServ, RConnection& aConnection );

    /**
    * Destructor.
    */      
    virtual ~CMiniBrowser();

    /**
    Make a HTTP GET request to the url defined.
    Use this override if the AP used does not have proxy defined.
    @param aUri 
    @param aProxyAddress Proxy server address.
    @param aProxyPort Port to be used for proxy server
    */      
    void HttpGetL( const TDesC& aUri );

    /**
    Make a HTTP GET request to the url defined.
    Use this override if the AP used has a proxy defined.
    @param aUri 
    @param aProxyAddress Proxy server address.
    @param aProxyPort Port to be used for proxy server
    */      
    void HttpGetL( const TDesC& aUri, const TDesC& aProxyAddress,
        const TUint aProxyPort );

    /**
    Returns the report URL that was in the previously received HeadWrapper document.
    Leaves with KErrNotFound if there was no valid report url or no HeadWrapper document
    has been received.
    @return Report Url. Left to CleanupStack.
    */
    HBufC* GetReportUrlLC();

private:
    CMiniBrowser( MMiniBrowserObserver& aObserver );
    void ConstructL( RSocketServ& aSocketServ, RConnection& aConnection );

    /**
    Called from HttpGetL.
    @see HttpGetL
    */
    void CMiniBrowser::DoHttpGetL(
        const TDesC& aUri,
        const TBool aUseProxy,
        const TDesC& aProxyAddress,
        const TUint aProxyPort );


public: // from MHTTPTransactionCallback

    /*!
    Called when the filter's registration conditions are satisfied
    for events that    occur on a transaction.    Note that this function
    is not allowed to leave if called with certain events
    */
    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent &aEvent );
    /*!
    Called when RunL leaves from a transaction event. This works
    in the same way as CActve::RunError; return KErrNone if you
    have handled the error. If you don't completely handle the
    error, a panic will occur.
    */
    TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction,
        const THTTPEvent& aEvent );

public: // from MHTTPDataSupplier

    /*!
    Obtain a data part from the supplier. The data is guaranteed to
    survive until a call is made to ReleaseData(). 
    */
    TBool GetNextDataPart( TPtrC8 &aDataPart );

    /*!
    Release the current data part being held at the data supplier.
    This call indicates to the supplier that the part is no longer
    needed, and another one can be supplied, if appropriate. 
    */
    void ReleaseData();

    /*!
    Obtain the overall size of the data being supplied, if known to
    the supplier. Where a body of data is supplied in several parts
    this size will be the sum of all the part sizes. If the size is
    not known, KErrNotFound is returned; in this case the client must
    use the return code of GetNextDataPart to find out when the data
    is complete.
    */
    TInt OverallDataSize();

    /*!
    Reset the data supplier. This indicates to the data supplier that
    it should return to the first part of the data. This could be used
    in a situation where the data consumer has encountered an error and
    needs the data to be supplied afresh. Even if the last part has
    been supplied (i.e. GetNextDataPart has returned ETrue), the data
    supplier should reset to the first part.

    If the supplier cannot reset it should return an error code;
    otherwise it should return KErrNone, where the reset will be
    assumed to have succeeded 
    */
    TInt Reset();

private:
    /*
    @return TInt KErrNone in case everything was Ok.
    */
    static TInt TimeOutCallBack( TAny* aMiniBrowser);
    /*
    Called by TimeOutCallBack
    */
    void DoTimeOutCallBackL();

    /*!
    Appends aData to iResponseData. If aData does not fit in, iResponseData
    is reallocated.
    */
    void AppendResponseDataL( const TDesC8& aData );

    /*!
    Deletes and allocates an empty buffer for iResponseData and iReportUrl.
    This is called from DoHttpGetL to prevent using same response data and
    report url twice.
    */
    void ResetResponseDataL();

    /**
    Currently not supported
    Shows the response text (from iResponseData) in a dialog. 
    */
//    void ShowMessageDialogL( const TDesC8& aMessage );

    /**
    Parses received data and saves provisioned settings if the data is a
    HeadWrapper document.
    */
    void ParseAndSaveProvisionedSettingsL();

    /**
    Saves provisioned settings.
    */
    void SaveProvisionedSettingsL( const TDesC8& aContent );

    /*!
    Get special user agent string for ServiceHelp silent mode.
    */
    HBufC8* GetUserAgentStringL();

    /*!
    Get report Url from CNHeadWrapperParser and store it into iReportUrl
    buffer for later reference.
    */
    void GetReportUrlL( const CNHeadWrapperParser& aNHwrParser );

private:
    /*! Time-out timer */
    CPeriodic* iPeriodic;
    /*! Observer is notified when transaction was successful or failed */
    MMiniBrowserObserver& iObserver;
    /*! Internet access point id */
    TInt iIapId;
    /*! buffer for URI */
    HBufC8* iUri;
    /*! buffer for received data */
    HBufC8* iResponseData;
    /*! buffer for report/next url parameter of received HeadWrapper document */
    HBufC8* iReportUrl;
    /*! The content type header value of the received data */
    TContentType iContentType;
    /*! Http session */
    RHTTPSession iSession;
    /*! String pool for http session */
    RStringPool iSessionStringPool;
    /*! Http transaction*/
    RHTTPTransaction iTransaction;
    /*! The status of last provisioning, default is EStatusUnknown */
    THttpProvStates::TProvisioningStatus iProvisioningStatus;
    };

#endif  // MINI_BROWSER_H

// End of File
