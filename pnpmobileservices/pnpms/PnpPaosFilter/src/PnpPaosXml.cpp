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
* Description:  For parsing PAOS XML documents
*
*/



#include <e32std.h>
#include <xml/parser.h> // for compact XML parser
#include <xml/documentparameters.h> // for RDocumentParameters
#include <sysutil.h>
#include <PnpToPaosInterface.h>
#include "HdcToPaosInterface.h"

#include "PnpPaosXml.h"
#include "PnpPaosLogger.h"
#include <PnpProvUtil.h>

const TInt KMessageIdLength(12);
const TInt KInitialResponseLength(512);
const TInt KInitialReferenceMessageIdLength(32);
const TInt KInitialUrlLength(128);

_LIT8( KParserDataType, "text/xml" );
_LIT8( KUriPaosLiberty, "urn:liberty:paos:2003-08" );
_LIT8( KLocalNameRequest, "Request" );
_LIT8( KUriSbLiberty, "urn:liberty:sb:2003-08" );
_LIT8( KLocalNameCorrelation, "Correlation" );
_LIT8( KAttributeResponseConsumerUrl, "responseConsumerURL" );
_LIT8( KPnpMsNokiaUri, "http://pnpms.nokia.com/signkey" );
_LIT8( KAttributeMessageId, "messageID" );
_LIT8( KElementGetKey, "getkey" );
_LIT8( KKeyRequest, "Keyrequest" );
_LIT8( KContentSetOfKeys, "SetOfKeys" );
_LIT8( KContentHdcSetOfKeys, "HdcSetOfKeys" );
_LIT8( KContentHdcSetOfKeys2, "HDCSetOfKeys" );


enum TElementIds
    {
    EElementUnrecognized = 0,
    EElementKeyRequest,
    EElementGetKey
    };

// Panic texts
_LIT( KResponseEmpty, "PnPMS PAOS Response" );

// PAOS data sent to the PnP MS server
_LIT8(
    KPnpPaosResponse,
    "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
      "<soap:Header>"
      "%S" /* Add a KPaosResponseMessageIds here if the request has messageid field */
      "</soap:Header>"
      "<soap:Body>"
        "<pp:Signkey xmlns:pp=\"http://pnpms.nokia.com/signkey\">"
            "<Keyinfo>%S</Keyinfo>"
            "<nonce>%S</nonce>"
            "<mcc>%S</mcc>"
            "<mnc>%S</mnc>"
            "<cmcc>%S</cmcc>"
            "<cmnc>%S</cmnc>"
            "<cVersion>%S</cVersion>"
            "<deviceinfo>%S</deviceinfo>"
        "</pp:Signkey>"
      "</soap:Body>"
    "</soap:Envelope>"
    );

// PAOS data sent to the HelpDeskConnect server
_LIT8(
    KHdcPaosResponse,
    "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
      "<soap:Header>"
      "%S" /* Add a KPaosResponseMessageIds here if the request has messageid field */
      "</soap:Header>"
      "<soap:Body>"
        "<pp:Signkey xmlns:pp=\"http://pnpms.nokia.com/signkey\">"
            "%S" /* Add a list of KHdcKeyInfoElement's here */
            "<nonce>%S</nonce>"
            "<hdcVersion>%S</hdcVersion>"
        "</pp:Signkey>"
      "</soap:Body>"
    "</soap:Envelope>"
    );

_LIT8(
    KHdcKeyInfoElement,
    "<Keyinfo>%S</Keyinfo>"
    );
const TInt KHdcKeyInfoElementLength( 21 );

_LIT8(
    KPaosResponseMessageIds,
    "<sb:Correlation xmlns:sb=\"urn:liberty:paos:2003-08\" "
    "messageID=\"%S\" "
    "refToMessageID=\"%S\"/>"
    );


CPnpPaosXml* CPnpPaosXml::NewL()
    {
    LOGSTRING("CPnpPaosXml::NewL()");
    CPnpPaosXml* self = new (ELeave) CPnpPaosXml();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    LOGSTRING("CPnpPaosXml::NewL() - done");
    return self;
    }

