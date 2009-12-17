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
* Description:  Class for handling provisioning message
*
*/



// INCLUDE FILES
#include "CWPEmailAdapter.h"
#include "CWPEmailItem.h"
#include "CWPEmailSaveSmtpImapItem.h"
#include "CWPEmailSaveSmtpPopItem.h"
#include "EmailAdapter.pan"  // Panic codes
#include "WPEmail.hrh"
#include <wpemailadapterresource.rsg>
#include "Cwplog.h"

#include <WPAdapterUtil.h> // Adapter utils
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>

#include <commdb.h>

// CONSTANTS
const TInt KFirstArrayIndex = 0;
const TInt KArrayGranularity = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPEmailAdapter::CWPEmailAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPEmailAdapter::CWPEmailAdapter() : CWPAdapter()
	{
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::ConstructL()
    {
    iSession = CMsvSession::OpenSyncL(*this);
    
    // Usually 1 or 2 settings.
    iEmailSettings = new(ELeave) CArrayPtrFlat<CWPEmailItem>(KArrayGranularity);
    
    iEmailSettingPair = 
                 new(ELeave) CArrayPtrFlat<CWPEmailSaveItem>(KArrayGranularity);
        
    TFileName fileName;
    Dll::FileName( fileName );
    iTitle = WPAdapterUtil::ReadHBufCL( fileName, 
                                        KAdapterName, 
                                        R_QTN_SM_TITLE_EMAIL_SETTINGS );
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPEmailAdapter* CWPEmailAdapter::NewL()
	{
	CWPEmailAdapter* self = new(ELeave) CWPEmailAdapter; 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPEmailAdapter::~CWPEmailAdapter()
	{
    delete iSession;
    delete iCommsDb;
    delete iCurrentItem;
    delete iTitle;

    if ( iEmailSettings )
        {
        iEmailSettings->ResetAndDestroy();
        delete iEmailSettings;
        }
    if ( iEmailSettingPair )
        {
        iEmailSettingPair->ResetAndDestroy();
        delete iEmailSettingPair;
        }
	}
// -----------------------------------------------------------------------------
// CWPEmailAdapter::AcceptedSettingGroup
// -----------------------------------------------------------------------------
//
TBool CWPEmailAdapter::AcceptedSettingGroup()
    {
    return ( iCurrentItem->IsValid() );
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::AddSmtpImapSettingsL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::AddSmtpImapSettingsL(	CWPEmailItem* aSmtpItem, 
                                            CWPEmailItem* aImapItem )
    {
    CWPEmailSaveItem* settingPair = CSaveSmtpImapItem::NewLC( aSmtpItem, 
                                                              aImapItem );
    if ( settingPair->ValidateL() )
        {
        iEmailSettingPair->AppendL( settingPair );
        CleanupStack::Pop(); //settingPair
        }
    else
        {
        //not valid  so delete
        CleanupStack::PopAndDestroy(); //settingPair
        }
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::AddSmtpPopSettingsL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::AddSmtpPopSettingsL( CWPEmailItem* aSmtpItem, 
                                           CWPEmailItem* aPopItem )
    {
    CWPEmailSaveItem* settingPair = CSaveSmtpPopItem::NewLC( aSmtpItem, 
                                                             aPopItem );
    if ( settingPair->ValidateL() )
        {
        iEmailSettingPair->AppendL(settingPair);
        CleanupStack::Pop(); //settingPair
        }
    else
        {
        //not valid  so delete
        CleanupStack::PopAndDestroy(); //settingPair
        }
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::CreateSettingsL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::CreateSettingsL( CWPEmailItem* aItem )
    {
    // All settings
    TInt count = iEmailSettings->Count();
    const TDesC& newItemID = aItem->ParameterValue( EWPParameterAppID );

    for (TInt i(KFirstArrayIndex); i < count; ++i)
        {        
        const TDesC& appId = iEmailSettings->At(i)->
            ParameterValue( EWPParameterAppID );

        // SMTP item
		if ( newItemID == KEmailSettingPortSMTP)
			{
            if ( appId == KEmailSettingPortIMAP )
				{
				AddSmtpImapSettingsL( aItem, iEmailSettings->At(i) );
				}
			if ( appId == KEmailSettingPortPOP )
				{
				AddSmtpPopSettingsL( aItem, iEmailSettings->At(i) );
				}
			}
		// POP/IMAP item
		else
			{
			if ( appId == KEmailSettingPortSMTP )
				{
 				if ( newItemID == KEmailSettingPortIMAP )
					{
					AddSmtpImapSettingsL( iEmailSettings->At(i), aItem );
					}
				if ( newItemID == KEmailSettingPortPOP )
					{
					AddSmtpPopSettingsL( iEmailSettings->At(i), aItem );
					}
				}			
			}
		}
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::HandleSessionEventL( TMsvSessionEvent /*aEvent*/, 
                                           TAny* /*aArg1*/, 
                                           TAny* /*aArg2*/, 
                                           TAny* /*aArg3*/ )
    {
	// From MMsvSessionObserver. Ignore session events
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::ItemCount
// -----------------------------------------------------------------------------
//
TInt CWPEmailAdapter::ItemCount() const
    {
    return iEmailSettingPair->Count();
    }
// -----------------------------------------------------------------------------
// CWPEmailAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPEmailAdapter::SummaryTitle( TInt /*aIndex*/ ) const
	{
    // title text is always same
	return *iTitle;
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CWPEmailAdapter::SummaryText( TInt aIndex ) const
	{
    if ( ItemCount() == 0 )
        {
        return KNullDesC;
        }
	return iEmailSettingPair->At(aIndex)->SummaryText();
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::SaveL( TInt aItem )
	{
    __ASSERT_DEBUG(aItem < ItemCount() && 
        aItem >= KFirstArrayIndex, Panic( EMailAdapterArgument ));
	
    // Create the commsdb when the save is initiated first time.
	// This stays alive until the adapter is destroyed.
	if ( !iCommsDb )
		{
		iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
		}    
    User::LeaveIfNull(iCommsDb);
    iEmailSettingPair->At(aItem)->SetCommsDataBase( *iCommsDb );
    iEmailSettingPair->At(aItem)->SaveL( iSession );
	}
	
// -----------------------------------------------------------------------------
// CWPEmailAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPEmailAdapter::CanSetAsDefault( TInt aItem ) const
    {
    // always set first setting as default
    return (aItem == KFirstArrayIndex && ItemCount());
    }
    
// -----------------------------------------------------------------------------
// CWPEmailAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::SetAsDefaultL( TInt /*aItem*/ )
    {
    // set always first setting as default
    iEmailSettingPair->At(KFirstArrayIndex)->SetAsDefaultL();
    }
    
// -----------------------------------------------------------------------------
// CWPEmailAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPEmailAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CWPEmailAdapter::ContextExtension( MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWPEmailAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	TInt type = aCharacteristic.Type();    
	// Are we dealing with application specific data.
    if ( type == KWPApplication ) 
		{
        iCurrentItem = CWPEmailItem::NewL();
        aCharacteristic.AcceptL( *iCurrentItem );
        
        if( AcceptedSettingGroup() )
            {
			CreateSettingsL( iCurrentItem );
            iEmailSettings->AppendL( iCurrentItem );
            }
        else
            {
            delete iCurrentItem;
            }
        iCurrentItem = NULL;
        }
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::VisitL( CWPParameter& /*aParameter*/)
	{
	// parameters are handled in the CWPEmailItem.
	}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::VisitLinkL( CWPCharacteristic& /*aParameter*/)
    {
    // visit link is handled in the CWPEmailItem.
    }
    
// -----------------------------------------------------------------------------
// CWPEmailAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CWPEmailAdapter::SaveDataL( TInt aIndex ) const
    {
	//	From MWPContextExtension
    return iEmailSettingPair->At(aIndex)->SaveData();
    }
    
// -----------------------------------------------------------------------------
// CWPEmailAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CWPEmailAdapter::DeleteL( const TDesC8& aSaveData )
    {
    //	From MWPContextExtension
    TPckgBuf<CWPEmailSaveItem::TSettingId> uids;
    uids.Copy( aSaveData );

    TInt32 smtpId = uids().iSmtpId;
    TInt32 popimapId = uids().iPopImapId;
    iSession->RemoveEntry( popimapId );
    // removing POP/IMAP entry removes also 
    // SMTP entry, but lets make sure it is deleted
    iSession->RemoveEntry(smtpId);
    
    LOG2("CWPEmailAdapter::DeleteL, popimapId: %d, smtpId: %d", popimapId, smtpId);
    }
    
// -----------------------------------------------------------------------------
// CWPEmailAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CWPEmailAdapter::Uid() const
    {
    return iDtor_ID_Key.iUid; 
    }

//  End of File  
