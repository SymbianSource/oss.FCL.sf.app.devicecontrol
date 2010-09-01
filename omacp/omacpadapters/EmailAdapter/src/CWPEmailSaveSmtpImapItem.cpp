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
* Description:  Saves SMTP IMAP settings
*
*/


// INCLUDE FILES
#include    "CWPEmailSaveSmtpImapItem.h"
#include    "CWPEmailItem.h"
#include    "WPEmail.hrh"
#include    "EmailAdapter.pan"
#include    "Cwplog.h"
#include    <CWPParameter.h>
#include    <imapset.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::CSaveSmtpImapItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSaveSmtpImapItem::CSaveSmtpImapItem( CWPEmailItem* aSmtpItem, 
                                      CWPEmailItem* aImapItem )
                                      : iSmtpItem( aSmtpItem ),
                                        iImapItem( aImapItem )
    {
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSaveSmtpImapItem::ConstructL()
    {
    CWPEmailSaveItem::ConstructL();
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSaveSmtpImapItem* CSaveSmtpImapItem::NewLC( CWPEmailItem* aSmtpItem, 
                                             CWPEmailItem* aImapItem )
    {
    __ASSERT_DEBUG(aSmtpItem && aImapItem , Panic(EMailAdapterNullPointer));
    CSaveSmtpImapItem* self = new( ELeave ) CSaveSmtpImapItem( aSmtpItem,
                                                               aImapItem );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
CSaveSmtpImapItem::~CSaveSmtpImapItem()
    {
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::SaveL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSaveSmtpImapItem::SaveL( CMsvSession* aSession )
    {  
    SaveMailSettingsToMessageServerL( *iSmtpItem, *iImapItem, EFalse, aSession );
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::SummaryText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpImapItem::ValidateL()
    {
    // Check items
    return ( ValidateSmtpItemL() && ValidateImapItemL() && 
        iSmtpItem->ParameterValue(EWPParameterProviderID) == 
        iImapItem->ParameterValue(EWPParameterProviderID));
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::SummaryText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSaveSmtpImapItem::SummaryText()
    {
    // return default name for settings 
    return CWPEmailSaveItem::SummaryText();
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::ValidateSmtpItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpImapItem::ValidateSmtpItemL()
    {
    const TDesC& ownAddress = iSmtpItem->ParameterValue(EWPParameterFrom);
    const TDesC& serverAddress = iSmtpItem->ParameterValue(EWPParameterAddr);

    if (ownAddress.Length() > 0
        && ownAddress.Length() <= KIacMaxAddressLength 
        && KErrNone == ValidateServerAddress(serverAddress)
        && serverAddress.Length() <= KIacMaxSendHostLength )
        {
        LOG1("CSaveSmtpImapItem::ValidateSmtpItemL %d", IsValidEmailAddressL(ownAddress));
        return (IsValidEmailAddressL(ownAddress));
        }
    LOG("CSaveSmtpImapItem::ValidateSmtpItemL: Not Valid");
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSaveSmtpImapItem::ValidateImapItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSaveSmtpImapItem::ValidateImapItemL()
    {
    const TDesC& userName = iImapItem->ParameterValue(EWPParameterAAuthName);
    const TDesC& password = iImapItem->ParameterValue(EWPParameterAAuthSecret);
    const TDesC& serverAddress = iImapItem->ParameterValue(EWPParameterAddr);
    const TDesC& imapName = iImapItem->ParameterValue(EWPParameterName);
    const TDesC& smtpName = iSmtpItem->ParameterValue(EWPParameterName);
    
    // Replace default name if found
	if (imapName.Length() > 0)
        {
        SetDefaultNameL(imapName);
        }
    else
        {
        if (smtpName.Length() > 0)
            {
            SetDefaultNameL(smtpName);        
            }
        }

    if (KErrNone == ValidateServerAddress(serverAddress)
        && serverAddress.Length() <= KIacMaxRecHostLength 
        && userName.Length() <= KIacMaxRemMbxUserNameLength 
        && password.Length() <= KIacMaxPasswdLength && imapName.Length()<=30 && smtpName.Length()<=30 )
        {
        LOG("CSaveSmtpImapItem::ValidateImapItemL: Valid");
        return ETrue;
        }
    LOG("CSaveSmtpImapItem::ValidateImapItemL: Not Valid");
    return EFalse;
    }
    
//  End of File  