CPnpPaosXml::CPnpPaosXml() :
    iPaosStatus( EPaosStatusUnknown ),
    iResponseConsumerUrlFound( EFalse ),
    iErrorFound( EFalse ),
    iCurrentElement( EElementUnrecognized )
    {
    LOGSTRING("constructor CPnpPaosXml()");
    }

void CPnpPaosXml::ConstructL()
    {
    LOGSTRING("CPnpPaosXml::ConstructL()");

    iParser = Xml::CParser::NewL( KParserDataType, *this );

    LOGSTRING("CPnpPaosXml::ConstructL() - 2");
    iPaosPostUrlPath = HBufC8::NewL( KInitialUrlLength );

    iMessageId = HBufC8::NewL( KMessageIdLength );
    // We do not know the length of refToMessageID until we get it
    // from the service
    iReferenceMessageId = HBufC8::NewL( KInitialReferenceMessageIdLength );
    iPaosResponse = HBufC8::NewL( KInitialResponseLength );

    LOGSTRING("CPnpPaosXml::ConstructL() - done");
    }

CPnpPaosXml::~CPnpPaosXml()
    {
    LOGSTRING("CPnpPaosXml::~CPnpPaosXml()");
    delete iParser;
    delete iPaosRequest;
    delete iPaosResponse;
    delete iMessageId;
    delete iReferenceMessageId;
    delete iPaosPostUrlPath;
    LOGSTRING("CPnpPaosXml::~CPnpPaosXml() - done");
    }

void CPnpPaosXml::OnOutOfData() 
    {
    }

void CPnpPaosXml::ParseL( TPaosStates& aPaosStatus )
    {
    LOGSTRING("CPnpPaosXml::ParseL()");

    // Reset possible previous paos response
    ReleaseData();

    // Reset state variables
    iPaosStatus = EPaosStatusUnknown;
    iErrorFound = EFalse;
    iCurrentElement = EElementUnrecognized;
    iResponseConsumerUrlFound = EFalse;

    LOGSTRING("ParseBeginL()");

    iParser->ParseBeginL();
    Xml::ParseL( *iParser, *iPaosRequest );
    
    aPaosStatus = iPaosStatus; // Store status
    LOGSTRING("ParseL() - done");

    // There has to be a consumer URL in the document
    if( !iResponseConsumerUrlFound )
        {
        LOGSTRING( "responseCosumerURL not found!" );
        User::Leave( KErrArgument );
        }

    // No errors allowed in the XML
    if( iErrorFound )
        {
        LOGSTRING( "Error in the XML!" );
        User::Leave( KErrArgument );
        }

    // Only key requests for PnP-MS or HelpDeskConnect
    // keys (parameters) allowed with this PAOS parser
    switch( aPaosStatus )
        {
    case EPaosStatusRequestingPnPKeys:
        ConstructPnPPaosResponseL();
        break;
    case EPaosStatusRequestingHdcKeys:
        ConstructHdcPaosResponseL();
        break;
    default:
        LOGSTRING( "Invalid PAOS request!" );
        User::Leave( KErrArgument );
        break;
        }

#ifdef LOGGING_ENABLED
    for( TInt i(0); i < iPaosResponse->Length(); i += 128 )
        {
        TPtrC8 logText = iPaosResponse->Right( iPaosResponse->Length() - i );
        LOGTEXT( logText );
        }
#endif

    // Reset state variables
    iPaosStatus = EPaosStatusUnknown;
    iErrorFound = EFalse;
    iCurrentElement = EElementUnrecognized;

    LOGSTRING("CPnpPaosXml::ParseL - done()");
    }

