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


#ifndef __HTTP_PROV_CONTENT_TYPE__H
#define __HTTP_PROV_CONTENT_TYPE__H

// INCLUDES
#include <e32base.h>
#include "SupportedContentTypes.h"

// Security parameter NETWPIN
const TUint8 KSECNETWPIN = 0x00;

// Security parameter USERPIN
const TUint8 KSECUSERPIN = 0x01;

// Security parameter USERNETWPIN
const TUint8 KSECUSERNETWPIN = 0x02;

// Security parameter USERPINMAC
const TUint8 KSECUSERPINMAC = 0x03;

// Securirty parameter PKA (or PKI)
const TUint8 KSECPKA = 0x04;

// No security parameter
const TUint8 KSECNONE = 0xff;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * CHttpProvContentType holds http provisioning content
 */ 
class CHttpProvContentType : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHttpProvContentType* NewL(const TDesC8& aContentTypeField);

        /**
        * Two-phased constructor.
        */
        static CHttpProvContentType* NewLC(const TDesC8& aContentTypeField);

        /**
        * Destructor.
        */
        virtual ~CHttpProvContentType();

    public: 

        /**
        * Get MAC
        */
        void MACL( TPtrC8& aMac );

        /**
        * Returns SEC type.
        * @return SEC
        */
        TUint8 SEC();

        /**
        * Get content type.
        */
        const TDesC8& ContentTypeL() const;

    private:
        /**
        * C++ default constructor.
        */
        CHttpProvContentType();

        /**
        * By default Symbian 2nd phase constructor is private.
        * Descriptor aContentTypeField must remain in the scope until CHttpProvContentType
        * is deleted.
        */
        void ConstructL(const TDesC8& aContentTypeField);

        /**
        * Parses content type
        */
        void ParseL(const TDesC8& aContentTypeField);

    private:  // Data
        // SEC type
        TUint8        iSEC;
        // Content type
        TPtrC8        iContentType;
        // MAC
        TPtrC8        iMAC;
    };

#endif    // __HTTP_PROV_CONTENT_TYPE__H
            
// End of File
