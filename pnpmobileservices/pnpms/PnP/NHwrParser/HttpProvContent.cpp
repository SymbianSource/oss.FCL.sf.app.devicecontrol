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



// INCLUDE FILES
#include <hash.h>

#include "HttpProvContent.h"
#include "HttpProvContentType.h"
#include "NHwrParserLogger.h"
#include "HttpProvHeaders.h"
#include "PnpUtilImpl.h"

// CONSTANTS

// Number of BCD digits in byte
const TInt KNumDigitsInByte = 2;

// Number of bits in half-byte
const TInt KNumBitsInNibble = 4;

// Ascii code for zero
const TUint8 KZero = '0';

// Padding half-byte
const TUint8 KPadNibble = 0xf;

// First nibble
const TUint8 KFirstNibble = 0x1;

// Parity bit number in first nibble
const TUint KParityBitNum = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpProvContent::CHttpProvContent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpProvContent::CHttpProvContent() : iAuthenticated( EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpProvContent::ConstructL( const TPtrC8 aContent )
    {
    iContent.Set( aContent );
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpProvContent* CHttpProvContent::NewL(const TPtrC8 aContent)
    {
    LOGSTRING( "CHttpProvContent::NewL()" );
    CHttpProvContent* self = NewLC(aContent);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpProvContent* CHttpProvContent::NewLC(const TPtrC8 aContent)
    {
    LOGSTRING( "CHttpProvContent::NewLC()" );
    CHttpProvContent* self = new(ELeave) CHttpProvContent; 
    CleanupStack::PushL(self);
    self->ConstructL(aContent);
    return self;
    }

// Destructor
CHttpProvContent::~CHttpProvContent()
    {
    LOGSTRING( "~CHttpProvContent - done" );
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::AuthenticateL
// -----------------------------------------------------------------------------
//
TInt CHttpProvContent::AuthenticateL( const CHttpProvHeaders& aHeaders, CHttpProvContentType& aContentType )
    {
    LOGSTRING( "CHttpProvContent::AuthenticateL" );

    TInt result( KHttpProvAuthResultAuthenticated );

    CPnpUtilImpl* pnpUtil = CPnpUtilImpl::NewLC();

    TInt token = 0;
    TBuf8<4> tokenbuf;    // Token cannot be more the 4 characters long
    tokenbuf.Zero();

    RMobilePhone::TMobilePhoneSubscriberId imsi;
    //    util->Imsi(imsi);
    //    RDebug::Print(imsi);
    
    // Create space for key
    HBufC8* key = HBufC8::NewLC( Max( 1, imsi.MaxLength() + tokenbuf.MaxLength() ) );
    TPtr8 keyPtr( key->Des() );

    LOGSTRING2( "Content type SEC: %i", aContentType.SEC() );

    // We support only security type NETWPIN
    switch( aContentType.SEC() )
        {
        case KSECUSERPIN:
            {
            LOGSTRING("KSECUSERPIN");
            // Get token saved by ConfManager    
            TInt err = pnpUtil->GetTokenValue( token );
            tokenbuf.AppendNum(token);
            if( err != KErrNone )
                {
                result = KHttpProvAuthResultTokenExpired;
                }
            else if( tokenbuf.Length() == 0 )
                {
                result = KHttpProvAuthResultPinRequired;
                }

            keyPtr.Copy( tokenbuf );
            if( result == KHttpProvAuthResultAuthenticated )
                AuthenticateSenderL( *key, aContentType, result );
            break;
            }

        case KSECUSERNETWPIN:
            {
            LOGSTRING("KSECUSERNETWPIN");
            // Get token saved by ConfManager    
            TInt err = pnpUtil->GetTokenValue( token );
            tokenbuf.AppendNum(token);
            if( err != KErrNone )
                {
                result = KHttpProvAuthResultTokenExpired;
                }
            else if( tokenbuf.Length() == 0 )
                {
                result = KHttpProvAuthResultPinRequired;
                }

            ConvertIMSIL( imsi, keyPtr );
            keyPtr.Append( tokenbuf );

            if( result == KHttpProvAuthResultAuthenticated )
                AuthenticateSenderL( *key, aContentType, result );
            break;
            }

        case KSECUSERPINMAC:
            {
            LOGSTRING("KSECUSERPINMAC");
            // Get token saved by ConfManager    
            TInt err = pnpUtil->GetTokenValue( token );
            tokenbuf.AppendNum(token);
            if( err != KErrNone )
                {
                result = KHttpProvAuthResultTokenExpired;
                }
            else if( tokenbuf.Length() == 0 )
                {
                result = KHttpProvAuthResultPinRequired;
                }

            keyPtr.Copy( tokenbuf );

            if( result == KHttpProvAuthResultAuthenticated )
                AuthenticateSenderL( *key, aContentType, result );
            break;
            }
        case KSECPKA:
            {
            LOGSTRING("KSECPKA");
            LOGSTRING( "Get nonce" );
            TBuf8<KNonceLength> nonce;
            pnpUtil->GetNonceL( nonce );

            LOGSTRING( "Verify signature" );
            // Verify digest (Hash value of "nonce:data") and signature of the sender
            if( !pnpUtil->VerifySignatureL(
                    aHeaders.GetParamValL( KDigestValue ),
                    aHeaders.GetParamValL( KSignatureValue ),
                    iContent,
                    nonce ) )
                {
                LOGSTRING("Signature verify failed");
                result = KHttpProvAuthResultAuthenticationFailed;
                }
            else
                {
                LOGSTRING("Signature verified");
                }
            break;
            }
        default:
            {
            LOGSTRING("No authentication");
            result = KHttpProvAuthResultNoAuthentication;
            }
        }


    CleanupStack::PopAndDestroy( key ); // key, headerMac

    CleanupStack::PopAndDestroy( pnpUtil );

    LOGSTRING2( "CHttpProvContent::AuthenticateL - done: %i", result );
    
    return result;
    }

void CHttpProvContent::AuthenticateSenderL( const TDesC8& aKey, CHttpProvContentType& aContentType, TInt& aResult )
    {
    LOGSTRING( "CHttpProvContent::AuthenticateSenderL" );
    // The HMAC is in ASCII HEX format. Convert to binary.
    TPtrC8 mac;
    aContentType.MACL(mac);
    LOGSTRING( "mac:" );
    LOGTEXT( mac );
    HBufC8* headerMac = PackLC( mac );
    CMessageDigest* digest = CSHA1::NewL();
    CleanupStack::PushL( digest );

    if( aContentType.SEC() == KSECUSERPINMAC )
        {
        // key C is a concatenation of pin K and digest m
        TPtrC8 K( aKey.Left( aKey.Length()/2 ) );
        TPtrC8 m( aKey.Right( aKey.Length()/2 ) );

        // M' = HMAC-SHA(K, A)
        CHMAC* hmac = CHMAC::NewL( K, digest );
        // Ownership of digest is transferred to hmac created above
        CleanupStack::Pop( digest );
        CleanupStack::PushL( hmac );
        TPtrC8 MM( hmac->Hash( iContent ) );

        // Create m' (renamed to mm)
        HBufC8* mm = HBufC8::NewLC( m.Length() );
        TPtr8 ptr( mm->Des() );
        for( TInt i( 0 ); i < m.Length(); i++ )
            {
            ptr.Append( (MM[i]%10)+KZero );
            }

        // Compare the MACs and mark the message as authenticated
        if( *mm != m )
            {
            aResult = KHttpProvAuthResultAuthenticationFailed;
            }
        CleanupStack::PopAndDestroy( mm );
        CleanupStack::PopAndDestroy( hmac );
        }
    else
        {
        // Create the HMAC from body
        CHMAC* hmac = CHMAC::NewL( aKey, digest );
        LOGTEXT( aKey );
        // Ownership of digest is transferred to hmac created above
        CleanupStack::Pop( digest );
        CleanupStack::PushL( hmac );

        // Compare the MACs and mark the message as authenticated
        TPtrC8 temp = hmac->Hash( iContent );
        if( headerMac->Length() == 0 
            || temp != *headerMac )
            {
            LOGSTRING( "KHttpProvAuthResultAuthenticationFailed" );
            aResult = KHttpProvAuthResultAuthenticationFailed;
            }
        CleanupStack::PopAndDestroy( hmac );
        }
    CleanupStack::PopAndDestroy( headerMac );
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::PackLC
// -----------------------------------------------------------------------------
//
HBufC8* CHttpProvContent::PackLC( const TDesC8& aHex ) const
    {
    HBufC8* bin = HBufC8::NewLC( aHex.Length()/2 );
    TPtr8 binPtr( bin->Des() );
    for( TInt i( 0 ); i < aHex.Length()/2; i++ )
        {
        TLex8 lex( aHex.Mid( i*2, 2 ) );
        TUint8 byte( 0 );
        User::LeaveIfError( lex.Val( byte, EHex ) );
        binPtr.Append( TUint8( byte ) );
        }

    return bin;
    }

// -----------------------------------------------------------------------------
// CHttpProvContent::ConvertIMSIL
// -----------------------------------------------------------------------------
//
void CHttpProvContent::ConvertIMSIL( const TDesC& aIMSI, TPtr8& aKey ) const
    {
    TUint8 parity( TUint8((aIMSI.Length() % 2) << KParityBitNum) );

    if( aIMSI.Length() == 0 )
        {
        aKey.Append( (KPadNibble<<KNumBitsInNibble) + KFirstNibble + parity );
        return;
        }

    // First byte contains just a header and one digit
    TInt first( aIMSI[0] - KZero );
    aKey.Append( (first<<KNumBitsInNibble) | KFirstNibble | parity );

    // Use semi-octet or BCD packing of IMSI. It means that one byte contains two
    // decimal numbers, each in its own nibble.
    for( TInt i( 1 ); i < aIMSI.Length(); i += KNumDigitsInByte )
        {
        TInt first( aIMSI[i] - KZero );
        TInt second( 0 );

        if( aIMSI.Length() == i+1 )
            {
            second = KPadNibble;
            }
        else
            {
            second = aIMSI[i+1] - KZero;
            }

        aKey.Append( (second<<KNumBitsInNibble) + first );
        }
    }


//  End of File  