TBool CPnpPaosXml::CollectResponseBodyL( MHTTPDataSupplier& aBody )
    {
    LOGSTRING( "CPnpPaosXml::CollectResponseBodyL" );

    // Store incoming body part
    TPtrC8 dataChunk;
    TBool returnValue = aBody.GetNextDataPart( dataChunk );

#ifdef LOGGING_ENABLED
    for( TInt i(0); i < dataChunk.Length(); i += 128 )
        {
        LOGSTRING( "CPnpPaosXml: MHTTPDataSupplier:" );
        TPtrC8 logText = dataChunk.Right( dataChunk.Length() - i );
        LOGTEXT( logText );
        }
#endif

    if( !iPaosRequest )
        {
        iPaosRequest = HBufC8::NewL( dataChunk.Length() );
        }

    TPtr8 paosRequestPtr = iPaosRequest->Des();
    if( paosRequestPtr.MaxLength() < ( iPaosRequest->Length() + dataChunk.Length() ) )
        {
        LOGSTRING( "Re-allocate response buffer" );
        iPaosRequest = iPaosRequest->ReAllocL( iPaosRequest->Length() + dataChunk.Length() );
        paosRequestPtr.Set( iPaosRequest->Des() );
        }
    paosRequestPtr.Append( dataChunk );

    LOGSTRING( "CPnpPaosXml::CollectResponseBodyL - done" );
    return returnValue;
    }
/*
#ifndef __SERIES60_ 
TPtrC8 CPnpPaosXml::ResponseBodyL()
    {
    LOGSTRING( "CPnpPaosXml::ResponseBodyL" );
    if( !iPaosRequest ) 
        {
        User::Leave( KErrNotFound );
        }
    return *iPaosRequest;
    }
#endif
*/
void CPnpPaosXml::ConstructHdcPaosResponseL()
    {
    LOGSTRING( "CPnpPaosXml::ConstructHdcPaosResponseL" );

    // give empty parameter values in case there is no DLL for HelpDeskConnect
    // -> The service may then make the following assumptions:
    // 1. If there is no PAOS header in the first HTTP GET request -> link: "install HDC"
    // 2. If there is PAOS headers but PAOS response contains empty parameter values -> link: "install HDC"
    // 3. PAOS header and a valid PAOS response -> HDC installed OK, proceed to a HDC trigger file

    RLibrary library;
    const TUidType uid( KNullUid, KNullUid, KHdcUtilDllUid );
    TInt result = library.Load( KHdcDllFileName, uid );
    if( result != KErrNone )
        {
        // if the dll is not found the response should be empty
        LOGSTRING2( "err in loading HDC dll: %i", result );
        ConstructPaosResponseL( KHdcPaosResponse );
        return;
        }
    CleanupClosePushL( library );
    // Function at ordinal 1 is NewLC
    TLibraryFunction entry = library.Lookup(1);
    // Call the function to create new hdc dll object
    CHdcToPaosInterface* hdcUtil = ( CHdcToPaosInterface* ) entry();

    // HDC-nonce
    // create new nonce
    TBuf8<KHdcNonceLength> nonce;
    hdcUtil->CreateNewNonceL( nonce );

    // HDC-version
    TBuf8<KMaxHdcVersionStringLength> version;
    hdcUtil->Version( version );

    // List of key-infos
    HdcKeyInfoList keyInfoList;
    hdcUtil->HdcKeyInfos( keyInfoList );
    TInt keyInfoCount = keyInfoList.Count();

    HBufC8* keyInfos = HBufC8::NewLC(
        keyInfoCount * KMaxHdcKeyInfoLength +            /* space for keyinfos */
        keyInfoCount * KHdcKeyInfoElement().Length() );    /* space for XML elements */
    TPtr8 keyInfosPtr = keyInfos->Des();
    for( TInt i(0); i < keyInfoCount; i++ )
        {
        TBuf8<KMaxHdcKeyInfoLength + KHdcKeyInfoElementLength> keyInfoXmlElement;
        keyInfoXmlElement.Format( KHdcKeyInfoElement, &( keyInfoList[i] ) );
        keyInfosPtr.Append( keyInfoXmlElement );
        }

    LOGSTRING( "CPnpPaosXml::ConstructHdcPaosResponseL format response" );
    // 1. KPaosResponseMessageIds
    // 2. List of KHdcKeyInfoElement's
    // 3. HDC nonce
    // 4. HDC version
    ConstructPaosResponseL( KHdcPaosResponse, *keyInfos, nonce, version );
    LOGSTRING( "CPnpPaosXml::ConstructHdcPaosResponseL response formatted" );

    // RArray must be closed before destructing
    keyInfoList.Close();

    CleanupStack::PopAndDestroy( keyInfos );
    CleanupStack::PopAndDestroy( hdcUtil );
    CleanupStack::PopAndDestroy(); // library.Close()
    }

