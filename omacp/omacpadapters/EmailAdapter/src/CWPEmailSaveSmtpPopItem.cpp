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
* Description:  Saves SMTP POP settings
*
*/


// INCLUDE FILES
#include    "CWPEmailSaveSmtpPopItem.h"
#include    "CWPEmailItem.h"
#include    "EmailAdapter.pan"
#include    "WPEmail.hrh"
#include    "Cwplog.h"
#include    <CWPParameter.h>
#include    <pop3set.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::CSaveSmtpPopItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSaveSmtpPopItem::CSaveSmtpPopItem( CWPEmailItem* aSmtpItem, 
                                    CWPEmailItem* aPopItem)
                                    : iSmtpItem( aSmtpItem ), 
                                      iPopItem( aPopItem ) 
    {
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSaveSmtpPopItem::ConstructL()
    {
    CWPEmailSaveItem::ConstructL();
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSaveSmtpPopItem* CSaveSmtpPopItem::NewLC( CWPEmailItem* aSmtpItem, 
                                           CWPEmailItem* aPopItem )
    {
    __ASSERT_DEBUG(aSmtpItem && aPopItem, Panic(EMailAdapterNullPointer));
    CSaveSmtpPopItem* self = new( ELeave ) CSaveSmtpPopItem( aSmtpItem,
                                                             aPopItem );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
CSaveSmtpPopItem::~CSaveSmtpPopItem()
    {
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::SaveL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaveSmtpPopItem::SaveL(CMsvSession* aSession)
    {    
    SaveMailSettingsToMessageServerL( *iSmtpItem, *iPopItem, ETrue, aSession );
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::SummaryText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSaveSmtpPopItem::SummaryText()
    {
    return CWPEmailSaveItem::SummaryText();
    }
    
// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::ValidateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpPopItem::ValidateL()
    {
    __ASSERT_DEBUG(iPopItem && iSmtpItem, Panic(EMailAdapterNullPointer));

    // validate items and Provider ID
    return (ValidatePopItemL() && ValidateSmtpItemL() && 
        iSmtpItem->ParameterValue(EWPParameterProviderID) == 
        iPopItem->ParameterValue(EWPParameterProviderID));
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::ValidateSmtpItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpPopItem::ValidateSmtpItemL()
    {
    const TDesC& ownAddress = iSmtpItem->ParameterValue(EWPParameterFrom);
    const TDesC& serverAddress = iSmtpItem->ParameterValue(EWPParameterAddr);

    if (ownAddress.Length() > 0
        && ownAddress.Length() <= KIacMaxAddressLength 
        && KErrNone == ValidateServerAddress(serverAddress)
        && serverAddress.Length() <= KIacMaxSendHostLength )
        {
        LOG1("CSaveSmtpPopItem::ValidateSmtpItemL %d", IsValidEmailAddressL(ownAddress));
        return IsValidEmailAddressL(ownAddress);
        }
    LOG("CSaveSmtpPopItem::ValidateSmtpItemL: Not Valid");
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSaveSmtpPopItem::ValidatePopItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpPopItem::ValidatePopItemL()
    {
    const TDesC& userName = iPopItem->ParameterValue(EWPParameterAAuthName);
    const TDesC& password = iPopItem->ParameterValue(EWPParameterAAuthSecret);
    const TDesC& serverAddress = iPopItem->ParameterValue(EWPParameterAddr);
    const TDesC& popName = iPopItem->ParameterValue(EWPParameterName);
    const TDesC& smtpName = iSmtpItem->ParameterValue(EWPParameterName);

    // Replace default name if found
	if (popName.Length() > 0)
        {
        SetDefaultNameL(popName);
        }
    else
        {
        if (smtpName.Length() > 0)
            {
            SetDefaultNameL(smtpName);        
            }
        }
    // Check max/min lenghts
    	if (KErrNone == ValidateServerAddress(serverAddress)
        && serverAddress.Length() <= KIacMaxRecHostLength 
        && userName.Length() <= KIacMaxRemMbxUserNameLength 
        && password.Length() <= KIacMaxPasswdLength && popName.Length() <= 30 &&  smtpName.Length() <= 30)
        {        	
        LOG("CSaveSmtpPopItem::ValidatePopItemL: Valid");
        return ETrue;
        }
    LOG("CSaveSmtpPopItem::ValidatePopItemL: Not Valid");
    return EFalse;
    }
    
//  End of File  
