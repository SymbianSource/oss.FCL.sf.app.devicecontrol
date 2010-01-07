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
* Description:  Methods for Server Profiles Container
*
*/


// INCLUDE FILES
#include <aknPopup.h>		// popup dialogs
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <textresolver.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <featmgr.h>

#include <AknUtils.h>

#include "NSmlDMProfilesContainer.h"
#include "NSmlDMProfilesView.h"
#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncAppEngine.h"
#include "NSmlDMSyncUi.hrh"
#include "NSmlDMdef.h"
#include "NSmlDMSyncDebug.h"
#include <NSmlDMSync.rsg>
#include <nsmldmsync.mbg>
#include <data_caging_path_literals.hrh>


#include <csxhelp/dm.hlp.hrh>


// ========================== MEMBER FUNCTIONS =================================

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMProfilesContainer* CNSmlDMProfilesContainer::NewL(       CAknView* aView,
                                                          const TRect&    aRect )
    {
    CNSmlDMProfilesContainer* self = new( ELeave ) CNSmlDMProfilesContainer( aView );
    
    CleanupStack::PushL( self );
    self->SetMopParent( (MObjectProvider*) aView );
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::CNSmlDMProfilesContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMProfilesContainer::CNSmlDMProfilesContainer( CAknView* aView )
    : iView( aView )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

	iContextMenuUp = EFalse;
	iAppUi = STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi());
	iDocument = STATIC_CAST( CNSmlDMSyncDocument*, iAppUi->Document() );

	iProfilesListBox = new ( ELeave ) CAknDoubleLargeStyleListBox;
	iProfilesListBox->SetContainerWindowL( *this );
	iProfilesListBox->ConstructL( this, EAknListBoxSelectionList );
	
	iProfilesListBox->SetListBoxObserver( this );
	
	iProfilesListBox->CreateScrollBarFrameL( ETrue );
	iProfilesListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
	                                            CEikScrollBarFrame::EOff,
	                                            CEikScrollBarFrame::EAuto );

	iDocument->RefreshProfileListL();
    AddIconsL();

	//Comment this for Moduletests
	ReadProfileListL();

	iProfilesListBox->UpdateScrollBarsL();
	iProfilesListBox->ScrollBarFrame()->MoveVertThumbTo( 0 );		

    SetRect( aRect );
    ActivateL();    
    }