void CPnpPaosXml::ConstructPnPPaosResponseL()
    {
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL" );

    // Dynamically load the pnp util DLL,
    const TUidType uid( KNullUid, KNullUid, KPnpUtilDllUid );
    RLibrary pnpLibrary;
    TInt result = pnpLibrary.Load( KPnpUtilDllFileName, uid );
    if( result != KErrNone )
        {
        // if the dll is not found the response should be empty
        LOGSTRING2( "err in loading pnp util dll: %i", result );
        ConstructPaosResponseL( KPnpPaosResponse );
        return;
        }
    CleanupClosePushL( pnpLibrary );

    // Function at ordinal 1 is NewPnpUtilLC
    //TLibraryFunction entry = pnpLibrary.Lookup(1);
    // Call the function to create new Pnp util object
    //MPnpToPaosInterface* pnpUtil = ( MPnpToPaosInterface* ) entry();

    CPnpUtilImpl *pnpUtil = CPnpUtilImpl::NewLC();

    // check version
    TBuf8<KMaxVersionStringLength> version;
    TBuf<KMaxVersionStringLength> version16;
    pnpUtil->Version( version16 );
    version.Copy( version16 );
    LOGTEXT( version );
    // BC break between versions NPnPS60-0.10 and newer ones; Version function 
    // should be binary compatible, though.
    // (older versions of pnputil should not be a problem as updating PAOS filter
    // without updating PnP-MS components should be possible only by installing
    // HelpDeskConnect With PAOS filter)
    if( version.Compare( _L8("NPnPS60-0.10") ) == 0 )
        {
        // if the dll is not compatible give an empty response
        LOGSTRING( "Too old version of PnpUtil installed" );
        ConstructPaosResponseL( KPnpPaosResponse );
        CleanupStack::PopAndDestroy( pnpUtil );
        CleanupStack::PopAndDestroy(); // pnpLibrary.Close();
        return;
        }

    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2" );

    // MNCs and MCCs
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.01" );
    pnpUtil->FetchHomeNetworkInfoL();
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.1" );
    TRAPD( err, pnpUtil->FetchNetworkInfoL() );
    if( err != KErrNone )
        {
        LOGSTRING2( "CPnpPaosXml::Could not fetch network info %i", err );
        pnpUtil->SetNetworkMccL( _L("") );
        pnpUtil->SetNetworkMncL( _L("") );
        }

    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.2" );
    RMobilePhone::TMobilePhoneNetworkCountryCode homeMcc = pnpUtil->HomeMccL();
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.3" );
    RMobilePhone::TMobilePhoneNetworkIdentity homeMnc = pnpUtil->HomeMncL();
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.4" );
    RMobilePhone::TMobilePhoneNetworkCountryCode networkMcc = pnpUtil->NetworkMccL();
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.5" );
    RMobilePhone::TMobilePhoneNetworkIdentity networkMnc = pnpUtil->NetworkMncL();
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 2.6" );
    TBuf8<4> hmcc;
    TBuf8<8> hmnc;
    TBuf8<4> nmcc;
    TBuf8<8> nmnc;
    hmcc.Copy( homeMcc );
    hmnc.Copy( homeMnc );
    nmcc.Copy( networkMcc );
    nmnc.Copy( networkMnc );

    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 3" );
    // keyinfo, nonce
    TBuf8<KMaxKeyInfoLength> keyInfo;
    pnpUtil->GetKeyInfoL( keyInfo );
    TBuf8<KNonceLength> nonce;
    TInt timeout(0);
    pnpUtil->CreateNewNonceL( timeout, nonce );
    
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL 4" );
    // product model
    TBuf8<KSysUtilVersionTextLength> deviceinfo;
    FetchProductModelL( deviceinfo );

	// Clear all service activation data in case if service 
	// activation fails
	// This will avoid displaying service specific notes for
	// example email service activation when help-portal service required
	// As of now only Help-portal server sends PAOS request 
	// and NSA server(which provides email service activation) is not 
	// supporting PAOS.
	// If NSA server provides PAOS then these resetting of
	// service activation data to be commented
		
    CPnpProvUtil* prov = CPnpProvUtil::NewLC();
    
    const TUint32 uidval = 0;
    prov->SetApplicationUidL(uidval);
	
	
    TBuf<2> buf(_L(""));
    prov->SetProvAdapterAppIdL(buf);

    CleanupStack::PopAndDestroy();

    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL format response" );
    // 1. Keyinfo
    // 2. Nonce
    // 3. Home MCC
    // 4. Home MNC
    // 5. Network (current) MCC
    // 6. Network MNC
    // 7. Version
    // 8. deviceinfo
    ConstructPaosResponseL( KPnpPaosResponse, keyInfo, nonce, hmcc, hmnc, nmcc, nmnc, version, deviceinfo );
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL response formatted" );
    
    LOGSTRING( "CleanupStack::PopAndDestroy( pnpUtil )" );
    CleanupStack::PopAndDestroy( pnpUtil );
    LOGSTRING( "pnpLibrary.Close()" );
    CleanupStack::PopAndDestroy(); // pnpLibrary.Close()
    LOGSTRING( "CPnpPaosXml::ConstructPnPPaosResponseL - done" );
    }

