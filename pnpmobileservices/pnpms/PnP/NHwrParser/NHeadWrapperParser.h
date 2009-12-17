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


#ifndef NHEADWRAPPER_PARSER_H
#define NHEADWRAPPER_PARSER_H

// INCLUDES
#include <e32base.h>
#include "HttpProvStates.h"

class CHttpProvHeaders;
class CHttpProvContentType;

enum TProvisioningActivation
    {
    EProvisioningActivationUser = 0,
    EProvisioningActivationOn,
    EProvisioningActivationOff
    };

class CNHeadWrapperParser : public CBase
    {
public:
    IMPORT_C ~CNHeadWrapperParser();

    /*!
    Constructor
    Does not make an own copy of the data, so aWrapperData should remain
    in the scope during the lifetime of the CNHeadWrapperParser object.
    @param aWrapperData Pointer to a buffer containing HeadWrapper data. 
    */
    IMPORT_C static CNHeadWrapperParser* NewL( const TPtrC8& aWrapperData );

    /*!
    Parses Oma Content provisioning data from wrapper
    */
    IMPORT_C THttpProvStates::TProvisioningStatus Parse();

    /*!
    @return headers
    Leaves with KErrNotFound if there was no headers in the HeadWrapper.
    */
    IMPORT_C const TDesC8& GetHeadersL() const;
    
    /*!
    @return  content type
    Leaves with KErrNotFound if there was no content type in the HeadWrapper.
    */
    IMPORT_C const TDesC8& GetContentTypeL() const;

    /*!
    @return content data
    */
    IMPORT_C const TDesC8& GetContentL() const;

    /*!
    @return report URL from the provisioned data
    */
    IMPORT_C const TDesC8& GetReportUrlL() const;

    /*!
    @return 
    ETrue if there was no "Provisioning Activation" header
    in the provisioned head wrapper or if the header was set to value "1"
    */
    IMPORT_C TProvisioningActivation GetProvisioningActivation() const;

    /*!
    @return ETrue if there was "User Interaction" header in the
    provisioned head wrapper and it was set to value "1". EFalse
    otherwise.
    */
    IMPORT_C TBool GetUserInteraction() const;

private:
    CNHeadWrapperParser();
    void ConstructL(const TPtrC8& aWrapperData);

    void DoParseL();
    void CheckHeadWrapperL();

    /*
    * Parses Uintvar from beginning of descriptor
    */
    TInt ParseUintVar(const TDesC8& aVarDes, TUint32& aVar, TUint &aVarLength);
    TBool GetBoolParamValueL( const TDesC8& aParam ) const;

private:

    // Wrapper data
    TPtrC8  iWrapperData;

    CHttpProvHeaders*       iHttpProvHeaders;
    CHttpProvContentType*   iHttpProvContentType;

    // headers
    TPtrC8  iHeaders;
    
    // Content type
    TPtrC8  iContentTypeField;

    // Content of wrapper
    TPtrC8  iContent;            
    
    };

#endif // NHEADWRAPPER_PARSER_H
