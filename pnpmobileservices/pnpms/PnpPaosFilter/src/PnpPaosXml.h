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
* Description:  For checking a PAOS XML document
*
*/



#ifndef PNP_PAOS_XML_H
#define PNP_PAOS_XML_H

#include <e32std.h>
#include <sysutil.h>
#include <xml/parser.h>         // for XML parser
#include <xml/contenthandler.h> // for MContentHandler
#include <http/mhttpdatasupplier.h>
#include <PnpUtilImpl.h>


using namespace Xml;

class CPnpUtil;


/**
For checking a PAOS XML document. Checks that the XML is of right format.
Currently only a request for "SetOfKeys" is allowed.
*/

class CPnpPaosXml :
    public CBase,
    public MContentHandler,
    public MHTTPDataSupplier /* to set the body of a http request*/
    {
public:
    /**
    Possible PAOS request states, currently only ERequestingKeys is available.
    */
    enum TPaosStates
        {
        EPaosStatusUnknown = 0,
        EPaosStatusRequestingPnPKeys,    /* Received a PAOS request for PnP-MS parameters */
        EPaosStatusRequestingHdcKeys    /* Received a PAOS request for HelpDeskConnect parameters */
        };

    static CPnpPaosXml* NewL();
    ~CPnpPaosXml();

    /**
    Construct a response to the PnP PAOS request received,
    the response is put into iPaosResponse buffer.
    */
    void     ConstructPnPPaosResponseL();

    /**
    Construct a response to the HelpDeskConnect PAOS request received,
    the response is put into iPaosResponse buffer.
    */
    void     ConstructHdcPaosResponseL();

    /**
    Construct a response to a PAOS request. The response is put into
    iPaosResponse buffer.
    @param aResponse The literal used in formatting the response. It is assumed
    to have %S directives to be replaced by the given parameter values. Maximum
    number of the directives is 8 as the first occurrence is always replaced by
    XML that contains PAOS message ids.
    @param aParameter1 Descriptor that replaces the second occurrence of %S directive in aResponse.
                        The default value is an empty descriptor.
    @param aParameter2 Descriptor that replaces the third occurrence of %S directive in aResponse.
                        The default value is an empty descriptor.
    @param aParameter3 And so on...
    */
    void ConstructPaosResponseL(
        const TDesC8& aResponse,
        const TDesC8& aParameter1 = KNullDesC8,
        const TDesC8& aParameter2 = KNullDesC8,
        const TDesC8& aParameter3 = KNullDesC8,
        const TDesC8& aParameter4 = KNullDesC8,
        const TDesC8& aParameter5 = KNullDesC8,
        const TDesC8& aParameter6 = KNullDesC8,
        const TDesC8& aParameter7 = KNullDesC8,
        const TDesC8& aParameter8 = KNullDesC8
        );

    /**
    Goes through the PAOS request received from the server. Leaves with 
    KErrArgument if the XML document was not correctly formatted.
    */
    void ParseL( TPaosStates& aPaosRequest );

    /**
    For storing the PAOS request piece by piece.
    @return ETrue if this was the last data part from MHTTPDataSupplier.
    */
    TBool CollectResponseBodyL( MHTTPDataSupplier& aBody );

//#ifndef __SERIES60_ 
    /**
    @return A pointer to a descriptor containing response body.
    */
//    TPtrC8 ResponseBodyL();
//#endif

    /**
    Returns the parameter responseConsumerUrl of the previous PAOS request parsed.
    */
    const TDesC8& GetPaosPostUrlL(){ return *iPaosPostUrlPath; };

    /**
    Returns the parameter referenceMessageId for the constructed PAOS response message
    */
    const TDesC8& GetReferenceMessageId(){ return *iReferenceMessageId; };
    
    /**
    Resets the PAOS request.
    */
    void ResetPaosRequest();

public: // from MContentHandler

    /**
    This method is a callback to indicate the start of the document.
    @param      aDocParam Specifies the various parameters of the document.
    @arg        aDocParam.iCharacterSetName The character encoding of the document.
    @param      aErrorCode is the error code. 
                If this is not KErrNone then special action may be required.
    */
    void OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode );

    /**
    This method is a callback to indicate the end of the document.
    @param      aErrorCode is the error code. 
                If this is not KErrNone then special action may be required.
    */
    void OnEndDocumentL( TInt aErrorCode );

    /**
    This method is a callback to indicate an element has been parsed.
    @param      aElement is a handle to the element's details.
    @param      aAttributes contains the attributes for the element.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnStartElementL( const RTagInfo& aElement,
        const RAttributeArray& aAttributes, TInt aErrorCode );

    /**
    This method is a callback to indicate the end of the element has been reached.
    @param      aElement is a handle to the element's details.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );


    /**
    This method is a callback that sends the content of the element.
    Not all the content may be returned in one go. The data may be sent in chunks.
    When an OnEndElementL is received this means there is no more content to be sent.
    @param      aBytes is the raw content data for the element. 
                The client is responsible for converting the data to the
                required character set if necessary.
                In some instances the content may be binary and must not be converted.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );


    /**
    This method is a notification of the beginning of the scope of
    a prefix-URI Namespace mapping.
    This method is always called before the corresponding OnStartElementL method.
    @param      aPrefix is the Namespace prefix being declared.
    @param      aUri is the Namespace URI the prefix is mapped to.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnStartPrefixMappingL( const RString& aPrefix, const RString& aUri,
                                       TInt aErrorCode );

    /**
    This method is a notification of the end of the scope of a prefix-URI mapping.
    This method is called after the corresponding DoEndElementL method.
    @param      aPrefix is the Namespace prefix that was mapped.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

    /**
    This method is a notification of ignorable whitespace in element content.
    @param      aBytes are the ignored bytes from the document being parsed.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );


    /**
    This method is a notification of a skipped entity. If the parser encounters an 
    external entity it does not need to expand it - it can return the entity as aName 
    for the client to deal with.
    @param      aName is the name of the skipped entity.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

    /**
    This method is a receive notification of a processing instruction.
    @param      aTarget is the processing instruction target.
    @param      aData is the processing instruction data. If empty none was supplied.
    @param      aErrorCode is the error code.
                If this is not KErrNone then special action may be required.
    */
    void OnProcessingInstructionL( const TDesC8& aTarget, const TDesC8& aData,
                                          TInt aErrorCode );

    /**
    This method indicates an error has occurred.
    @param      aError is the error code
    */
    void OnError( TInt aErrorCode );

    /**
    This method obtains the interface matching the specified uid.
    @return     0 if no interface matching the uid is found.
                Otherwise, the this pointer cast to that interface.
    @param      aUid the uid identifying the required interface.
    */
    TAny* GetExtendedInterface( const TInt32 aUid );
    
    /**
    This method is indicates there is no more data in the data supplier to parse.
    If there is more data to parse the Start method should be called once there
    is more data in the supplier to continue parsing.
    */
    virtual void OnOutOfData();