void CPnpPaosXml::ConstructPaosResponseL(
    const TDesC8& aResponse,
    const TDesC8& aParameter1,
    const TDesC8& aParameter2,
    const TDesC8& aParameter3,
    const TDesC8& aParameter4,
    const TDesC8& aParameter5,
    const TDesC8& aParameter6,
    const TDesC8& aParameter7,
    const TDesC8& aParameter8
    )
    {
    // Construct PAOS message id element if reference message id is available
    HBufC8* messageIds(0);
    if( iReferenceMessageId->Length() )
        {
        LOGSTRING( "iMessageId:" );
        LOGTEXT( *iMessageId );
        LOGSTRING( "iReferenceMessageId:" );
        LOGTEXT( *iReferenceMessageId );

        messageIds = HBufC8::NewLC(
            KPaosResponseMessageIds().Length() +
            iMessageId->Length() +
            iReferenceMessageId->Length() );
        messageIds->Des().Format( KPaosResponseMessageIds, iMessageId, iReferenceMessageId );
        }

    LOGSTRING( "CPnpPaosXml::ConstructPaosResponseL" );
    // Construct a new PAOS response
    TInt responseLength(
        aResponse.Length() +
        aParameter1.Length() +
        aParameter2.Length() +
        aParameter3.Length() +
        aParameter4.Length() +
        aParameter5.Length() +
        aParameter6.Length() +
        aParameter7.Length() +
        aParameter8.Length()
        );

    // Add the length of messageIds buffer to responseLength
    if( messageIds )
        responseLength += messageIds->Length();

    // Expand buffer if needed
    if( iPaosResponse->Des().MaxSize() < responseLength )
        iPaosResponse = iPaosResponse->ReAllocL( responseLength );

    LOGSTRING( "CPnpPaosXml::ConstructPaosResponseL - format" );
    if( messageIds )
        {
        iPaosResponse->Des().Format(
            aResponse,
            messageIds, /* The first parameter in aResponse is assumed to be messageids */
            &aParameter1,
            &aParameter2,
            &aParameter3,
            &aParameter4,
            &aParameter5,
            &aParameter6,
            &aParameter7,
            &aParameter8
            );
        LOGSTRING( "CleanupStack::PopAndDestroy( messageIds )" );
        CleanupStack::PopAndDestroy( messageIds );
        }
    else
        {
        iPaosResponse->Des().Format(
            aResponse,
            &KNullDesC8(),
            &aParameter1,
            &aParameter2,
            &aParameter3,
            &aParameter4,
            &aParameter5,
            &aParameter6,
            &aParameter7,
            &aParameter8
            );
        }
    LOGSTRING( "CPnpPaosXml::ConstructPaosResponseL - done" );
    }

void CPnpPaosXml::ResetPaosRequest()
    {
    delete iPaosRequest;
    iPaosRequest = 0;
    }

