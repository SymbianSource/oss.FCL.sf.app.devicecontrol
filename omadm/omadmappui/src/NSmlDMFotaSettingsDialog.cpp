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
* Description:  Methods For FOTA Setting Dialog
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include <aknradiobuttonsettingpage.h>
#include <aknlists.h>

#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMFotaSettingsDialog.h"
#include "NSmlDMFotaModel.h"
#include "NSmlDMSyncUi.hrh"
#include <NSmlDMSync.rsg>
#include <featmgr.h>
#include "nsmldmsyncinternalpskeys.h" 

#include <hlplch.h>
#include <csxhelp/dm.hlp.hrh>


#include "NSmlDMSyncDebug.h"


// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaSettingsDialog* CNSmlDMFotaSettingsDialog::NewL()
    {
    CNSmlDMFotaSettingsDialog* self = new ( ELeave ) CNSmlDMFotaSettingsDialog;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CNSmlDMFotaSettingsDialog::~CNSmlDMFotaSettingsDialog()
    {
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    
    if ( iTitlePane->Text() && iOriginalTitle ) 
        {
        TRAP_IGNORE( iTitlePane->SetTextL( *iOriginalTitle ) );
        }
    delete iOriginalTitle;
    iAvkonAppUi->RemoveFromStack( this );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::UpdateFotaSettListboxL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::UpdateFotaSettListboxL()
    {
    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::UpdateFotaSettListboxL()" );

    iFotaSettListBox->ItemDrawer()->ClearAllPropertiesL();
    
    CDesCArray* itemsArray = (CDesCArray*) iFotaSettListBox->Model()->ItemTextArray();
    itemsArray->Reset();
    
    TInt profileId = iDocument->FotaModel()->DefaultFotaProfileIdL();
    HBufC* profileName = HBufC::NewLC( KNSmlMaxProfileNameLength );
    iDocument->FotaModel()->GetProfileNameL( profileId, profileName );
    
    // No default profile found, "Always ask" is shown instead.
    if ( profileName->Des() == KNullDesC )
        {
        // Existing buffer is deleted, since the AllocReadResourceLC reserves
        // memory for the resource.
        CleanupStack::PopAndDestroy( profileName );
        profileName  = iCoeEnv->AllocReadResourceLC( R_QTN_FOTA_PROFILE_ALWAYS_ASK );
        }
    
    
    HBufC* itemText = StringLoader::LoadLC( R_ITEM_FOTA_DEFAULT_SERVER, *profileName );

    itemsArray->AppendL( itemText->Des() );
    
    CleanupStack::PopAndDestroy( 2 ); // itemText, profileName
    iFotaSettListBox->HandleItemAdditionL();

    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::UpdateFotaSettListboxL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::HandleOKL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::HandleOKL()
    {
    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::HandleOKL()" );
	
    // Profile list owned by the dialog
    CArrayFixFlat<TInt>* profileIdList = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
    CleanupStack::PushL( profileIdList );

    CDesCArray* items = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( items );
    items->Reset();

    HBufC* string = StringLoader::LoadLC( R_QTN_FOTA_PROFILE_ALWAYS_ASK );
    items->AppendL( string->Des() );
    CleanupStack::PopAndDestroy( string );
    string = NULL;

    iDocument->FotaModel()->ReadProfileListL( items, profileIdList );
    TInt value = iDocument->FotaModel()->SelectDefaultProfileFromList( profileIdList ) + 1;
    
    CAknRadioButtonSettingPage* dlg = 
        new (ELeave) CAknRadioButtonSettingPage( R_SETTINGS_RADIO,
                                                 value, items );
 
    HBufC* hdr = StringLoader::LoadLC( R_HDR_FOTA_DEFAULT_SERVER );
	CleanupStack::PushL( dlg );
	dlg->SetSettingTextL( hdr->Des() );
    CleanupStack::Pop( dlg );

    TBool ret = dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );

	CleanupStack::PopAndDestroy( hdr );
	hdr = NULL;
    CleanupStack::PopAndDestroy( items ); 

    if ( ret )  // User has accepted a choice
        {
        // Checks that the value conversion is done propery and the result
        // is in range.
        __ASSERT_ALWAYS( ( value >= 0 ) && ( value - 1 < profileIdList->Count() ),
                         TUtil::Panic( KErrArgument ) );
        
        if ( value == 0 )
            {
            iDocument->FotaModel()->SetDefaultFotaProfileIdL( KErrNotFound );
            }
        else
            {
            TInt profileId( ( *profileIdList )[value - 1] );
            iDocument->FotaModel()->SetDefaultFotaProfileIdL( profileId );
            }
        }
    else //For End key press
        {
        STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->ExitCallL();	
        }    
    CleanupStack::PopAndDestroy( profileIdList );
        
    UpdateFotaSettListboxL();

    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::HandleOKL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaSettingsDialog::OkToExitL( TInt aButtonId )
    {
    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::OkToExitL()" );
    
    if ( aButtonId == EAknSoftkeyChange )
        {
    	HandleOKL();
    	return EFalse;
        }
       
    if ( aButtonId == EEikBidCancel || aButtonId == EAknSoftkeyBack )
        {
        TInt value = EFalse;  //IAD: Making DM Idle
        TInt r1 = RProperty::Set(KPSUidNSmlDMSyncApp,KDMIdle,value); 
        FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::OkToExitL() completed" );
        return ETrue;
        }

    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::OkToExitL() completed" );
    return CAknDialog::OkToExitL( aButtonId );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::PreLayoutDynInitL()
    {
    iAvkonAppUi->AddToStackL( this );
    
    iFotaSettListBox = (CAknSettingStyleListBox*) ControlOrNull ( ENSmlFotaSettList );

    if ( !iFotaSettListBox )
        {
        User::Leave( KErrGeneral );
        }
    // Set up/down arrows at bottom of the screen (scrollbar)
    iFotaSettListBox->CreateScrollBarFrameL( ETrue );
    iFotaSettListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn,
                                                                 CEikScrollBarFrame::EAuto );

    iFotaSettListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );   // Deletes items array
    UpdateFotaSettListboxL();

    // Set up the observer (events listener)
    iFotaSettListBox->SetListBoxObserver( this );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::DynInitMenuPaneL( TInt aResourceID,CEikMenuPane* aMenuPane )
	{
     	FLOG( "[OMADM] CNSmlDMDlgProfileView::DynInitMenuPaneL:" );
     
     	if (aResourceID == R_SETTINGS_MENU &&
     	         !FeatureManager::FeatureSupported( KFeatureIdHelp ))    
          {   
              aMenuPane->SetItemDimmed( EAknCmdHelp , ETrue );
          
          }
	}


// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMFotaSettingsDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                              TEventCode aType )
	{
    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::OfferKeyEventL()" );

    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyEscape:
                {
                iAvkonAppUi->ProcessCommandL( EAknCmdExit );
                break;
                }
            case EKeyEnter: // Enter button
            case EKeyOK: // OK button
                {
                HandleOKL();
                return EKeyWasConsumed;
                }
            case EKeyPhoneEnd:
                {
                STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->ExitCallL();
                return EKeyWasConsumed;	
                }
            default:
                {
                break;
                }
            }
        }
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType);
    }
    

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::ProcessCommandL( TInt aCommandId )
    {
    FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::ProcessCommandL()" );

    HideMenu();
    switch ( aCommandId )
        {
        case EAknCmdHelp:    
            { if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
               {               
               	HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi())->AppHelpContextL());
               }
            
            break;
            }
        case ENSmlSettChange:
        case ENSmlMenuCmdFotaSettChange:
            {				
            HandleOKL();
            break;
            }			

        case ENSmlSettExit:
        case EAknCmdExit:
        case EEikCmdExit:
            {
            STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->ExitCallL();
	        STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->CloseGs();
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                 TListBoxEvent aEventType )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::HandleListBoxEventL()" );
	
	if ( aEventType == EEventEnterKeyPressed ||
	     aEventType == EEventItemSingleClicked )
		{
		HandleOKL();
		}
	}


// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	FLOG( "[OMADM] CNSmlDMFotaSettingsDialog::GetHelpContext" );
	
	aContext.iMajor = KUidSmlSyncApp;
    aContext.iContext = KFOTA_HLP_SETTINGS;
    }



// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::CNSmlDMFotaSettingsDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaSettingsDialog::CNSmlDMFotaSettingsDialog()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaSettingsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaSettingsDialog::ConstructL()
    {
    CAknDialog::ConstructL( R_SETTINGS_PROFILEMENU );

    iDocument = (CNSmlDMSyncDocument*) iAvkonAppUi->Document();
    
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( TUid::Uid(EEikStatusPaneUidTitle) ).IsPresent() )
        {
        iNaviPane = ( CAknNavigationControlContainer* )
            statusPane->ControlL( TUid::Uid(EEikStatusPaneUidNavi) );
        iNaviPane->PushDefaultL();
        iTitlePane = static_cast<CAknTitlePane*>
            ( statusPane->ControlL( TUid::Uid(EEikStatusPaneUidTitle) ) );
        if ( iTitlePane->Text() ) 
			{
			iOriginalTitle = HBufC::NewL( 50 ); //KNsmlMaxTitleSize );
			iOriginalTitle->Des().Copy( *iTitlePane->Text() );
            HBufC* titleText = StringLoader::LoadLC( R_QTN_FOTA_TITLE_SETTINGS_DIALOG );
	   		iTitlePane->SetTextL( *titleText );
            CleanupStack::PopAndDestroy( titleText );
            titleText = NULL;
			}
		}
    }


//  End of File
