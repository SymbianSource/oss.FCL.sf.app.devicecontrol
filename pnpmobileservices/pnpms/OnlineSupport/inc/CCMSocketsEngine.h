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
* Description:  Socket engine class used for the DNS query
*
*/



#ifndef CCMSOCKETSENGINE_H
#define CCMSOCKETSENGINE_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <http/mhttptransactioncallback.h>
#include <http/rhttpsession.h>
#include <http/rhttptransaction.h>

// CONSTANTS
const TInt KMaximumLengthOfAddress = 100;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
class MCMSocketsEngineNotifier;

/**
*  Sockets engine class 
*  Used to test the DNS connection 
*
*  @lib Confmanager.app
*  @since 2.0
*/
class CCMSocketsEngine :
    public CActive,
    public MHTTPTransactionCallback
    {
    public:  // Constructors and destructor
        
        enum TEngineStatus 
            {
            EStatusUnknown,
            EStatusConnecting,
            EStatusResolving,
            EStatusMakingHttpHeadRequest,
            EStatusReady
            };
        /**
        * Two-phased constructor.
        */
        static CCMSocketsEngine* NewL( MCMSocketsEngineNotifier& aNotifier);
        
        /**
        * Destructor.
        */
        ~CCMSocketsEngine();

    public: // New functions

        /**
        * Starts the DNS query
        * @since 2.0
        * @param aAp the access point id to use for the connection
        * @param aAddress address 
        */
        void ConnectL( TUint32 aAp, const TDesC& aAddress );

        /**
        * Starts the DNS query
        * @since 2.0
        * @param aAp the access point id to use for the connection
        * @param aAddress address 
        */
        void ConnectL( const TDesC& aAddress );
        
        /**
        * Ends the DNS query
        * @since 2.0
        */
        void Disconnect( );

        /**
        * Sets the value for the iLeaveOpen
        * @since 2.0
        */
        void SetOpen( TBool aOpen );

        const TDesC& ProxyIp(){ return *iProxyIp; };
        const TInt ProxyPort(){ return iProxyPort; };
        const TUint32 IapId(){ return iIapId; };
        const TBool ProxyDefined(){ return iProxyIp->Length() > 0; };

        /*!
        @return iSocketServ
        */
        RSocketServ& SocketServ(){ return iSocketServ; };

        /*!
        @return iConnection
        */
        RConnection& Connection(){ return iConnection; };
        
	/*!
        @return IAPID
        */

        TUint32 ReturnIAPID();

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
        TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    protected:  // New functions
        
        

    protected:  // Functions from base classes
        
       

    private: // Functions from base classes
        
        /**
        * From CActive Handles an active object’s request completion event.
        * @since 2.0
        * @return void
        */
        void RunL();

        /**
        * From CActive  Cancels any outstanding request
        * @return void
        */
        void DoCancel();

        /**
        * From CActive  Cancels any outstanding request
        * @return TInt should be always KErrNone
        */
        TInt RunError(TInt aError);    

    private:

        /**
        * C++ default constructor.
        */
        CCMSocketsEngine( MCMSocketsEngineNotifier& aNotifier);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void MakeHttpHeadRequestL( const TDesC& aProxyAddress, const TUint aProxyPort );
        TBool CheckForProxyL( const TUint32 aIapId );
    public:     // Data
        
    protected:  // Data
        

    private:    // Data
        /*! Interface to UI class */
        MCMSocketsEngineNotifier& iSocketsEngineNotifier; 
        // Socket server
        RSocketServ iSocketServ;
        //  host name resolution (DNS)
        RHostResolver iResolver;
        /*! Connection */
        RConnection iConnection;
        /*! Connection open flag */
        TBool iConnectionOpen;
        // Name entry for the resolver
        TNameEntry iNameEntry;
        // if This is true then the iResolver exsists
        TBool iResolverExsists;
        // Status of the engine
        TEngineStatus iEngineStatus;
        // address to be resolved
        TBuf <KMaximumLengthOfAddress> iWapPage;
        // If this is true then the connection will be left open after the engine is deleted
        TBool iLeaveOpen;
        // if this is true then the connection exsists
        TBool iConnectionExsists;     
        /*! Proxy IP, empty descriptor for none */
        HBufC* iProxyIp;
        /*! Proxy port, 0 for none */
        TInt iProxyPort;
        /*! Tells if there is a proxy defined for the current AP */
        TBool iProxyDefined;

        /*! Http session */
        RHTTPSession iSession;
        /*! Http session open flag */
        TBool iHttpSessionOpen;
        /*! String pool for http session */
        RStringPool iSessionStringPool;
        /*! Http transaction*/
        RHTTPTransaction iTransaction;
        /*! Http transaction open flag */
        TBool iTransactionOpen;

        /** Id for the current AccessPoint this id refers to the WAP_IAP table.*/
        TUint32 iIapId;

    public:     // Friend classes
            
    protected:  // Friend classes
        
    private:    // Friend classes
        

    };

#endif      // CCMSocketsEngine_H   
            
// End of File