TBool CPnpPaosXml::GetNextDataPart( TPtrC8& aDataPart )
    {
    LOGSTRING("CPnpPaosXml::GetNextDataPart");

    if( !iPaosResponse )
        {
        LOGSTRING( "empty response!" );
        // Panics are not shown (on browser process), the browser is just closed
        __ASSERT_ALWAYS( EFalse, User::Panic( KResponseEmpty, KErrNotFound ) );
        }
    aDataPart.Set( *iPaosResponse  );
    return ETrue;
    }

void CPnpPaosXml::ReleaseData()
    {
    LOGSTRING("CPnpPaosXml::ReleaseData");
    // No need to delete our data
    // They are deleted in the destructor and reallocated when needed
    iPaosResponse->Des().Zero();
    iMessageId->Des().Zero();
    iReferenceMessageId->Des().Zero();
    iPaosPostUrlPath->Des().Zero();
    }

TInt CPnpPaosXml::OverallDataSize()
    {
    LOGSTRING("CPnpPaosXml::OverallDataSize");
    if( !iPaosResponse )
        {
        LOGSTRING( "empty response!" );
        // Panics are not shown (on browser process), the browser is just closed
        __ASSERT_ALWAYS( EFalse, User::Panic( KResponseEmpty, KErrNotFound ) );
        }
    return iPaosResponse->Length();
    }

TInt CPnpPaosXml::Reset()
    {
    LOGSTRING("CPnpPaosXml::Reset");
    return KErrNone;
    }

void CPnpPaosXml::OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnStartDocumentL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    // Keep compiler happy
    (void)aDocParam;
    LOGRSTRING( "CharSet: %S", aDocParam.CharacterSetName() );
    }

void CPnpPaosXml::OnEndDocumentL( TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnEndDocumentL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    }

void CPnpPaosXml::OnStartElementL( const RTagInfo& aElement, const RAttributeArray& aAttributes, 
                             TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnStartElementL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }

    RString uri = aElement.Uri();
    RString localName = aElement.LocalName();

#ifdef LOGGING_ENABLED
    RString prefix = aElement.Prefix();
