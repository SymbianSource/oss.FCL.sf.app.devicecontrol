/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A interface definition for reading HDC related parameters
*
*/




#ifndef HDC_TO_PAOS_INTERFACE_H
#define HDC_TO_PAOS_INTERFACE_H

//INCLUDES
#include <e32base.h>

const TInt KMaxHdcVersionStringLength(40); 
const TInt KHdcNonceLength(8); 
_LIT( KHdcDllFileName, "hdcutil.dll" ); 
const TUid KHdcUtilDllUid = { 0x10272D3A };

const TInt KMaxHdcKeyInfoLength(20);
typedef TBuf8<KMaxHdcKeyInfoLength> HdcKeyInfo;
typedef RArray<HdcKeyInfo> HdcKeyInfoList;

const TInt KMaxURLLength(255);
typedef TBuf8<KMaxURLLength> HdcTrustedDomain;
typedef RArray<HdcTrustedDomain> HdcTrustedDomainList;


/*!
MHdcToPaosInterface implementor is assumed to be part of a DLL that can be
loaded dynamically using RLibrary::Load. It should have <KHdcUtilDllUid> as
its uid3 and it should be named <KHdcDllFileName>. There should be a NewLC
implementation in the first ordinal of the DLL that returns a pointer to
a CHdcToPaosInterface derived object to allow dynamic loading.
*/

class CHdcToPaosInterface : public CBase
    {
    public:
    
        /**
        * Returns HDC version string
        * @param aVersion Current version of the HDC. max length of version is 
        *    KMaxHdcVersionStringLength
        * @return KErrNone, KErrArgument
        */
        virtual TInt Version( TDes8& aVersion ) const = 0;

        /**
        * Create and save new nonce. 
        * @param aNonce Nonce. Length of nonce is KHdcNonceLength
        */
        virtual void CreateNewNonceL( TDes8& aNonce ) = 0;
        
        /**
        * Get current nonce value from the database which has been created using
        *    CreateNonceL().
        * @param aNonce Nonce. Length of nonce is KHdcNonceLength
        */
        virtual void NonceL( TDes8& aNonce ) const = 0;
        
        /**
        * Return Keyinfo list array which are defined in HDCKeyInfo.h
        * @param aList Keyinfo list. 0..N keyinfos. See typedefinitions.
        */
        virtual void HdcKeyInfos( HdcKeyInfoList& aList) = 0;
        
        /**
        * Get a list of trusted domains which PAOS can use to determine valid domains.
        * @param aList See typedefinitions.
        **/
        virtual void TrustedDomainsL( HdcTrustedDomainList& aList ) = 0;    
    };

#endif // HDC_TO_PAOS_INTERFACE_H