// Destructor
CNSmlDMProfilesContainer::~CNSmlDMProfilesContainer()
    {
    FLOG( "[OMADM] CNSmlDMProfilesContainer::~CNSmlDMProfilesContainer" );
    delete iProfilesListBox;
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::SizeChanged
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::SizeChanged()
    {
    iProfilesListBox->SetRect( Rect() );    // Mandatory, otherwise not drawn
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    
    //Handle change in layout orientation
    if ( aType == KEikDynamicLayoutVariantSwitch || aType == KAknsMessageSkinChange )
        {
        if ( aType == KAknsMessageSkinChange )
        	{
        	// Skin changed; we'll have to reload the icon array
            CArrayPtr<CGulIcon>* iconArray =
                iProfilesListBox->ItemDrawer()->FormattedCellData()->IconArray();
            if ( iconArray )
                {
    	        iconArray->ResetAndDestroy();
	            delete iconArray;
	            iconArray = NULL;
    	        }
    	    TRAP_IGNORE( AddIconsL() );
        	}

        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawDeferred();
		}
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CNSmlDMProfilesContainer::CountComponentControls() const
    {
    TInt retval( 0 );
    if ( iProfilesListBox )
        {
        retval = 1;
        }
    return retval; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CNSmlDMProfilesContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iProfilesListBox;
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::Draw
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::Draw( const TRect& /* aRect */ ) const
    {
    }


// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::HandleControlEventL( CCoeControl* /* aControl */,
                                                TCoeEvent /* aEventType */ )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMProfilesContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                             TEventCode aType )
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::OfferKeyEventL()" );
	FTRACE( FPrint(
    _L("[OMADM]\t CNSmlDMProfilesContainer::OfferKeyEventL>> Begin code = 0x%x"),aKeyEvent.iCode ) );
	
	if ( iProfilesListBox && aType == EEventKey )
		{
		switch ( aKeyEvent.iCode )
			{
			case EStdKeyHome:
				{
				// delete only in main view
                if ( iDocument->ProfileCount() != 0 ) 
                    {
                    iAvkonAppUi->HandleCommandL( ENSmlMenuCmdEraseProfile );
                    }
			    break;
                }
            case EKeyEnter: //Enter key
			case EKeyOK: // OK button
				{
				if ( ! iAppUi->Synchronise() )
					{
					if (iSettingsView != EFalse) 
						{
						return EKeyWasNotConsumed;
						}
					else
						{
						ShowContextMenuL( R_SMLSYNC_CONTXT_MENUBAR_PROFILES_VIEW );
						}
					}
				return EKeyWasConsumed;
				}
			case EKeyEscape:
				{
				if ( (iSettingsView != EFalse) || (  iAppUi->Synchronise() ) )
					{
					return EKeyWasNotConsumed;
					}
				else
					{
					iAvkonAppUi->ProcessCommandL( EEikCmdExit );
					}
		    	break;
				}
			case EKeyUpArrow:
			case EKeyDownArrow:
			    {
			    TKeyResponse retVal =
			        iProfilesListBox->OfferKeyEventL( aKeyEvent, aType );
                iDocument->SetCurrentIndex( iProfilesListBox->CurrentItemIndex() );
                return retVal;
			    }
			case EKeyLeftArrow:
			case EKeyRightArrow:
			    {
			    break;  // AppUi handles the tab changes
			    }
			default:
			    {
			    return iProfilesListBox->OfferKeyEventL( aKeyEvent, aType );
			    }
			}
		}
	FLOG( "[OMADM] CNSmlDMProfilesContainer::OfferKeyEventL() completed, key was not consumed" );
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::PostLayoutDynInitL()
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::PostLayoutDynInitL()" );
	
	iConfirmationNoteUp = EFalse;
	iMenuUp = 0;

	if ( iProfilesListBox->Model()->NumberOfItems() == 0 )
		{
		HBufC* emptyText = 
		                iEikonEnv->AllocReadResourceLC( R_SML_MAIN_NOPROFILES );
		iProfilesListBox->View()->SetListEmptyTextL( emptyText->Des() );
		CleanupStack::PopAndDestroy( emptyText );

		iConfirmationNoteUp = ETrue; // using flag for preventing the popupmenu
									 // from popping up while dialog is up
		HBufC* newProfile = 
		            iEikonEnv->AllocReadResourceLC( R_SML_APPUI_NOTE_TEXT_NEW );

		CAknQueryDialog* startDialog = new (ELeave) CAknQueryDialog();

		if ( startDialog->ExecuteLD( R_STARTQUERY_NOTE, newProfile->Des() ) )
			{	
			iAppUi->ShowEditProfileDialogL( ESmlNewProfile ); 			
			}
		startDialog = NULL;
		CleanupStack::PopAndDestroy( newProfile );
		iConfirmationNoteUp = EFalse;
		}
	else
		{
		iDocument->SetCurrentIndex(0); 
		iProfilesListBox->ScrollBarFrame()->MoveVertThumbTo( 
		                                iProfilesListBox->CurrentItemIndex() );
		}

	FLOG( "[OMADM] CNSmlDMProfilesContainer::PostLayoutDynInitL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::PreLayoutDynInitL()
    {
    FLOG( "[OMADM] CNSmlDMProfilesContainer::PreLayoutDynInitL" );
    
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::HandleListBoxEventL( CEikListBox* aListBox, 
                                                 TListBoxEvent aEventType )
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::HandleListBoxEventL()" );	
	iDocument->SetCurrentIndex( aListBox->CurrentItemIndex() ); 
	if ( aEventType == MEikListBoxObserver::EEventItemSingleClicked )
	    {
	    ShowContextMenuL( R_SMLSYNC_CONTXT_MENUBAR_PROFILES_VIEW );
	    }	
	FLOG( "[OMADM] CNSmlDMProfilesContainer::HandleListBoxEventL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::ShowContextMenuL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::ShowContextMenuL( TInt aResource )
    {
	FLOG( "[OMADM] CNSmlDMProfilesContainer::ShowContextMenuL()" );
	// Switch to Context specific options menu,
	// Show it and switch back to main options menu.
	CEikMenuBar* menuBar = iView->MenuBar();
	menuBar->SetMenuTitleResourceId( aResource );
    menuBar->SetMenuType( CEikMenuBar::EMenuContext );
	// TRAP displaying of menu bar.
	// If it fails, the correct resource is set back before leave.
	TRAPD( err, menuBar->TryDisplayMenuBarL() );
	menuBar->SetMenuTitleResourceId( R_SMLSYNC_MENUBAR_PROFILES_VIEW );
	menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
	User::LeaveIfError( err );
	FLOG( "[OMADM] CNSmlDMProfilesContainer::ShowContextMenuL() completed" );
    }


// -----------------------------------------------------------------------------
//	CNSmlDMProfilesContainer::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::ReadProfileListL()
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::ReadProfileListL()" );
	
	TInt index = 0;
	TInt lockindex = 0;
    iDocument->RefreshProfileListL();
	CArrayFixFlat<TNSmlDMProfileItem> *profileList =
	                                             iDocument->ProfileList( index ); 
	CDesCArray* items = (CDesCArray*) iProfilesListBox->Model()->ItemTextArray();

	items->Reset();
	
	TBuf<KNSmlMaxTextLength256> text;
	TBuf<KNSmlMaxTextLength64>  timeString; 
	TBuf<KNSmlMaxTextLength64>  dateString; 
	TBuf<5> indexBuf;

	HBufC* timeFormatted;
	
	TBool settingEnforcement = TUtil::SettingEnforcementState();
	TBool btObex = FeatureManager::FeatureSupported(KFeatureIdSyncMlDmObex);

	for ( TInt i = 0; i < profileList->Count(); i++ )
		{
		text.Zero();
		TInt index( EProfileInternet ); //internet
		
		if ( ( *profileList )[i].iBearer != EProfileInternet )
			{
			// EProfileObexBlueTooth is defined to 1, but that index has
			// the HTTP "off" icon. Looking at AddIconsL, BlueTooth icons
			// should be 2 and 3, if present.
			//index = EProfileObexBlueTooth;
			index = EProfileObexBlueTooth + 1;
			}
		if ( ( *profileList )[i].iActive == 0 )
            {
			index++;
            }

		if (settingEnforcement ||( *profileList )[i].iProfileLocked ) 
			{
			// Locked setting item icon
			lockindex = 2;
			if (btObex)
				{
				// BlueTooth icons are present
				lockindex = 4;
				}
			}

		indexBuf.Num( index );
		text.Append ( KNSmlTab );
		text.Append ( ( *profileList )[i].iProfileName );
		text.Append ( KNSmlTab );
		if (  ( *profileList )[i].iSynced ) // profile has been synchronised
			{
			timeFormatted = TUtil::SyncTimeLC( ( *profileList )[i].iLastSuccessSync );
			}
		else
			{
			timeFormatted = 
			            iEikonEnv->AllocReadResourceLC( R_SML_MAIN_NOTSYNCED );
			}
		timeString.Append( timeFormatted->Des() );
		
		text.Append ( timeString );
		timeString.Zero();
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );
		text.Insert( 0, indexBuf );
		if ( settingEnforcement || ( *profileList )[i].iProfileLocked)
		     {
		      indexBuf.Num( lockindex );
			  text.Append ( KNSmlTab );
			  text.Insert( text.Length(), indexBuf );
			  }
		items->AppendL ( text );		
		CleanupStack::PopAndDestroy( timeFormatted );
		}

	iProfilesListBox->HandleItemAdditionL();

	if ( iProfilesListBox->Model()->NumberOfItems() == 0 )
		{
		//For Main pane primary text
		HBufC* emptyPrimText = 
		             iEikonEnv->AllocReadResourceLC( R_SML_MAIN_NOPROFILES );
        //For Main pane secondary text		                
        HBufC* emptySecText = 
		        iEikonEnv->AllocReadResourceLC( R_SML_MAIN_NOPROFILES_SEC_TXT );
        HBufC* emptyText = HBufC::NewLC( emptyPrimText->Length() + emptySecText->Length()+ 2 );        
        emptyText->Des().Append( emptyPrimText->Des() );
        emptyText->Des().Append( KSmlEOL );
        emptyText->Des().Append( emptySecText->Des() );
        iProfilesListBox->View()->SetListEmptyTextL( emptyText->Des() );
        CleanupStack::PopAndDestroy( emptyText );
		CleanupStack::PopAndDestroy( emptySecText );
		CleanupStack::PopAndDestroy( emptyPrimText );
		}
	
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMProfilesContainer::ReadProfileListL() completed, profile count = %d"),
        profileList->Count() ) );
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::RefreshL 
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::RefreshL()
	{
	ReadProfileListL();
	//SetCurrentIndex( iProfilesListBox->CurrentItemIndex() );
	SetCurrentIndex( iDocument->CurrentIndex() );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::SetCurrentIndex 
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::SetCurrentIndex( const TInt aIndex )
	{
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMProfilesContainer::SetCurrentIndex() aIndex = %d"),
        aIndex ) );
	
	TInt newIndex( aIndex );
	
	if ( iProfilesListBox->Model()->NumberOfItems() > 0 )
	    {
    	if ( newIndex < 0 )
    	    {
    	    newIndex = 0;
    	    }
      if ( newIndex > ( iProfilesListBox->Model()->NumberOfItems() - 1 ) )
    	    {
    	        newIndex = iProfilesListBox->Model()->NumberOfItems() - 1;
    	        iDocument->SetCurrentIndex(newIndex);
    	    }
    	iProfilesListBox->SetCurrentItemIndexAndDraw( newIndex );
	    }

	FLOG( "[OMADM] CNSmlDMProfilesContainer::SetCurrentIndex() completed" );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::ClosePopupMenu
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::ClosePopupMenuL()
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::ClosePopupMenu" );
	
	if (iContextMenuUp)
		{
	    iAppUi->SetFadedL( EFalse );
		
		CEikButtonGroupContainer* controlPane = 
		                                    CEikButtonGroupContainer::Current();
			controlPane->AddCommandSetToStackL(
					R_NSML_FOTA_PROFILES_VIEW_MSK_OPTIONS_EXIT_CONTEXTOPTIONS );
		controlPane->DrawNow();
		iEikonEnv->AppUiFactory()->ClosePopup( iEikonEnv->EikAppUi() );
		iContextMenuUp = EFalse;
		}
	}


// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
	FLOG( "[OMADM] CNSmlDMProfilesContainer::GetHelpContext" );
	
	aContext.iMajor = KUidSmlSyncApp;
    aContext.iContext = KDM_HLP_SERVERS_VIEW;
	}



// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::FocusChanged
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
	{
	if ( iProfilesListBox )
		{
		iProfilesListBox->SetFocus( IsFocused() );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMProfilesContainer::AddIconsL
// -----------------------------------------------------------------------------
//
void CNSmlDMProfilesContainer::AddIconsL()
    {
    FLOG( "[OMADM] CNSmlDMSyncMainDialog::AddIconsL" );
    
    HBufC* appIconFilePath = HBufC::NewLC( 5 + KDC_APP_BITMAP_DIR().Length() 
                                             + KDMAppIconFileName().Length() );
    TPtr ptr = appIconFilePath->Des();
    ptr.Append( KZDrive );
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KDMAppIconFileName );

	CArrayPtr<CGulIcon>* icons = new (ELeave) CArrayPtrFlat<CGulIcon>(10);
	iProfilesListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );

	icons->AppendL( AknsUtils::CreateGulIconL( 
	                             AknsUtils::SkinInstance(),
	                             KAknsIIDQgnPropSmlHttp,
	                             *appIconFilePath,
	                             EMbmNsmldmsyncQgn_prop_sml_http,
	                             EMbmNsmldmsyncQgn_prop_sml_http_mask ) );
	                             
	icons->AppendL( AknsUtils::CreateGulIconL( 
	                          AknsUtils::SkinInstance(),
	                          KAknsIIDQgnPropSmlHttpOff,
	                          *appIconFilePath,
	                          EMbmNsmldmsyncQgn_prop_sml_http_off,
	                          EMbmNsmldmsyncQgn_prop_sml_http_off_mask ) );

	if ( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmObex  ) )
		{
		icons->AppendL( AknsUtils::CreateGulIconL( 
		                            AknsUtils::SkinInstance(),
		                            KAknsIIDQgnPropSmlBt,
		                            *appIconFilePath,
		                            EMbmNsmldmsyncQgn_prop_sml_bt,
		                            EMbmNsmldmsyncQgn_prop_sml_bt_mask ) );
		                            
		icons->AppendL( AknsUtils::CreateGulIconL(
		                        AknsUtils::SkinInstance(),
		                        KAknsIIDQgnPropSmlBtOff,
		                        *appIconFilePath,
		                        EMbmNsmldmsyncQgn_prop_sml_bt_off,
		                        EMbmNsmldmsyncQgn_prop_sml_bt_off_mask ) );
		}
	
	icons->AppendL( AknsUtils::CreateGulIconL( 
	                          AknsUtils::SkinInstance(),
	                          KAknsIIDQgnIndiSettProtectedAdd,
	                          *appIconFilePath,
	                          EMbmNsmldmsyncQgn_indi_sett_protected_add,
	                          EMbmNsmldmsyncQgn_indi_sett_protected_add_mask ) );

    CleanupStack::PopAndDestroy( appIconFilePath );
    
    FLOG( "[OMADM] CNSmlDMSyncMainDialog::AddIconsL Done" );
    }

void CNSmlDMProfilesContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
      if ( AknLayoutUtils::PenEnabled() )
        { 
        if (iProfilesListBox->Model()->NumberOfItems() == 0 )
           {
           if ( aPointerEvent.iType == TPointerEvent::EButton1Up )	
           		 {
           		  if ( iContextMenuUp )
                    {
                    // Absorb EButton1Up event if we already launched the stylus
                    // popup menu
                    iContextMenuUp = EFalse;
                    }
                  else  
                    {
                    ShowContextMenuL( R_SMLSYNC_CONTXT_MENUBAR_PROFILES_VIEW );	
                    }
           	
           		 }
           else
              CCoeControl::HandlePointerEventL(aPointerEvent);		 
           }
        else    
     	CCoeControl::HandlePointerEventL(aPointerEvent);    
        }
    }
// End of File