#endif
    LOGRSTRING( "uri: %S", uri );
    LOGRSTRING( "localName: %S", localName );
    LOGRSTRING( "prefix: %S", prefix );

    if( iCurrentElement == EElementKeyRequest )
        {
        if( localName.DesC().Compare( KElementGetKey ) == 0 )
            {
            LOGSTRING( "setting iCurrentElement = EElementGetKey" );
            iCurrentElement = EElementGetKey;
            }
        }
    else if( uri.DesC().Compare( KPnpMsNokiaUri ) == 0 )
        {
        if( localName.DesC().Compare( KKeyRequest ) == 0 )
            {
            LOGSTRING( "setting iCurrentElement = EElementKeyRequest" );
            iCurrentElement = EElementKeyRequest;
            }
        }
    

    for( TInt i(0); i < aAttributes.Count(); i++ )
        {
        RAttribute attribute = aAttributes[i];
        RTagInfo tagInfo = attribute.Attribute();
        RString attributeLocalName = tagInfo.LocalName();
        RString value = attribute.Value();

#ifdef LOGGING_ENABLED
        RString attributeUri = tagInfo.Uri();
        RString attributePrefix = tagInfo.Prefix();
#endif
        LOGRSTRING( "attribute uri: %S", attributeUri );
        LOGRSTRING( "attribute localName: %S", attributeLocalName );
        LOGRSTRING( "attribute prefix: %S", attributePrefix );
        LOGRSTRING( "value: %S", value );

        // According to PAOS spec: "SOAP request message that (provided that the
        // SOAP processor wishes to use the PAOS binding) MUST contain a <paos:Request>
        // SOAP header block."
        // For some reason Futurice uses prefix ns1 (ns1:Request) for a SOAP request.
        // So we allow any prefix for a SOAP request instead of only allowing "paos"
 
        if( localName.DesC().Compare( KLocalNameRequest ) == 0 &&
            uri.DesC().Compare( KUriPaosLiberty ) == 0 )
            {
            LOGSTRING("uri PAOS found");
            const TDesC8& strAttributeLocalName = attributeLocalName.DesC();
            const TDesC8& strValue = value.DesC();

            if( strAttributeLocalName.Compare( KAttributeResponseConsumerUrl ) == 0 )
                {
                LOGSTRING( "PAOS post url found" );
                if( iPaosPostUrlPath->Des().MaxLength() < strValue.Length() )
                    {
                    iPaosPostUrlPath = iPaosPostUrlPath->ReAllocL( strValue.Length() );
                    }
                iPaosPostUrlPath->Des().Copy( strValue );
                LOGSTRING( "iResponseConsumerUrlFound = ETrue" );
                iResponseConsumerUrlFound = ETrue;
                }
            else if( attributeLocalName.DesC().Compare( KAttributeMessageId ) == 0)
                {
                LOGSTRING("messageID found");
                const TDesC8& strValue = value.DesC();
                if( iReferenceMessageId->Des().MaxLength() < strValue.Length() )
                    {
                    iReferenceMessageId = iReferenceMessageId->ReAllocL( strValue.Length() );
                    }
                iReferenceMessageId->Des().Copy( strValue );
                // Generate a random character string for our own messageID
                TPtr8 messageIdPtr = iMessageId->Des();
                GenerateNonceString( messageIdPtr );
                }
            }
        else if( localName.DesC().Compare( KLocalNameCorrelation ) == 0 &&
            uri.DesC().Compare( KUriSbLiberty ) == 0 )
            {
            LOGSTRING("uri SB found");
            if( attributeLocalName.DesC().Compare( KAttributeMessageId ) == 0)
                {
                LOGSTRING("messageID found");
                const TDesC8& strValue = value.DesC();
                iReferenceMessageId = iReferenceMessageId->ReAllocL( strValue.Length() );
                iReferenceMessageId->Des().Copy( strValue );
                // Generate a random character string for our own messageID
                TPtr8 messageIdPtr = iMessageId->Des();
                GenerateNonceString( messageIdPtr );
                }
            }
        }
    LOGSTRING("CPnpPaosXml::OnStartElementL - done");
    }

void CPnpPaosXml::GenerateNonceString( TDes8& aString )
    {
    LOGSTRING("CPnpPaosXml::GenerateNonceString");
    aString.Zero();

    _LIT8( KTemp, "123456abcdef");
    if( aString.MaxLength() <= KTemp().Length() )
        {
        aString.Copy( KTemp().Left( aString.MaxLength() ) );
        }
    else
        {
        aString.Copy( KTemp );
        }

    LOGSTRING("CPnpPaosXml::GenerateNonceString - done");
    }

void CPnpPaosXml::OnEndElementL( const RTagInfo& aElement, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnEndElementL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }

    RString uri = aElement.Uri();
    RString localName = aElement.LocalName();
    RString prefix = aElement.Prefix();

    TBuf<255> buff;
    buff.Copy( uri.DesC().Left(255) );
    LOGSTRING2( "uri: %S", &buff );
    buff.Copy( localName.DesC().Left(255) );
    LOGSTRING2( "localName: %S", &buff );
    buff.Copy( prefix.DesC().Left(255) );
    LOGSTRING2( "prefix: %S", &buff );

    if( iCurrentElement == EElementGetKey )
        {
        // Ending Get key element?
        if( localName.DesC().Compare( KElementGetKey ) == 0 )
            {
            LOGSTRING( "setting iCurrentElement = EElementKeyRequest" );
            // Must be inside key request
            iCurrentElement = EElementKeyRequest;
            }
        }
    else if( iCurrentElement == EElementKeyRequest )
        {
        // Ending Key request element?
        if( uri.DesC().Compare( KPnpMsNokiaUri ) == 0 )
            {
            if( localName.DesC().Compare( KKeyRequest ) == 0 )
                {
                LOGSTRING( "setting iCurrentElement = EElementUnrecognized" );
                iCurrentElement = EElementUnrecognized;
                }
            }
        }
    }

