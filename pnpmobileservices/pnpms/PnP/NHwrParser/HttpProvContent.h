/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


#ifndef __HTTP_PROV_CONTENT__H
#define __HTTP_PROV_CONTENT__H

// INCLUDES
#include <e32base.h>

// Authentication result: Authenticated
const TInt KHttpProvAuthResultAuthenticated = 0;

// Authentication result: PIN required, but not supplied
const TInt KHttpProvAuthResultPinRequired = 1;

// Authentication result: Authentication failed
const TInt KHttpProvAuthResultAuthenticationFailed = 2;

// Authentication result: No authentication contained in message
const TInt KHttpProvAuthResultNoAuthentication = 3;

// Authentication result: The token has expired
const TInt KHttpProvAuthResultTokenExpired = 4;

// FORWARD DECLARATIONS
class CHttpProvContentType;
class CHttpProvHeaders;
// CLASS DECLARATION

/**
 * CHttpProvContent holds http provisioning content
 */ 
class CHttpProvContent : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHttpProvContent* NewL(const TPtrC8 aContent);

        /**
        * Two-phased constructor.
        */
        static CHttpProvContent* NewLC(const TPtrC8 aContent);

        /**
        * Destructor.
        */
        virtual ~CHttpProvContent();

    public: 

        /**
        * Return state of authetication
        * @returns ETrue if authenticated
        */
        TBool Authenticated();

        /**
        * Authenticates mcontent
        * @returns error value defined at start of this file
        */
        TInt AuthenticateL( const CHttpProvHeaders& aHeaders, CHttpProvContentType& aContentType );

    private:
        /**
        * C++ default constructor.
        */
        CHttpProvContent();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TPtrC8 aContent);

        void AuthenticateSenderL( const TDesC8& aKey, CHttpProvContentType& aContentType, TInt& aResult );

        /**
        * Converter
        */
        HBufC8* PackLC( const TDesC8& aHex ) const;
        
        /**
        * Converts IMSI code
        */
        void ConvertIMSIL( const TDesC& aIMSI, TPtr8& aKey ) const;

    private:  // Data
        // Authentication status
        TBool        iAuthenticated;
        // Content
        TPtrC8        iContent;
    };

#endif    // __HTTP_PROV_CONTENT__H
            
// End of File
