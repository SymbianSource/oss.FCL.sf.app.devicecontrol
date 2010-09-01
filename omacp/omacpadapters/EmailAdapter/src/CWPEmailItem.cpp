/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Email settin data object
*
*/



// INCLUDE FILES
#include "CWPEmailItem.h"
#include "CWPCharacteristic.h"
#include "CWPParameter.h"
#include "EmailAdapter.pan"
#include "WPEmail.hrh"
#include "Cwplog.h"
#include    "../../../group/omacplogger.h"


// CONSTANTS
const TInt KArrayGranularity = 10;



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPEmailItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPEmailItem* CWPEmailItem::NewL()
	{
	CWPEmailItem* self = CWPEmailItem::NewLC(); 
	CleanupStack::Pop(self);
	return self;
	}

CWPEmailItem* CWPEmailItem::NewLC()
	{
	CWPEmailItem* self = new(ELeave) CWPEmailItem; 
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// Destructor
CWPEmailItem::~CWPEmailItem()
	{
	delete iEmailSettingParams;
	}
// Constructor
CWPEmailItem::CWPEmailItem()
	{
	}
// -----------------------------------------------------------------------------
// CWPEmailItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPEmailItem::ConstructL()
	{
	// average setting contains 10 items
	iEmailSettingParams = new(ELeave) CArrayPtrFlat<CWPParameter>(
        KArrayGranularity);
	}


const TDesC& CWPEmailItem::ParameterValue(TInt aParamID) const
    {
	TInt count = iEmailSettingParams->Count();
	for (TInt i=0; i < count; ++i)
		{
		if ( iEmailSettingParams->At(i)->ID() == aParamID )
			{
			return iEmailSettingParams->At(i)->Value();
			}
		}
    return KNullDesC;
    }


HBufC* CWPEmailItem::AssociatedIapNameL()
    {    
    HBufC* string = NULL;
    
    if ( iNapDef )
        {
        // We have only 1 IAP
        CArrayFix<TPtrC>* name = new(ELeave) CArrayFixFlat<TPtrC>(1);
        CleanupStack::PushL(name);
        
        // iap name
        iNapDef->ParameterL(EWPParameterName, name);
        
        if (name->Count() > 0)
            {
            // IAP name is found from index 0
            string = name->At(0).AllocL();
            }
        
        CleanupStack::PopAndDestroy(); // name
        }
       
	return string;
    }

TUint32 CWPEmailItem::AssociatedIapId()
    {
    TPckgBuf<TUint32> pckg;
    pckg() = 0;

    if ( iNapDef )
        {
        const TDesC8& data = iNapDef->Data();
        pckg.Copy( data );	
        }
	
    return pckg();
    }

TBool CWPEmailItem::IsValid() const
    {
    // Check AppID and IAP
    // if IAP not defined, then  system tries to use default IAP from CommsDB.
    const TDesC& appID = ParameterValue(EWPParameterAppID);
    if ( appID == KEmailSettingPortSMTP ||
        appID == KEmailSettingPortPOP ||
        appID == KEmailSettingPortIMAP )
        {
        LOG("CWPEmailItem::IsValid");
        return ETrue;
        }
    LOG("CWPEmailItem::IsNotValid");
    return EFalse;
    }

void CWPEmailItem::VisitL(CWPCharacteristic& aCharacteristic)
	{
	switch ( aCharacteristic.Type() )
		{	
		case KWPAppAddr:
			// FALLTROUGH
		case KWPAppAuth:
            // FALLTROUGH
		case KWPPort:
			aCharacteristic.AcceptL( *this );
            LOG1("CWPEmailItem::AcceptL: CWPCharacteristic Type: %d", 
                aCharacteristic.Type());
			break;
	
		default:
            LOG1("CWPEmailItem::Ignore: CWPCharacteristic Type: %d", 
                aCharacteristic.Type());
			// do nothing
			break;
		}
	
	}


void CWPEmailItem::VisitL(CWPParameter& aParameter)
	{
	switch( aParameter.ID() )
		{
        // read these parameters
        case EWPParameterAppID:
        case EWPParameterName:
        case EWPParameterProviderID:            
		case EWPParameterAddr:
		case EWPParameterPortNbr:
		case EWPParameterService:
		case EWPParameterAAuthType:
		case EWPParameterAAuthName:
		case EWPParameterAAuthSecret:
        case EWPParameterFrom:
			iEmailSettingParams->AppendL(&aParameter);
            LOG2("CWPEmailItem::AppendL: CWPParameter id: %d value: %S", 
                aParameter.ID(), &aParameter.Value());
			break;
			
        case EWPNamedParameter:
            {
            TPtrC namedParam(aParameter.Name());
            LOGSTRING2( "[Provisioning] CWPEmailItem::VisitL: Parameter Name, %i", namedParam);
            
            if (namedParam.CompareF(KParamEMN) == 0 || namedParam.CompareF(
                    KParamCS) == 0 || namedParam.CompareF(KParamLMS) == 0
                    || namedParam.CompareF(KParamPOLL) == 0
                    || namedParam.CompareF(KParamONLINE) == 0)
                {
                iEmailSettingParams->AppendL(&aParameter);
				LOG2("CWPEmailItem::VisitL: Named Parameter %S: value: %S", &aParameter.Name(),
                        &aParameter.Value());
                }
            break;
            }
		 
		default:
            LOG2("CWPEmailItem::Ignore: CWPParameter id: %d value: %S",
                aParameter.ID(), &aParameter.Value());
			// Do nothing
			break;
		}

	}

void CWPEmailItem::VisitLinkL(CWPCharacteristic& aCharacteristic )
    {
    if ( aCharacteristic.Type() ==  KWPNapDef )
        {
        // Link to the IAP settings 
        iNapDef = &aCharacteristic;
        LOG("iNapDef is set");
        }
	}

const TDesC& CWPEmailItem::ParameterValue(const TDesC& aParamName)
    {
    TInt count = iEmailSettingParams->Count();
    for (TInt i=0; i < count; ++i)
        {
        if ( iEmailSettingParams->At(i)->Name().CompareF(aParamName) == 0 )
            {
            return iEmailSettingParams->At(i)->Value();
            }
        }
    return KNullDesC;
    }

//  End of File  