void CPnpPaosXml::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnContentL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    LOGTEXT( aBytes );

    if( iCurrentElement == EElementGetKey )
        {
        LOGSTRING( "Element GetKey" );
        if( aBytes.Compare( KContentSetOfKeys ) == 0 )
            {
            LOGSTRING("EPaosStatusRequestingPnPKeys");
            iPaosStatus = EPaosStatusRequestingPnPKeys;
            }
        else if( aBytes.Compare( KContentHdcSetOfKeys ) == 0 || aBytes.Compare( KContentHdcSetOfKeys2 ) == 0 )
            {
            LOGSTRING("EPaosStatusRequestingHdcKeys");
            iPaosStatus = EPaosStatusRequestingHdcKeys;
            }
        }
    }

void CPnpPaosXml::OnStartPrefixMappingL( const RString& /*aPrefix*/, const RString& /*aUri*/,
                                   TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnStartPrefixMappingL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    }

void CPnpPaosXml::OnEndPrefixMappingL( const RString& /*aPrefix*/, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnEndPrefixMappingL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    }

void CPnpPaosXml::OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnIgnorableWhiteSpaceL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    LOGTEXT( aBytes );
    // Keep compiler happy
    (void)aBytes;
    }

void CPnpPaosXml::OnSkippedEntityL( const RString& aName, TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnSkippedEntityL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    LOGRSTRING( "name: %S", aName );
    // Keep compiler happy
    (void)aName;
    }

void CPnpPaosXml::OnProcessingInstructionL( const TDesC8& aTarget, const TDesC8& aData, 
                                      TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnProcessingInstructionL: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        User::Leave( KErrArgument );
        }
    // Keep compiler happy
    (void)aTarget;
    (void)aData;
    LOGSTRING( "target:" );
    LOGTEXT( aTarget );
    LOGSTRING( "data:" );
    LOGTEXT( aData );
    }

void CPnpPaosXml::OnError( TInt aErrorCode )
    {
    LOGSTRING2( "CPnpPaosXml::OnError: %i", aErrorCode );
    if( aErrorCode != KErrNone )
        {
        iErrorFound = ETrue;
        }
    }

TAny* CPnpPaosXml::GetExtendedInterface( const TInt32 aUid )
    {
    LOGSTRING2( "CPnpPaosXml::GetExtendedInterface: %i", aUid );
    // Keep compiler happy
    (void)aUid;
    return 0;
    }
    
void CPnpPaosXml::FetchProductModelL( TDes8& aModel )
    {
    HBufC* tmpVersion = HBufC::NewLC( KSysUtilVersionTextLength );
    TPtr ptr( tmpVersion->Des() );
    User::LeaveIfError( SysUtil::GetSWVersion( ptr ) );
    LOGTEXT(ptr);

    _LIT(KVerStrStart,"V ");
    _LIT(KVerStrEnd,"\n");

    TInt pos1 = tmpVersion->Find(KVerStrStart);
    TInt pos2 = tmpVersion->Find(KVerStrEnd);
    TInt verlen = ((TDesC)(KVerStrStart)).Length();
    
    if( pos1==KErrNotFound) // Version does not start with "V "
        {
        pos1=0;
        verlen=0;
        }

    if(    (pos1!=KErrNotFound) 
        && (pos2!=KErrNotFound) 
        && (pos2 > (pos1 + verlen) ) 
      )
        {
        TPtrC ptrSeek(ptr);
        pos1 = ptrSeek.Find(KVerStrEnd);
        if(pos1>=0)
            {
            ptrSeek.Set(ptrSeek.Mid(pos1+1));
            pos1 = ptrSeek.Find(KVerStrEnd);
            if( pos1 >= 0 )
                {
                ptrSeek.Set(ptrSeek.Mid(pos1+1));
                pos1 = ptrSeek.Find(KVerStrEnd);
                if( pos1 < 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1));
                    aModel.Copy(ptrSeek);
                    }
                else if( pos1 > 0 )
                    {
                    ptrSeek.Set(ptrSeek.Mid(1,pos1-1));
                    aModel.Copy(ptrSeek);
                    }
                LOGTEXT(aModel);
                }
            }
        }
    CleanupStack::PopAndDestroy();
    }
// End of File  
