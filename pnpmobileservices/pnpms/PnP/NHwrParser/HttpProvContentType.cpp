/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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



// INCLUDE FILES
#include "HttpProvContentType.h"
#include "NHwrParserLogger.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpProvContentType::CHttpProvContentType
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpProvContentType::CHttpProvContentType() :
    iSEC(KSECNONE),
    iContentType( KNullDesC8 ),
    iMAC( KNullDesC8 )
    {
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpProvContentType::ConstructL(const TDesC8& aContentTypeField)
    {
    ParseL( aContentTypeField );
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpProvContentType* CHttpProvContentType::NewL(const TDesC8& aContentTypeField)
    {
    LOGSTRING( "CHttpProvContentType::NewL()" );
    CHttpProvContentType* self = NewLC(aContentTypeField);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpProvContentType* CHttpProvContentType::NewLC(const TDesC8& aContentTypeField)
    {
    LOGSTRING( "CHttpProvContentType::NewLC()" );
    CHttpProvContentType* self = new(ELeave) CHttpProvContentType; 
    CleanupStack::PushL(self);
    self->ConstructL(aContentTypeField);
    return self;
    }

// Destructor
CHttpProvContentType::~CHttpProvContentType()
    {
    LOGSTRING( "~CHttpProvContentType" );
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::ParseL
// -----------------------------------------------------------------------------
//
void CHttpProvContentType::ParseL( const TDesC8& aContentTypeField )
    {
    LOGSTRING( "CHttpProvContentType::ParseL" );
    iContentType.Set( aContentTypeField );

    // Check MIME type
    if( iContentType.Find( KContentTypeApplicationWapConnectivityWbxml ) == 0 )
        {
        LOGSTRING("CHttpProvContentType Content type application/vnd.wap.connectivity-wbxml");
        iContentType.Set( KContentTypeApplicationWapConnectivityWbxml );
        }
    else if( iContentType.Find( KContentTypeTextPlain ) == 0 )
        {
        LOGSTRING("CHttpProvContentType Content type text/plain");
        iContentType.Set( KContentTypeTextPlain );
        }
    else
        {
        LOGSTRING("CHttpProvContentType::ParseL->Invalid contentType");
        LOGTEXT( aContentTypeField );
        User::Leave( KErrNotSupported );
        }

    // contentTypeFieldLen is at least the lentgh of KContentTypeApplicationWapConnectivityWbxml
    TInt contentTypeFieldLen( aContentTypeField.Length() );

    // remove line feed/carriage return
    _LIT8( KNewLine, "\n" );
    _LIT8( KCarriageReturn, "\r" );
    _LIT8( KLineFeed, "\r\n" );
    if( aContentTypeField.Mid( contentTypeFieldLen - 1 ) == KNewLine ||
        aContentTypeField.Mid( contentTypeFieldLen - 1 ) == KCarriageReturn )
        {
        LOGSTRING("New line / Carriage return found");
        contentTypeFieldLen--;
        }
    else if( aContentTypeField.Mid( contentTypeFieldLen - 2 ) == KLineFeed )
        {
        LOGSTRING("Line feed found");
        contentTypeFieldLen -= 2;
        }
    LOGSTRING2( "CHttpProvContentType::contentTypeFieldLen: %i", contentTypeFieldLen );
    if( contentTypeFieldLen <= iContentType.Length() )
        {
        LOGSTRING("iSEC = KSECNONE");
        // No possibility for a security breach since CHttpProvContent::AuthenticateL
        // returns KHttpProvAuthResultNoAuthentication in this case
        iSEC = KSECNONE;
        return;
        }
    // aContentTypeField minus possible line feed / carriage return chars 
    TPtrC8 contentTypeField = aContentTypeField.Left( contentTypeFieldLen );


    // Parse all parameteres
    _LIT8(KDelimeter, ";");
    _LIT8(KEqual,"=");
    // runner is aContentTypeField without the content type (in the beginning)
    // and without the line feed / carriage return chars (at the end)
    TPtrC8 runner( contentTypeField.Mid( iContentType.Length() ) );
    TBool eot(EFalse);
    while(runner.Length()>0 && !eot)
        {
        // Find delimeter and seek over it
        if(runner.Left(1).Compare(KDelimeter)!=0)
            {
            LOGSTRING("CHttpProvContentType::ParseL->Delimeter not found");
            User::Leave(KErrCorrupt);
            }
        runner.Set(runner.Mid(1));
        // new code
        LOGSTRING( "skip leading space" );
        // skip leading space
        FOREVER
            {
            LOGSTRING( "FOREVER" );
            if( runner.Length() && runner[0] == ' ' )
                {
                LOGSTRING( "skipping leading space" );
                runner.Set( runner.Mid(1) );
                LOGSTRING( "runner now:" );
                LOGTEXT( runner );
                }
            else
                {
                LOGSTRING( "break" );
                break;
                }
            }
        LOGSTRING( "skip trailing space" );
        // skip trailing space
        FOREVER
            {
            if( runner.Length() && runner[runner.Length()-1] == ' ' )
                {
                LOGSTRING( "skipping trailing space" );
                runner.Set( runner.Left(runner.Length()-1) );
                LOGSTRING( "runner now:" );
                LOGTEXT( runner );
                }
            else
                {
                LOGSTRING( "break" );
                break;
                }
            }

        // Parse parameter name and value
        TInt pos = runner.Find(KEqual);
        if(pos<1)
            {
            LOGSTRING("CHttpProvContentType::ParseL->Parameters corrupted");
            User::Leave(KErrCorrupt);
            }
        TPtrC8 paramName(runner.Left(pos));
        TPtrC8 paramValue(runner.Mid(pos+1));
        TInt endpos = paramValue.Find(KDelimeter);
        if(endpos>0)
            {
            paramValue.Set(paramValue.Left(endpos));
            if(runner.Length()<=pos+1+endpos)
                {
                LOGSTRING("CHttpProvContentType::ParseL->Delimeter found at the end (corrupt)");
                User::Leave(KErrCorrupt);
                }
            runner.Set(runner.Mid(pos+1+endpos));
            }
        else
            {
            eot = ETrue;
            }

        // Store SEC & MAC parameters
        // We could store other parameters as well, but we don't need them
        _LIT8(KSec,"SEC");
        _LIT8(KMac,"MAC");
        LOGTEXT( paramName );
        LOGTEXT( paramValue );
        if(paramName == KSec)
            {
            // parse SEC type
            _LIT8(KNetWPin,"NETWPIN");
            _LIT8(KUserPin,"USERPIN");
            _LIT8(KUserNetWPin,"USERNETWPIN");
            _LIT8(KUserPinMac,"USERPINMAC");
            _LIT8( KPublicKeyAuthentication, "PKA" );

            if(paramValue.Compare(KNetWPin)==0)
                {
                iSEC = KSECNETWPIN;
                }
            else if(paramValue.Compare(KUserPin)==0)
                {
                iSEC = KSECUSERPIN;
                }
            else if(paramValue.Compare(KUserNetWPin)==0)
                {
                iSEC = KSECUSERNETWPIN;
                }
            else if(paramValue.Compare(KUserPinMac)==0)
                {
                iSEC = KSECUSERPINMAC;
                }
            else if( paramValue.Compare( KPublicKeyAuthentication ) == 0
                || paramValue.Compare( _L8("PKI") ) == 0 /*temporary*/ )
                {
                iSEC = KSECPKA;
                }
            else 
                {
                // Assume PKA method in case iSEC-parameter is missing
                iSEC = KSECPKA;
                //iSEC = KSECNONE;
                }
            }
        else if(paramName == KMac)
            {
            // Store MAC
            iMAC.Set( paramValue );
            }
        }    

    LOGSTRING( "ParseL - done" );
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::MAC
// -----------------------------------------------------------------------------
//
void CHttpProvContentType::MACL(TPtrC8& aMAC)
    {
    if( iMAC.Length() )
        {
        aMAC.Set( iMAC );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CHttpProvContentType::SEC
// -----------------------------------------------------------------------------
//
TUint8 CHttpProvContentType::SEC()
    {
    return iSEC;
    }

const TDesC8& CHttpProvContentType::ContentTypeL() const
    {
    if( iContentType.Length() < 1 )
        {
        User::Leave( KErrNotFound );
        }
    return iContentType;
    }

//  End of File  
