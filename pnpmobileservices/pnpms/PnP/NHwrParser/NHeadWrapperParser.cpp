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
#include <e32svr.h>
#include "NHeadWrapperParser.h"
#include "NHwrParserLogger.h"
#include "HttpProvHeaders.h"
#include "HttpProvContent.h"
#include "HttpProvContentType.h"
#include "SupportedContentTypes.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::CNHeadWrapperParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNHeadWrapperParser::CNHeadWrapperParser()
    {
    }
    
// Destructor
EXPORT_C CNHeadWrapperParser::~CNHeadWrapperParser()
    {
    LOGSTRING("~CNHeadWrapperParser");
    delete iHttpProvHeaders;
    delete iHttpProvContentType;
    LOGSTRING("~CNHeadWrapperParser - done");
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNHeadWrapperParser* CNHeadWrapperParser::NewL(const TPtrC8& aWrapperData)
    {
    CNHeadWrapperParser* self = new (ELeave) CNHeadWrapperParser;
    CleanupStack::PushL( self );
    self->ConstructL( aWrapperData );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNHeadWrapperParser::ConstructL( const TPtrC8& aWrapperData )
    {
    iWrapperData.Set( aWrapperData );
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::ParseL
// -----------------------------------------------------------------------------
//
EXPORT_C THttpProvStates::TProvisioningStatus CNHeadWrapperParser::Parse()
    {
    LOGSTRING("CNHeadWrapperParser::Parse - begin");
    TRAPD( err, DoParseL() );
    if( err != KErrNone )
        {
        LOGSTRING("CNHeadWrapperParser::Parse ret: EStatusWrapperParsingFailed");
        return THttpProvStates::EStatusWrapperParsingFailed; // Do not continue
        }
        
    TRAP( err, CheckHeadWrapperL() );

    // CheckHeadWrapperL might leave with one of TProvisioningStatus codes
    // or with standard error codes
    if( err < 0 )    // standar error codes
        {
        LOGSTRING("CNHeadWrapperParser::Parse ret: EStatusWrapperParsingFailed");
        return THttpProvStates::EStatusWrapperParsingFailed;
        }
    else            // one of TProvisioningStatus
        {
        LOGSTRING2("CNHeadWrapperParser::Parse ret: %d", err);
        return (THttpProvStates::TProvisioningStatus) err;
        }
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::DoParseL
// -----------------------------------------------------------------------------
//
void CNHeadWrapperParser::DoParseL()
    {
    LOGSTRING( "CNHeadWrapperParser::DoParseL - begin" );
    LOGSTRING( "Check wrapper length " );
    // Check wrapper length min 6 bytes
    if( iWrapperData.Length() < 6 )
        {
        User::Leave( KErrCorrupt );
        }

    LOGSTRING( "Check signature" );
    // Check signature
    _LIT8( KHeadWrapperSignature, "NHWR" );
    if( iWrapperData.Left(4).Compare( KHeadWrapperSignature ) != 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Parse version
    TUint8 version = (iWrapperData)[4];
    if( version != 1 )
        {
        User::Leave( KErrNotSupported );
        }
        
    LOGSTRING( "Parse content type" );
    // Parse content type
    TUint8 contentTypeLen = (iWrapperData)[5];
    LOGSTRING2( "content type len: %i", contentTypeLen );

    if( iWrapperData.Length() < 6 + contentTypeLen )
        {   
        User::Leave( KErrCorrupt );
        }

    if( contentTypeLen < 1 )
        {
        User::Leave( KErrCorrupt );
        }

    iContentTypeField.Set( iWrapperData.Mid( 6, contentTypeLen ) );

    // Parse Headers and Content Length
    LOGSTRING( "Parse headers and data length" );
    if( iWrapperData.Length() < 6+contentTypeLen+2 ) //  +2 min for length datas
        {
        User::Leave( KErrCorrupt );
        }
    TUint32    headersLen = 0; //(*iWrapperData)[6+contentTypeLen+1];
    TUint32    contentLen = 0; //(*iWrapperData)[6+contentTypeLen+2];
    TUint headersVarLen = 0;
    TUint contentVarLen = 0;

    if( ParseUintVar( iWrapperData.Mid( 6+contentTypeLen ),
                      headersLen, headersVarLen ) != KErrNone )
        {
        User::Leave( KErrCorrupt );
        }
    LOGSTRING2( "headersLen %i", headersLen );
    LOGSTRING2( "headersVarLen %i", headersLen );

    if( ParseUintVar( iWrapperData.Mid( 6+contentTypeLen+headersVarLen ),
                      contentLen, contentVarLen ) != KErrNone )
        {
        LOGSTRING( "content length failed" );
        User::Leave( KErrCorrupt );
        }
    LOGSTRING2( "contentLen %i", contentLen );
    LOGSTRING2( "contentVarLen %i", contentVarLen );

    // Final check for sizes
    if( iWrapperData.Length() != (TInt)( 6+contentTypeLen+headersVarLen
        +contentVarLen+headersLen+contentLen ) )
        {
        LOGSTRING( "Lengths do not match" );
        LOGSTRING2( "Expected length %i", (TInt)( 6+contentTypeLen+headersVarLen
                                            +contentVarLen+headersLen+contentLen ) );
        LOGSTRING2( "Actual data length %i", iWrapperData.Length() );
        User::Leave( KErrCorrupt );
        }

    if( headersLen > 0 )
        {
        iHeaders.Set( iWrapperData.Mid( 6+contentTypeLen+headersVarLen
                                         +contentVarLen, headersLen ) ); 
        }

    LOGSTRING( "Parse content" );
    iContent.Set( iWrapperData.Mid( 6+contentTypeLen+headersVarLen+contentVarLen+headersLen,contentLen ) );

    // Log
#ifdef _DEBUG
    TInt i(0);
    LOGSTRING( "Content type:" );
    for( i = 0; i < iContentTypeField.Length(); i += 128 )
        {
        TPtrC8 logText = iContentTypeField.Right( iContentTypeField.Length() - i );
        LOGTEXT( logText );
        }

    if( iHeaders.Length() )
        {
        LOGSTRING( "Headers:" );
        for( i = 0; i < iHeaders.Length(); i += 128 )
            {
            TPtrC8 logText = iHeaders.Right( iHeaders.Length() - i );
            LOGTEXT( logText );
            }
        }

    LOGSTRING( "Content:" );
    for( i = 0; i < iContent.Length(); i += 128 )
        {
        TPtrC8 logText = iContent.Right( iContent.Length() - i );
        LOGTEXT( logText );
        }
#endif
    LOGSTRING( "CNHeadWrapperParser::DoParseL - end" );
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::CheckHeadWrapperL
// -----------------------------------------------------------------------------
//
void CNHeadWrapperParser::CheckHeadWrapperL()
    {
    LOGSTRING( "CNHeadWrapperParser::CheckHeadWrapperL()" );
    LOGSTRING( "Get content type" );
    if( iContentTypeField.Length() < 1 )
        {
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    LOGSTRING( "Create CHttpProvContentType" );
    if( iHttpProvContentType )
        {
        delete iHttpProvContentType;
        iHttpProvContentType = 0;
        }
    TRAPD( err, iHttpProvContentType = CHttpProvContentType::NewL( iContentTypeField ) );
    LOGSTRING( "Create CHttpProvContentType 2" );
    if( err != KErrNone )
        {
        LOGSTRING2( "Content type, create failed: %i", err );
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    LOGSTRING( "Get headers" );
    if( iHeaders.Length() < 1 )
        {
        LOGSTRING( "no headers" );
        // Plug and Play Mobile Services Specification.doc:
        // "The Headers field MAY contain headers defining additional
        // Meta data about the content"
        // if there is no headers in the wrapper we will just leave,
        // we do not want to open any unauthorized content:
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    delete iHttpProvHeaders;
    iHttpProvHeaders = 0;
    iHttpProvHeaders = CHttpProvHeaders::NewL( iHeaders );
    
    LOGSTRING( "Get wrapper content" );
    if( iContent.Length() < 1 )
        {
        User::Leave( THttpProvStates::EStatusWrapperParsingFailed );
        }

    CHttpProvContent* c = CHttpProvContent::NewLC( iContent );

    TInt res( KErrNone );
    TRAP( err, res = c->AuthenticateL( *iHttpProvHeaders, *iHttpProvContentType ) );
    if( res == KHttpProvAuthResultTokenExpired )
        {
        User::Leave( THttpProvStates::EStatusTokenExpired );
        }
    else if( res!=KHttpProvAuthResultAuthenticated || err != KErrNone )
        {
        LOGSTRING2("AuthenticationFailed, res: %i", res );
        LOGSTRING2("err: %i", err );
        User::Leave( THttpProvStates::EStatusSignatureFailed );
        }

    CleanupStack::PopAndDestroy(c);
    LOGSTRING( "CNHeadWrapperParser::CheckHeadWrapperL() - done" );
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::ParseUintVar
// -----------------------------------------------------------------------------
//
TInt CNHeadWrapperParser::ParseUintVar(const TDesC8& aVarDes, TUint32& aVar, TUint &aVarLength)
    {
    TUint32 finalValue = 0;
    TInt i;
    for(i=0; i<5; i++)
        {
        if(aVarDes.Length()<=i)
            {
            return KErrCorrupt;
            }

        TUint8 val = aVarDes[i];
        finalValue <<= 7;
        finalValue |= (((TUint32)val)&0x7F);
        if( !(val&0x80 ) )
            {
            break;
            }
        }

    if(i==5)
        {
        return KErrCorrupt;
        }
    else
        {
        aVar = finalValue;
        aVarLength = i+1;
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetHeaders
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CNHeadWrapperParser::GetHeadersL() const
    {
    if( iHeaders.Length() < 1 )
        {
        User::Leave( KErrNotFound );
        }
    return iHeaders;
    }


// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetContentType
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CNHeadWrapperParser::GetContentTypeL() const
    {
    if( !iHttpProvContentType )
        {
        User::Leave( KErrNotFound );
        }
    return iHttpProvContentType->ContentTypeL();
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetContent
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CNHeadWrapperParser::GetContentL() const
    {
    if( iContent.Length() < 1 )
        {
        User::Leave( KErrNotFound );
        }
    return iContent;
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetReportUrlL
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CNHeadWrapperParser::GetReportUrlL() const
    {
    if( !iHttpProvHeaders )
        {
        User::Leave( KErrNotFound );
        }
    return iHttpProvHeaders->GetParamValL( KReportUrl );
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetProvisioningActivation
// -----------------------------------------------------------------------------
//
EXPORT_C TProvisioningActivation CNHeadWrapperParser::GetProvisioningActivation() const
    {
    // The default value is "User selects"
    TProvisioningActivation value( EProvisioningActivationUser );

    if( !iHttpProvHeaders )
        {
        return value;
        }

    TInt err( KErrNone );
    TPtrC8 descValue( KNullDesC8 );
    TRAP( err, descValue.Set( iHttpProvHeaders->GetParamValL( KProvisioningActivation ) ) );
    if( err != KErrNone )
        {
        return value;
        }

    TInt intValue(0);
    TLex8 parser( descValue );
    err = parser.Val( intValue );

    // ignore illegal values
    if( err != KErrNone )
        {
        return value;
        }

    if( intValue < EProvisioningActivationUser || intValue > EProvisioningActivationOff )
        {
        return value;
        }

    // cast
    value = (TProvisioningActivation) intValue;
    return value;
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetUserInteraction
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNHeadWrapperParser::GetUserInteraction() const
    {
    // The default value is EFalse
    TBool value( EFalse );

    if( !iHttpProvHeaders )
        {
        return value;
        }

    TInt err( KErrNone );
    TRAP( err, value = GetBoolParamValueL( KUserInteraction ) );
    return value;
    }

// -----------------------------------------------------------------------------
// CNHeadWrapperParser::GetBoolParamValueL
// -----------------------------------------------------------------------------
//
TBool CNHeadWrapperParser::GetBoolParamValueL( const TDesC8& aParam ) const
    {
    LOGSTRING("CNHeadWrapperParser::GetBoolParamValueL");
    LOGTEXT( aParam );
    const TDesC8& paramVal = iHttpProvHeaders->GetParamValL( aParam );
    LOGTEXT( paramVal );
    if( paramVal.Compare( KStringValueTrue ) == 0 )
        {
        return ETrue;
        }
    return EFalse;        
    }

// End of File  
