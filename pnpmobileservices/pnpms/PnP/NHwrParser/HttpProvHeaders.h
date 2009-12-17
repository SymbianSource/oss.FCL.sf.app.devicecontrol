/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __HTTPPROVHEADERS_H__
#define __HTTPPROVHEADERS_H__

#include <e32base.h>

_LIT8( KReportUrl, "Report-URL" ); 
_LIT8( KSignatureValue, "SignValue" );
_LIT8( KDigestValue, "DigValue" );
_LIT8( KProvisioningActivation, "Provisioning-Activation" );
_LIT8( KUserInteraction, "User-Interaction" );
_LIT8( KStringValueTrue, "1" );

class CHttpProvHeaders : public CBase
{
public:

/*!
  @function NewL
   
  @discussion Create a CHttpProvHeaders object
  @param aHeaders A descriptor containing headers of Nokia HeadWrapper.
  Should remain in the scope during the lifetime of the CHttpProvHeaders object
  @result a pointer to the created instance of CHttpProvHeaders
  */
    static CHttpProvHeaders* NewL(const TDesC8& aHeaders);

/*!
  @function NewLC
   
  @discussion Create a CHttpProvHeaders object
  @param aHeaders A descriptor containing headers of Nokia HeadWrapper.
  Should remain in the scope during the lifetime of the CHttpProvHeaders object
  @result a pointer to the created instance of CHttpProvHeaders
  */
    static CHttpProvHeaders* NewLC(const TDesC8& aHeaders);

/*!
  @function ~CHttpProvHeaders
  
  @discussion Destroy the object and release all memory objects
  */
    ~CHttpProvHeaders();

    const TDesC8& GetParamValL( const TDesC8& aName ) const;

private:

/*!
  @function CHttpProvHeaders

  @discussion Constructs this object
  */
    CHttpProvHeaders();

/*!
  @function ConstructL

  @discussion Performs second phase construction of this object
  */
    void ConstructL(const TDesC8& aHeaders);

    void ParseL( const TDesC8& aHeaders  );
    void GetLineL( const TDesC8& aBuffer, TPtrC8& aLine, TInt& aNextLinePos );
    void HandleHeaderLineL( const TDesC8& aLine );
    void SetParamL( const TDesC8& aName, const TPtrC8& aVal );
    void SplitLineL( const TDesC8& aLine, TChar aSplitBy, TPtrC8& aLeft, TPtrC8& aRight );


private:
    // Member variables
    TPtrC8 iReportUrl;
    TPtrC8 iSignatureValue;
    TPtrC8 iDigestValue;
    TPtrC8 iProvisioningActivation;
    TPtrC8 iUserInteraction;
};

#endif // __HTTPPROVHEADERS_H__