public: // From MHTTPDataSupplier
    /**
    From MHTTPDataSupplier.

    Obtains a data part from the supplier.
    The data is guaranteed to survive until a call is made to ReleaseData().

    @param      aDataPart The data part
    @return     ETrue if this is the last part, EFalse otherwise
    */
    virtual TBool GetNextDataPart( TPtrC8& aDataPart );

    /**
    From MHTTPDataSupplier.

    Releases the current data part being held at the data supplier.
    This call indicates to the supplier that the part is no longer needed,
    and another one can be supplied, if appropriate.
    */
    virtual void ReleaseData();

    /**
    From MHTTPDataSupplier.

    Obtains the overall size of the data being supplied, if known to the
    supplier. Where a body of data is supplied in several parts, this size is
    the sum of all the part sizes. If the size is not known, KErrNotFound is 
    returned; in this case the caller must use the return code of
    GetNextDataPart() to find out when the data is complete.
    @return     A size in bytes, or KErrNotFound if the size is not known.
    */
    virtual TInt OverallDataSize();

    /**
    From MHTTPDataSupplier.
    Resets the data supplier. 
    
    This indicates to the data supplier that it should return to the first part
    of the data. This could be used in a situation where the data consumer has
    encountered an error and needs the data to be supplied afresh. Even if the
    last part has been supplied (i.e. GetNextDataPart() has returned ETrue),
    the data supplier should reset to the first part.
    
    @return     If the supplier cannot reset, it should return an error code,
                otherwise it should return KErrNone, where the reset will be
                assumed to have succeeded.
    */
    virtual TInt Reset();

private:
    CPnpPaosXml();
    void ConstructL();

    /**
    Generates a random character string. The original descriptor contents is lost.
    */
    void GenerateNonceString( TDes8& aString );
    
    /**
    * Fetch product model
    * @since 2.0
    * @return void
    */ 
    void FetchProductModelL( TDes8& aModel );

    /** For parsing XML */
    CParser* iParser;

    /** Buffer for storing a http response (PAOS request) */
    HBufC8* iPaosRequest;
    /** Buffer for storing a http request (PAOS response) */
    HBufC8* iPaosResponse;
    /** The path for posting PAOS response to (responseConsumerURL) */
    HBufC8* iPaosPostUrlPath;
    /** messageID parameter PAOS spec:
    "RECOMMENDED that the value of this attribute have nonce characteristic" */
    HBufC8* iMessageId;
    /** refToMessageID parameter; references to the messageID of the PAOS query received.
    PAOS spec: "RECOMMENDED that the value of this attribute have nonce characteristic" */
    HBufC8* iReferenceMessageId;
    TPaosStates iPaosStatus;
    /** ETrue if there was the parameter responseConsumerURL in the xml,
    EFalse otherwise */
    TBool iResponseConsumerUrlFound;
    /** Set to ETrue if parsing the XML caused a call to MContentHandler::OnError */
    TBool iErrorFound;
    /** An enum value for storing the current xml element */
    TInt iCurrentElement;
};

#endif // PNP_PAOS_XML_H
