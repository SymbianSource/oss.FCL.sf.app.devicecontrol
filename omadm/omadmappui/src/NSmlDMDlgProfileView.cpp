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
* Description:  Methods for Server Profile Settings dialog
*
*/


// INCLUDE FILES
#include <akntextsettingpage.h>
#include <aknpasswordsettingpage.h>
#include <aknnotewrappers.h>
#include <aknnavi.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <ConnectionUiUtilities.h>
#include <aknradiobuttonsettingpage.h>
#include <cmapplicationsettingsui.h>
#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMDlgProfileView.h"
#include "NSmlDMSyncProfile.h"
#include "NSmlDMSyncAppEngine.h"
#include "NSmlDMSyncDebug.h"
#include "NSmlDMItemData.h"
#include "nsmldmsyncinternalpskeys.h"
#include <nsmldmsync.rsg>
#include <cmmanager.rsg>
#include <connectionuiutilities.rsg>
#include <bautils.h> 

#include <hlplch.h>
#include <csxhelp/dm.hlp.hrh>

_LIT( KACMManagerResDirAndFileName, "z:cmmanager.rsc" );
_LIT(KConnectionUiUtilitiesRscPath, "z:ConnectionUiUtilities.rsc");
_LIT(KDC_RESOURCE_FILES_DIR,"\\resource\\");  


// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::NewL
// -----------------------------------------------------------------------------
//
CNSmlDMDlgProfileView* CNSmlDMDlgProfileView::NewL( CNSmlDMSyncDocument* aDoc,
		                                            TNSmlEditMode aMode,
		                                            TInt aProfileId )
    {
    FLOG( "CNSmlDMDlgProfileView::NewL" );

    CNSmlDMDlgProfileView* self =
                            new ( ELeave ) CNSmlDMDlgProfileView( aDoc,
                                                                  aMode,
                                                                  aProfileId );
    CleanupStack::PushL( self );
    self->ConstructL( R_SETTINGS_PROFILEMENU );
    CleanupStack::Pop();
    return( self );
    }

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::CNSmlDMDlgProfileView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNSmlDMDlgProfileView::CNSmlDMDlgProfileView( CNSmlDMSyncDocument* aDoc,
                                              TNSmlEditMode aMode,
                                              TInt aProfileId  )
                                            : iDoc( aDoc ),
                                              iEditMode( aMode ),
                                              iProfileId( aProfileId ),
                                              iResourceOffset(KErrNotFound),
                                              iConnUtilResourceOffset(KErrNotFound)
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::CNSmlDMDlgProfileView" );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNSmlDMDlgProfileView::~CNSmlDMDlgProfileView()
    {
    FLOG( "[OMADM] ~CNSmlDMDlgProfileView:" );
    if(iResourceOffset >= 0)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }    
    if(iConnUtilResourceOffset >= 0)
        {
        CCoeEnv::Static()->DeleteResourceFile(iConnUtilResourceOffset);
        }
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    
	if ( iProfileList )
		{
		iProfileList->ResetAndDestroy();
		delete iProfileList;
		}
	iAvkonAppUi->RemoveFromStack( this );
	FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::OkToExitL( TInt aButtonId )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::OkToExitL:" );    
	
	
    if ( aButtonId == EAknSoftkeyChange )
        {
    	HandleOKL();
    	return EFalse;
        }
	
	if ( aButtonId == EAknSoftkeyBack )
        {
		TBool closeDlg = ETrue;
		TBool saveProfile = ETrue;
        //TInt index;
		TInt retValue = CheckMandatoryFieldsL();
		if ( retValue != KErrNotFound ) 
			{
            if(CheckUniqueFieldsL()== ENSmlServerId )
                {
			    STATIC_CAST( CNSmlDMSyncAppUi*,
			                     iEikonEnv->EikAppUi())->ShowResourceErrorNoteL(R_QTN_DM_CONF_SET_DUP_SERVER_ID );
                closeDlg = EFalse;
                iProfileListBox->SetCurrentItemIndexAndDraw( retValue );
                saveProfile = EFalse;
			    }			
		  else if(ShowConfirmationNoteL( R_NSML_SETTINGS_SAVE_ANYWAY ))
                {
			    
                if ( iEditMode == ESmlCopyProfile  )
                    {
                    iDoc->MoveIndexToProfile( iProfileId );
                    // This deletes the source profile
                    //iDoc->DeleteProfile( index );
                    iDoc->AppEngine()->DeleteProfileL( iProfileId );
                    iDoc->MoveIndexToProfile( iOldProfileId );
                    }
                else if ( iEditMode == ESmlNewProfile )
                    {
                    iDoc->AppEngine()->DeleteProfileL( iProfileId );
                    }
                //For CP stored umcompleted message
                else if ( iEditMode == ESmlEditProfile )
                    {
                    iDoc->AppEngine()->DeleteProfileL( iProfileId );
                    iDoc->RefreshProfileListL();
                    iDoc->MoveIndexToProfile( iProfileId );
                    }
                //For CP umcompleted message
                saveProfile = EFalse;
                }
            else
                {
                closeDlg = EFalse;
                iProfileListBox->SetCurrentItemIndexAndDraw( retValue );
                saveProfile = EFalse;
                }
              
			}
			
		if ( saveProfile )
			{
			SaveProfileL();	
			}
		if ( closeDlg )
			{
			((CNSmlDMSyncAppUi*)iDoc->AppUi())->ChangeViewL( ETrue );
			}
		return closeDlg;
		}
	return CAknDialog::OkToExitL( aButtonId );
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::ProcessCommandL( TInt aCommandId )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::ProcessCommandL:" );
	
	HideMenu();
	switch ( aCommandId )
		{
     case EAknCmdHelp:             
			{
				if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
             { 
			          HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), ((CNSmlDMSyncAppUi*)iDoc->AppUi())->AppHelpContextL() ); 	
             }
				
			
            break;
			}
		case ENSmlSettChange:
			{				
			CNSmlDMProfileData* item = 
			            GetItemForIndex( iProfileListBox->CurrentItemIndex());
			if ( item->iItemType == CNSmlDMProfileData::ENSmlTypeListYesNo ||
				 item->iItemType == CNSmlDMProfileData::ENSmlTypehttpListYesNo ||
				 item->iItemType == CNSmlDMProfileData::ENSmlTypeList
			    )
				{
				if ( EditSettingItemListL( *item ) )
					{
					//update listbox
					AddProfileListItemL( iProfileListBox, iProfileList );
					}
				iProfileListBox->HandleItemAdditionL();
				}
			else
				{
				HandleOKL();
				}
			}			
		    break;

		case ENSmlSettExit:
		case EAknCmdExit:
        case EEikCmdExit:
			{
			if ( CheckMandatoryFieldsL() >= KErrNone)
				{
				if ( !ShowConfirmationNoteL( R_NSML_SETTINGS_SAVE_ANYWAY ) )
					{
					return;
					}
				else if( iEditMode == ESmlEditProfile )	
					{
					iDoc->AppEngine()->DeleteProfileL( iProfileId );
					}
				else if ( iEditMode == ESmlNewProfile )
                    {
                    iDoc->AppEngine()->DeleteProfileL( iProfileId );
                    }
				}
			TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	        TApaTask task1(taskList.FindApp( KFotaServerAppUid));
	        if(task1.Exists())
	        	{
	        	FLOG( "ending fotaserver 1..." );
	        	task1.EndTask();
	        	}
	        TApaTask task2(taskList.FindApp( KFotaServerAppUid));
	        if(task2.Exists())
	        	{
	        	FLOG( "ending fotaserver 2..." );
	        	task2.EndTask();
	        	}
	        TApaTask task3(taskList.FindApp( KFotaServerAppUid));
	        if(task3.Exists())
	        	{
	        	FLOG( "ending fotaserver 3..." );
	        	task3.EndTask();
	        	}
			STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->DoExit();
			}
		    break;

		default:
	    	break;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::PreLayoutDynInitL()
    {
    FLOG(  "[OMADM] CNSmlDMDlgProfileView::PreLayoutDynInitL:");    
    TParse parse;
    parse.Set(KACMManagerResDirAndFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CEikonEnv* coeEnv = CEikonEnv::Static();
    RFs& fileSession = coeEnv->FsSession();    
    BaflUtils::NearestLanguageFile( fileSession, resourceFile );    
    if ( !BaflUtils::FileExists( fileSession, resourceFile ) )
        {
        User::Leave( KErrNotFound );
        }
     iResourceOffset = coeEnv->AddResourceFileL( resourceFile );
     
     TParse connutilname;
     connutilname.Set(KConnectionUiUtilitiesRscPath, &KDC_RESOURCE_FILES_DIR, NULL);     
     resourceFile.Zero();
     resourceFile.Append(connutilname.FullName());     
     BaflUtils::NearestLanguageFile( fileSession, resourceFile );     
     if ( !BaflUtils::FileExists( fileSession, resourceFile ) )
         {
         User::Leave( KErrNotFound );
         }
     iConnUtilResourceOffset = coeEnv->AddResourceFileL( resourceFile );
     
    iAvkonAppUi->AddToStackL( this );
	FeatureManager::InitializeLibL();

	iProfileListBox = (CAknSettingStyleListBox*)Control( ENSmlSettingList );
	iProfileListBox->SetListBoxObserver( this );
	iProfileListBox->CreateScrollBarFrameL( ETrue );
	iProfileListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
	                                                CEikScrollBarFrame::EOn,
	                                                CEikScrollBarFrame::EAuto );

    iSyncAppEngine = iDoc->AppEngine();
	iProfileList = new (ELeave) CArrayPtrFlat<CNSmlDMProfileData>(1);
	
    if ( iEditMode == ESmlCopyProfile )
		{
		iOldProfileId = iProfileId;
		iDoc->GetCopyProfileL( iProfileId );
		iDoc->RefreshProfileListL();
		iDoc->MoveIndexToProfile( iProfileId );
		iServerIdChanged = ETrue;
		}
	else if ( iEditMode == ESmlNewProfile )
		{
        iProfileId = iSyncAppEngine->CreateProfileL();
		}
    CreateProfileSettingsListL();
    SetSettingTitleL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::DynInitMenuPaneL( TInt aResourceID,
                                              CEikMenuPane* aMenuPane )
	{
     	FLOG( "[OMADM] CNSmlDMDlgProfileView::DynInitMenuPaneL:" );
     	
     	
     	if (aResourceID == R_SETTINGS_MENU  && 
     	                 !FeatureManager::FeatureSupported( KFeatureIdHelp ))
          {   aMenuPane->SetItemDimmed( EAknCmdHelp , ETrue );
          }
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                 TListBoxEvent aEventType )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::HandleListBoxEventL:" );
	
	if ( aEventType == EEventEnterKeyPressed ||
	     aEventType == EEventItemSingleClicked )
		{
		HandleOKL();
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::HandleOKL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::HandleOKL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::HandleOKL:");
	
	CNSmlDMProfileData* item = 
	                    GetItemForIndex( iProfileListBox->CurrentItemIndex() );
	if ( PrepareToEditSettingItemL( *item ) )
		{
		if ( iProfileListBox->CurrentItemIndex() == ENSmlServerId )
			{	
			iServerIdChanged = ETrue;
			if ( CheckUniqueFieldsL() != KErrNotFound )
				{
				STATIC_CAST( CNSmlDMSyncAppUi*,
				         iEikonEnv->EikAppUi())->ShowResourceErrorNoteL(
				                              R_QTN_DM_CONF_SET_DUP_SERVER_ID );
				}
			else
				{
				iServerIdChanged = EFalse;
				}	
			}
		else if ( iProfileListBox->CurrentItemIndex() == ENSmlConnType )
			{
			TBool hidden = 
			    ( Item( ENSmlConnType )->iNumberData != EProfileInternet );
			Item( ENSmlAccessPoint )->iHidden = hidden;			  
			Item( ENSmlProfilePort )->iHidden = hidden;
			Item( ENSmlHttpsUsed )->iHidden = hidden;
			if(Item( ENSmlHttpsUsed )->iNumberData == 0 || hidden )
			    {			    
			    Item( ENSmlHttpsUsername )->iHidden = ETrue;
			    Item( ENSmlHttpsPassword )->iHidden = ETrue;
			    }
			else
			    {			    
			    Item( ENSmlHttpsUsername )->iHidden = EFalse;
			    Item( ENSmlHttpsPassword )->iHidden = EFalse;
			    }			
			}
		//update listbox items
		AddProfileListItemL( iProfileListBox, iProfileList );
		}
	iProfileListBox->HandleItemAdditionL();
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMDlgProfileView::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                    TEventCode aType )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::OfferKeyEventL:" );
	
	if ( aType == EEventKey )
		{
		switch ( aKeyEvent.iCode )
			{
			case EKeyEscape:
                {
                iAvkonAppUi->ProcessCommandL( EAknCmdExit );
                }
			    break;
			case EKeyEnter:
			case EKeyOK: // OK button
                {
				HandleOKL();
                return EKeyWasConsumed;
                }
			default:
			    break;
			}
		}
	return CEikDialog::OfferKeyEventL( aKeyEvent, aType);
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::AddProfileListItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::AddProfileListItemL( 
                                             CEikTextListBox* aListBox,
                                             CNSmlDMSettingItemList* aItemList )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::AddProfileListItemL:" );
	
	if ( ( !aListBox ) || ( !aItemList ) )
		{
		User::Leave( KErrNotFound );
		}
	CDesCArray* arr = (CDesCArray*)aListBox->Model()->ItemTextArray();
	arr->Reset();

	for ( TInt index = 0; index < aItemList->Count(); index++ )
		{
		if ( !(aItemList->At( index ))->iHidden )
			{
			TBool convert = 
			           !( index == ENSmlProfileURL || index == ENSmlUsername /*|| index == ENSmlProfilePort*/ );
			
			arr->AppendL( (aItemList->At(index))->ItemToListLC( convert )->Des() );
			(aItemList->At( index ))->iIndex = arr->Count() - 1;
			
			CleanupStack::PopAndDestroy(); //ItemToListLC()
			}
		else
			{
			(aItemList->At( index ))->iIndex = KErrNotFound;
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::CreateProfileSettingsListL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::CreateProfileSettingsListL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::CreateProfileSettingsListL:" );
	
	CDesCArrayFlat* items = 
	                iEikonEnv->ReadDesCArrayResourceL( R_SML_SETTINGS_ITEMS );
	CleanupStack::PushL( items );
	TInt index;
	for ( index = 0; index < items->Count(); index++ )
		{
		iProfileList->AppendL( CNSmlDMProfileData::NewLC() );		
		CleanupStack::Pop();
		}
	BuildHeaders( items );
	BuildItemIds();
	// Open profile	
	iProfile = iSyncAppEngine->OpenProfileL( iProfileId, ESmlOpenReadWrite );

	BuildProfileNameItemL();
	BuildProfileServerIdItemL();
	BuildProfileServerPasswordItemL();
	BuildProfileConnectionTypeItemL();
	BuildProfileAccessPointItemL();
    BuildProfileUrlItemL();
    BuildProfilePortItemL();
    BuildProfileUserNameandPassWordItemL();
    BuildProfileActiveAcceptedItemL();
    BuildProfileHttpsUsedItemL();
    BuildProfileHttpsUserNameAndPasswordItemL();
    
	// Close profile
	iSyncAppEngine->CloseProfile();
	
	AddProfileListItemL( iProfileListBox, iProfileList );
	CleanupStack::PopAndDestroy( items );
	}


// -----------------------------------------------------------------------------
// void CNSmlDMDlgProfileView::SaveProfileL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::SaveProfileL()
	{
	FLOG( "[OMADM] void CNSmlDMDlgProfileView::SaveProfileL:" );
	
    TRAPD( err, iProfile = iSyncAppEngine->OpenProfileL( iProfileId, ESmlOpenReadWrite ) );
    if( err != KErrNone )
    	{
    	FLOG( "[OMADM] err != KErrNone--problem in opening the profile" );
    	// Profile could not be opened for saving
    	iDoc->RefreshProfileListL();
    	iDoc->MoveIndexToProfile( iProfileId );
    	return;
    	}

	// Server name
	iProfile->SetNameL( Item(ENSmlProfileName)->Value()->Des() );
	// Server Id
	iProfile->SetServerIdL( Item(ENSmlServerId)->Value()->Des() );
	// Server password
	iProfile->SetServerPasswordL( Item(ENSmlProfileDMServerPassword)->iValue );
	// connection type
    iProfile->SetBearerTypeL( Item(ENSmlConnType)->iNumberData );
    // Accesspoint
    if ( Item(ENSmlAccessPoint)->iWapAPInUse )
        {
        // User has not edited accesspoint fiels so convert it back to iap
        SetAccessPointInfoL( Item(ENSmlAccessPoint)->iNumberData );
        }
    iProfile->SetAccessPointL( Item(ENSmlAccessPoint)->iNumberData );
    // Host address
    TInt num = Item( ENSmlProfilePort )->iNumberData;
	iProfile->SetHostAddressL( Item(ENSmlProfileURL)->Value()->Des(), num );
	// User name
    iProfile->SetUserNameL( Item(ENSmlUsername)->Value()->Des() );
    // User password
    iProfile->SetPasswordL( Item(ENSmlPassword)->iValue );
    // Active profile
	if (Item(ENSmlServerAlertAllowed)->iNumberData)
		{
		iProfile->SetSASyncStateL( ESASyncStateEnable );
		}
	else
		{
		iProfile->SetSASyncStateL( ESASyncStateDisable );
		}
    // Server alert allowed
    if ( Item(ENSmlServerAlertAllowed)->iNumberData )
        {
        if ( !Item(ENSmlServerAccept)->iNumberData )
			{
			iProfile->SetSASyncStateL( ESASyncStateConfirm );
			}
		}
    // Http authentication	
    iProfile->SetHttpUsedL( Item(ENSmlHttpsUsed)->iNumberData );
    // http username
    iProfile->SetHttpUserNameL( Item(ENSmlHttpsUsername)->Value()->Des() );
    // http password
    iProfile->SetHttpPasswordL( Item(ENSmlHttpsPassword)->iValue );
    
	iProfile->SetCreatorId( KUidSmlSyncApp.iUid );
	iProfileId = iProfile->ProfileId();
    iProfile->SaveL();
    iSyncAppEngine->CloseProfile();
    
	iDoc->RefreshProfileListL();	
	iDoc->MoveIndexToProfile( iProfileId );
	}

//-----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::Item
//-----------------------------------------------------------------------------
//
CNSmlDMProfileData* CNSmlDMDlgProfileView::Item( TInt aItemId )
	{
	return iProfileList->At( aItemId );		
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::CheckMandatoryFieldsL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDlgProfileView::CheckMandatoryFieldsL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::CheckMandatoryFieldsL:" );
	
	TInt returnValue = KErrNotFound;

	HBufC* emptyText =
	    iEikonEnv->AllocReadResourceLC( R_QTN_SML_SET_VAL_FIELD_NONE );
	HBufC* emptyAddr = 
	    iEikonEnv->AllocReadResourceLC( R_QTN_SML_SET_VAL_HOSTADDR_FIELD_NONE );
	
	for ( TInt index = 0; index < iProfileList->Count(); index++ )
		{
		if ( Item(index)->iMandatory && 
		   ( ( index == ENSmlServerId &&
		           Item(index)->Value()->Des() == emptyText->Des() ) ||
		     ( index == ENSmlProfileURL &&
		           Item(index)->Value()->Des() == emptyAddr->Des() ) || 
		     ( index == ENSmlUsername &&
		           Item(index)->Value()->Des() == emptyText->Des() ) ||      
		     ( Item(index)->Value()->Des().Length() == 0 ) ) )
		    {
			returnValue = Item(index)->iIndex;
			break;
			}
		}
	if ( returnValue == KErrNotFound )
		{
		returnValue = CheckUniqueFieldsL();
		}
	CleanupStack::PopAndDestroy( emptyAddr );
	CleanupStack::PopAndDestroy( emptyText );
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::CheckUniqueFieldsL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDlgProfileView::CheckUniqueFieldsL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::CheckUniqueFieldsL:" );
	
    TInt returnValue( KErrNotFound );
	CNSmlDMProfileData* item = Item(ENSmlServerId);

	if ( iServerIdChanged )
		{
		if ( iEditMode != ESmlEditProfile )
			{
			if ( iSyncAppEngine->ServerIdFoundL( item->Value()->Des(),
			                                     iProfileId ) )
				{			    
				returnValue = ENSmlServerId;
				}
			}
		else if ( ( iEditMode == ESmlEditProfile ) &&
		          ( item->Value()->Des() != item->iValue ) )
			{
			if ( iSyncAppEngine->ServerIdFoundL( item->Value()->Des(),
			                                     iProfileId ) )
				{		
				returnValue = ENSmlServerId;
				}
			}
		}
        
	return returnValue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::PrepareToEditSettingItemL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::PrepareToEditSettingItemL( 
                                                    CNSmlDMProfileData& aItem )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::PrepareToEditSettingItemL:" );
	
	TInt prompt = WarnUserOfChangeL();
	if ( prompt )	//show confirmation if needed
		{
		if ( !ShowConfirmationNoteL( prompt ) )
			{
			return EFalse;
			}
		}
	switch ( aItem.iItemType )
		{
		case CNSmlDMProfileData::ENSmlTypeIAP:
		    {
			return EditSettingItemAccessPointL( aItem );
		    }
			
		case CNSmlDMProfileData::ENSmlTypeText:
		    {
			return EditSettingItemTextL( aItem );
		    }

		case CNSmlDMProfileData::ENSmlTypeSecret:
			{
			return EditSettingItemSecretL( aItem );
			}

		case CNSmlDMProfileData::ENSmlTypeNumber:
		    {
			return EditSettingItemNumberL( aItem );
		    }

		case CNSmlDMProfileData::ENSmlTypeList:
		    {
		    return EditConnectionSettingItemL( aItem );
		    }
		
		case CNSmlDMProfileData::ENSmlTypeListYesNo:
		    {
		    return EditSettingItemListYesNoL( aItem );
		    }

		case CNSmlDMProfileData::ENSmlTypehttpListYesNo:
		    {
		    return EditHttpSettingItemListYesNoL( aItem );
		    }
		    
		case CNSmlDMProfileData::ENSmlTypeTime:
		    {
			return EFalse;
		    }
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemTextL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemTextL( CNSmlDMProfileData& aItem )
	{
    FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemTextL:");
    
    TBool retVal = EFalse;
    TInt flags( CAknTextSettingPage::EZeroLengthAllowed );
	if ( aItem.iMandatory )
		{
		flags = CAknTextSettingPage::EZeroLengthNotOffered;
		}

	HBufC* tmpBuf = HBufC::NewLC( aItem.iLength ); 
	TPtr ptr( tmpBuf->Des() );
	ptr.Zero();
	//Language varaint error fix  EPZO-6Y88F9
	HBufC* emptyText =
	    iEikonEnv->AllocReadResourceLC( R_QTN_SML_SET_VAL_FIELD_NONE );
	if ( aItem.Value()->Des() != emptyText->Des() )
	{
	ptr.Append( aItem.Value()->Des() );
	}  
	//Language varaint error fix  EPZO-6Y88F9
	CAknTextSettingPage* dlg = 
	        new (ELeave) CAknTextSettingPage( R_NSML_SETTINGS_TEXT, ptr, flags );
	CleanupStack::PushL( dlg );
	dlg->SetSettingTextL( aItem.Header()->Des() ); 
	dlg->ConstructL();
	dlg->TextControl()->SetTextLimit( aItem.iLength );

	if ( aItem.iLatinInput )
		{		
		dlg->TextControl()->SetAvkonWrap( ETrue );
		dlg->TextControl()->SetAknEditorCase( EAknEditorLowerCase );
		dlg->TextControl()->SetAknEditorFlags( 
		                                EAknEditorFlagNoT9 |
		                                EAknEditorFlagLatinInputModesOnly );
		}
	CleanupStack::Pop( dlg );
	
	if ( ( dlg->ExecuteLD ( CAknSettingPage::EUpdateWhenChanged ) ) &&
	     ( aItem.Value()->Des() != ptr ) )
		{//Fix for RKCC-7BC8Z2 
		if(aItem.iItemId== ENSmlProfileURL)    
		{  TBuf<255> tempBuf;
		   tempBuf.Zero();
		  _LIT(Kfslash ,"/");
		   for(TInt i= 0;i<ptr.Length();i++)
		    {   TChar ch = ptr[i];
		        TText* t = (TText*)ptr[i]; 
		        if( ptr[i] == '\\') 
		        {
		          tempBuf.Append(Kfslash);	
		        }
			    else
			    {
			      tempBuf.Append(ch);	
			    } 
	    	}
	    	if(tempBuf.Length())
	    	{
	    		ptr.Copy(tempBuf);
	    	}
		 }
		aItem.SetValue(ptr);
		retVal = ETrue;
		}
	CleanupStack::PopAndDestroy( emptyText ); 
	CleanupStack::PopAndDestroy( tmpBuf );
	return retVal;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemSecretL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemSecretL( CNSmlDMProfileData& aItem )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemSecretL:" );
	
	TBuf<KNSmlMaxTextLength128> buf;
	CAknAlphaPasswordSettingPage* pwd =
	         new (ELeave) CAknAlphaPasswordSettingPage( R_SETTING_PASSWORD,
		                                                aItem.iValue,
		                                                aItem.iValue  );
  	CleanupStack::PushL( pwd );

	pwd->SetSettingTextL( aItem.Header()->Des() ); 
	pwd->ConstructL();
	pwd->AlphaPasswordEditor()->SetMaxLength( aItem.iLength );
	//pwd->AlphaPasswordEditor()->RevealSecretText( ETrue );

	CleanupStack::Pop( pwd );
	if ( pwd->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
		{
		buf.Fill('*', 4 );
		aItem.SetValue( buf ); //the real value backed up
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemNumberL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemNumberL( CNSmlDMProfileData& aItem )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemNumberL:" );
	
	CAknIntegerSettingPage * dlg = 
	    new (ELeave) CAknIntegerSettingPage( 
	                        R_SETTINGS_NUMBER,
	                        aItem.iNumberData,
	                        CAknIntegerSettingPage::EInvalidValueNotOffered) ;
	CleanupStack::PushL( dlg );
	dlg->SetSettingTextL( aItem.Header()->Des() );
	dlg->ConstructL();
	dlg->IntegerEditorControl()->SetAknEditorNumericKeymap(
											EAknEditorPlainNumberModeKeymap );
    if ( aItem.iLatinInput )
	 {
		
    dlg->IntegerEditorControl()->SetAknEditorFlags( EAknEditorFlagLatinInputModesOnly );
    
    dlg->IntegerEditorControl()->SetValueL( aItem.iNumberData );
    
	 }											
	CleanupStack::Pop( dlg );

	if ( dlg->ExecuteLD( CAknIntegerSettingPage::EUpdateWhenChanged ) )
		{
		   aItem.SetValue( aItem.iNumberData );
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemListL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemListL( CNSmlDMProfileData& aItem )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemListL:" );
	
	TInt curSelection(aItem.iNumberData );
	CDesCArray* stringData = iEikonEnv->ReadDesCArrayResourceL(aItem.iResource);
	CleanupStack::PushL( stringData );
	
	CAknRadioButtonSettingPage* dlg = 
	new (ELeave) CAknRadioButtonSettingPage( R_SETTINGS_RADIO,
	                                         curSelection,
	                                         stringData );
	CleanupStack::PushL( dlg );
	dlg->SetSettingTextL( aItem.Header()->Des() ); 
    CleanupStack::Pop( dlg );
	if ( dlg->ExecuteLD() )
		{
		aItem.iNumberData = curSelection;
		if ( aItem.iItemType == CNSmlDMProfileData::ENSmlTypehttpListYesNo )
			{
			if ( aItem.iNumberData == 1 )
				{
				Item( ENSmlHttpsUsername )->iHidden =  EFalse;
				Item( ENSmlHttpsPassword )->iHidden =  EFalse;
				}
			else
				{
				Item( ENSmlHttpsUsername )->iHidden =  ETrue;
				Item( ENSmlHttpsPassword )->iHidden =  ETrue;
				}
			}
		aItem.SetValue( (*stringData)[curSelection] );
		CleanupStack::PopAndDestroy(stringData);
		return ETrue;
		}
	CleanupStack::PopAndDestroy(stringData);
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemAccessPointL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemAccessPointL( 
                                                    CNSmlDMProfileData& aItem )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemAccessPointL:" );
	
	TInt curSelection(ENsmlDefaultConnection);
   if(aItem.iNumberData == KErrGeneral || aItem.iNumberData == KErrNotFound ) //default conenction
        {
        curSelection = ENsmlDefaultConnection;
        }
    else
        {        
        curSelection = ENsmlIap;
        }

    HBufC* destName = StringLoader::LoadLC( 
									R_CMMANAGERUI_SETT_DEFAULT_CONNECTION);
    HBufC* userdef = StringLoader::LoadLC(
									R_CMMANAGERUI_SETT_USER_DEFINED);
    HBufC* title = StringLoader::LoadLC(
									R_CMMANAGERUI_SETT_DESTINATION);
    
    CDesCArrayFlat* stringData = new (ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( stringData );
    stringData->AppendL(destName->Des());
    stringData->AppendL(userdef->Des());

    CAknRadioButtonSettingPage* dlg = 
    new (ELeave) CAknRadioButtonSettingPage( R_SETTINGS_RADIO,
            curSelection,
            stringData );
    CleanupStack::PushL( dlg );
    dlg->SetSettingTextL( title->Des() ); 
    CleanupStack::Pop( dlg );

    TInt ret = dlg->ExecuteLD(); 

	TUint32 currentAccesspoint(0);
	if ( !ret )
		{		
		CleanupStack::PopAndDestroy(4);
		return EFalse; // user canceled "Always ask" dialog
		}
	if(curSelection == ENsmlDefaultConnection || curSelection == ENsmlAlwaysAsk )
        {        
        aItem.SetValue( destName->Des() );
        aItem.iNumberData = KErrGeneral;
        CleanupStack::PopAndDestroy(4);        
        return ETrue;  // user selected "default connection" option 
        }
	else
	    {	   
	    currentAccesspoint =  aItem.iNumberData ;	    
	    FTRACE( RDebug::Print( 
	     _L("[OMADM] CNSmlDMDlgProfileView::EditSettingItemAccessPointL: current ap (%d)"),
	                                                currentAccesspoint ) );
	    }
	 TCmSettingSelection userSelection;
	 userSelection.iId = currentAccesspoint;
	 userSelection.iResult = CMManager::EConnectionMethod;
	        CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
	        CleanupStack::PushL(settings);
	        TUint listedItems = CMManager::EShowConnectionMethods; 
	        TBearerFilterArray filter;
	        TBool selected = settings->RunApplicationSettingsL(userSelection,
	                                                           listedItems,
	                                                           filter);
	        CleanupStack::PopAndDestroy(settings);
	        if (selected) {
	                   switch (userSelection.iResult)
	                       {	                      
	                       case CMManager::EConnectionMethod:
	                           {
	                           
	                           SetAccessPointInfoL( userSelection.iId );
	                           break;
	                           }
							default:
								break;
	                          }	                   	                   
	                   }
	        else
	            {
	        // Check end key handling etc?
	        //Keep previous selection
	            }
	CleanupStack::PopAndDestroy(4);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::SetAccessPointInfoL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::SetAccessPointInfoL( TUint aId )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::SetAccessPointInfoL:");
	TBool retValue( EFalse );
	RCmManager cmManager;    
	        cmManager.OpenLC();
	        RCmConnectionMethod conn;
	        TRAPD(err, conn = cmManager.ConnectionMethodL( aId ));
	        if(err == KErrNone)//connection method exists
	            retValue = ETrue;
	       
	if (retValue )
		{
	    CleanupClosePushL( conn ); 
		HBufC* name = conn.GetStringAttributeL( CMManager::ECmName );
		Item( ENSmlAccessPoint )->SetValue( *name );
		delete name;
		CleanupStack::PopAndDestroy( 1 ); // conn
		Item( ENSmlAccessPoint )->iNumberData = aId;
		Item( ENSmlAccessPoint )->iWapAPInUse = EFalse; 
		}
	else//connection method doesn't exist
		{		
		HBufC* emptyText = 
		    iEikonEnv->AllocReadResourceLC(R_CMMANAGERUI_SETT_DEFAULT_CONNECTION);
        Item( ENSmlAccessPoint )->SetValue( emptyText->Des() );
		Item( ENSmlAccessPoint )->iNumberData = KErrNotFound;
		Item( ENSmlAccessPoint )->iWapAPInUse = EFalse;
		CleanupStack::PopAndDestroy(emptyText);
		}
	CleanupStack::PopAndDestroy( 1 ); //cmManager	
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::GetAccessPointInfoL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::GetAccessPointInfoL( TInt aId )
    {
    FTRACE( RDebug::Print( 
            _L("[OMADM] CNSmlDMDlgProfileView::GetAccessPointInfoL: aId (%d)"), aId));

    TInt err = KErrNotFound;
    TBool apExists = EFalse;
    RCmManager cmManager;    
    cmManager.OpenLC();
    RCmConnectionMethod conn;
    TRAP(err, conn = cmManager.ConnectionMethodL( aId ));
    if(err == KErrNone)//connection method exists
        {
    CleanupClosePushL( conn );
    apExists = ETrue;
        }
    if(!apExists || aId == KErrGeneral || aId == KErrNotFound ) 
        {
    HBufC* emptyText = 
            iEikonEnv->AllocReadResourceLC(R_CMMANAGERUI_SETT_DEFAULT_CONNECTION);
    Item( ENSmlAccessPoint )->SetValue( emptyText->Des() );
    Item( ENSmlAccessPoint )->iNumberData = KErrGeneral;
    Item( ENSmlAccessPoint )->iWapAPInUse = EFalse;
    CleanupStack::PopAndDestroy( emptyText );
    if(err == KErrNone)//connection method exists
        {
    CleanupStack::PopAndDestroy( 2 );//conn, cmManager	 
        }
    else
        CleanupStack::PopAndDestroy( 1 );//cmManager
    return;
        }
    else
        {   
        HBufC* name = conn.GetStringAttributeL( CMManager::ECmName );
        Item( ENSmlAccessPoint )->SetValue( *name );
        delete name;
        Item( ENSmlAccessPoint )->iNumberData = aId;
        Item( ENSmlAccessPoint )->iWapAPInUse = ETrue;        
		}
    if(err == KErrNone)//connection method exists
        {
        CleanupStack::PopAndDestroy( 2 );//conn, cmManager
        }
    else
        CleanupStack::PopAndDestroy( 1 );

    }

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditSettingItemListYesNoL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditSettingItemListYesNoL( 
                                                    CNSmlDMProfileData& aItem )
	{
    FLOG( "[OMADM] CNSmlDMDlgProfileView::EditSettingItemListYesNoL:");

	CDesCArray* stringData = 
	                      iEikonEnv->ReadDesCArrayResourceL( aItem.iResource );
	CleanupStack::PushL( stringData );
	
	if ( aItem.iNumberData == 0 )
		{
		aItem.iNumberData = 1;
		}
	else
		{
		aItem.iNumberData = 0;
		}
	
	aItem.SetValue( (*stringData)[aItem.iNumberData] );
	CleanupStack::PopAndDestroy( stringData );
	return ETrue;
	}


// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditHttpSettingItemListYesNoL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditHttpSettingItemListYesNoL( 
                                                    CNSmlDMProfileData& aItem )
	{
    FLOG( "[OMADM] CNSmlDMDlgProfileView::EditHttpSettingItemListYesNoL:" );

	CDesCArray* stringData = 
	                      iEikonEnv->ReadDesCArrayResourceL( aItem.iResource );
	CleanupStack::PushL( stringData );
	
	if ( aItem.iNumberData == 0 )
		{
		aItem.iNumberData = 1;
		Item( ENSmlHttpsUsername )->iHidden =  EFalse;
		Item( ENSmlHttpsPassword )->iHidden =  EFalse;
		}
	else
		{
		aItem.iNumberData = 0;
		Item( ENSmlHttpsUsername )->iHidden =  ETrue;
		Item( ENSmlHttpsPassword )->iHidden =  ETrue;
		}
	
	aItem.SetValue( (*stringData)[aItem.iNumberData] );
	CleanupStack::PopAndDestroy( stringData );
	return ETrue;
	}
	
// ---------------------------------------------------------------------------------------------------
// CNSmlDMDlgProfileView::EditConnectionSettingItemL
// This method gets called when stylus single clck or MSK pressed on session mode item
// ---------------------------------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::EditConnectionSettingItemL( 
                                                    CNSmlDMProfileData& aItem )
    {
    FLOG( "[OMADM] CNSmlDMDlgProfileView::EditConnectionSettingItemL:" );

    CDesCArray* stringData = 
                          iEikonEnv->ReadDesCArrayResourceL( aItem.iResource );
    CleanupStack::PushL( stringData );
    TBool hidden = EFalse;
    //Value of the setting item gets changed between Internet & Bluetooth for single click or msk press
	//If any new session mode like USB(third item) is going to add, radio button setting page should pop up
    if ( aItem.iNumberData == EProfileInternet )
        {
        hidden = ETrue;
        aItem.iNumberData = EProfileObexBlueTooth;
        Item( ENSmlAccessPoint )->iHidden = ETrue;           
        Item( ENSmlProfilePort )->iHidden = ETrue;
        Item( ENSmlHttpsUsed )->iHidden = ETrue;        
        }
    else
        {
        hidden = EFalse;
        aItem.iNumberData = EProfileInternet;
        Item( ENSmlAccessPoint )->iHidden = EFalse;           
        Item( ENSmlProfilePort )->iHidden = EFalse;
        Item( ENSmlHttpsUsed )->iHidden = EFalse;        
        }
    
    if(Item( ENSmlHttpsUsed )->iNumberData == 0 || hidden )
        {               
        Item( ENSmlHttpsUsername )->iHidden = ETrue;
        Item( ENSmlHttpsPassword )->iHidden = ETrue;
        }
    else
        {               
        Item( ENSmlHttpsUsername )->iHidden = EFalse;
        Item( ENSmlHttpsPassword )->iHidden = EFalse;
        } 
    
    aItem.SetValue( (*stringData)[aItem.iNumberData] );
    CleanupStack::PopAndDestroy( stringData );
    return ETrue;
    }
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::SetSettingTitleL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::SetSettingTitleL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::SetSettingTitleL:" );
	
	TBuf<KNSmlMaxTextLength128> title;
	StringLoader::Load( title, R_SML_SETTINGS_TITLE );
	CAknTitlePane* titlePane = 
	            (CAknTitlePane*) iAvkonAppUi->StatusPane()->ControlL( 
	                                      TUid::Uid( EEikStatusPaneUidTitle ) );
	titlePane->SetTextL( title );
    iNaviPane = ( CAknNavigationControlContainer* )
        iAvkonAppUi->StatusPane()->ControlL( TUid::Uid(EEikStatusPaneUidNavi) );
    iNaviPane->PushDefaultL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDlgProfileView::ShowConfirmationNoteL(TInt aResource)
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::ShowConfirmationNoteL:" );
	
	HBufC* label = iEikonEnv->AllocReadResourceLC( aResource ); 
	CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog;
	TBool retVal = queryDialog->ExecuteLD( R_STARTQUERY_NOTE, label->Des() );
	CleanupStack::PopAndDestroy( label );
	return retVal;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::WarnUserOfChangeL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDlgProfileView::WarnUserOfChangeL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::WarnUserOfChangeL:" );
	
	TInt prompt = KErrNone;
    if ( iProfileListBox->CurrentItemIndex() == ENSmlServerId )
        {
    	HBufC* emptyText = 
    	        iEikonEnv->AllocReadResourceLC( R_QTN_SML_SET_VAL_FIELD_NONE );
    	if ( Item(ENSmlServerId)->Value()->Des() != emptyText->Des() )
    		{
    		prompt = R_QTN_DM_CONF_SET_SERVER_ID;
    		}
    	CleanupStack::PopAndDestroy( emptyText );
        }
	return prompt; 	
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::DoExitFromMenuL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::DoExitFromMenuL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::DoExitFromMenuL:" );
	
	TInt retValue = CheckMandatoryFieldsL();
	if ( retValue >= KErrNone )
		{
		if ( !ShowConfirmationNoteL( R_NSML_SETTINGS_SAVE_ANYWAY ) )
			{
			return;
			}
		}
	((CNSmlDMSyncAppUi*)iDoc->AppUi())->ChangeViewL();
	((CNSmlDMSyncAppUi*)iDoc->AppUi())->DoExit();		
	}



// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::GetHelpContext(TCoeHelpContext& aContext) const
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::GetHelpContext:");
	
	aContext.iMajor = KUidSmlSyncApp;
    aContext.iContext = KDM_HLP_SETTINGS;
	}
	


// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildHeaders
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildHeaders( CDesCArray* aHeaders )
	{	
	for ( TInt index = 0; index < iProfileList->Count(); index++ )
		{
		iProfileList->At(index)->SetHeader( (*aHeaders)[index] );
		}
	}
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildItemIds
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildItemIds( )
	{	
	for ( TInt index = 0; index < iProfileList->Count(); index++ )
		{
		iProfileList->At( index )->iItemId = index;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileNameItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileNameItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileNameItemL" );
	
    CNSmlDMProfileData* item = Item(ENSmlProfileName);
	if ( EditMode() || CopyMode() )
		{
        TBuf<KNSmlMaxItemLength> buf;
        iProfile->GetName( buf );
		item->SetValue( buf );
		}
	//set default profile name if new or namelength is zero
	if ( ( iEditMode == ESmlNewProfile ) ||
	     ( item->Value()->Des().Length() == 0) )
		{
		item->SetValue( iDoc->GetNextProfileNameLC()->Des() );
		CleanupStack::PopAndDestroy();
		}
	item->iMandatory = ETrue;
	item->iLength = KNSmlMaxProfileNameLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeText;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileServerIdItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileServerIdItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileServerIdItemL" );
    CNSmlDMProfileData* item = Item(ENSmlServerId);
	if ( EditMode() )
		{
        TBuf<KNSmlMaxItemLength> buf;
        iProfile->GetServerId( buf );
        if( buf.Length() > 0 )       
           {
		item->SetValue( buf );
           } 
		}
	else
		{
		HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
		                                         R_QTN_SML_SET_VAL_FIELD_NONE );
		item->SetValue( emptyText->Des() );
		CleanupStack::PopAndDestroy( emptyText );
		
		}
	item->iMandatory = ETrue;
	item->iLength = KNSmlDmServerIdMaxLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeText;
	item->iLatinInput = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileServerPasswordItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileServerPasswordItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileServerPasswordItemL" );
	
    TBuf<KNSmlMaxTextLength256> buf;
    CNSmlDMProfileData* item = Item(ENSmlProfileDMServerPassword);
	if ( EditMode() || CopyMode() )
		{
        TBuf<KNSmlMaxItemLength> buf;
        iProfile->GetServerPassword( buf );
        item->iValue.Copy( buf );
		}
	else
		{
		item->iValue = KNullDesC;
		}

	buf.Fill('*', 4 );
	item->SetValue(buf);
	item->iLength = KNSmlMaxPasswordLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeSecret;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileConnectionTypeItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileConnectionTypeItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileConnectionTypeItemL" );
	
	CNSmlDMProfileData* item = Item(ENSmlConnType);
	if ( EditMode() || CopyMode() )
		{
        item->iNumberData = iProfile->BearerType();
		if ( ( item->iNumberData != EProfileObexBlueTooth ) && 
		     ( item->iNumberData != EProfileInternet ) )
			{
			//default to internet
			item->iNumberData = EProfileInternet;
			}
		}
	else
		{
		item->iNumberData = EProfileInternet;
		}
	CDesCArray* stringData = 
	                    iEikonEnv->ReadDesCArrayResourceL(R_SML_BEARER_TYPE);
	CleanupStack::PushL( stringData );
	
	if ( !FeatureManager::FeatureSupported( KFeatureIdSyncMlDmObex  ) )
		{
		item->iHidden = ETrue;
		item->iNumberData = EProfileInternet;
		}
	
	item->SetValue ( (*stringData)[item->iNumberData] );
	CleanupStack::PopAndDestroy( stringData );
	item->iResource = R_SML_BEARER_TYPE;
	item->iLength   = 80;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeList;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileAccessPointItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileAccessPointItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileAccessPointItemL" );
	
	CNSmlDMProfileData* item = Item(ENSmlAccessPoint);
	item->iWapAPInUse = EFalse;
	if ( EditMode() || CopyMode() )
		{
        item->iNumberData = iProfile->AccessPointL();
		}
	else
		{
		item->iNumberData = KErrGeneral;
		}
    GetAccessPointInfoL( item->iNumberData );
	item->iLength = 0;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeIAP;
	item->iHidden = ( Item( ENSmlConnType )->iNumberData != EProfileInternet );	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileUrlItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileUrlItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileUrlItemL" );
	CNSmlDMProfileData* item = Item(ENSmlProfileURL);
	if ( EditMode() || CopyMode() )
		{
		iProfile->GetHostAddress( iBuf, item->iNumberData );
		if (iBuf.Length() > KNSmlMaxURLLength )
		  {
		    iBuf.SetLength(KNSmlMaxURLLength);
		  }
		if( iBuf.Length() > 0 )        
           {
		item->SetValue( iBuf );
           }   
		}
	else
		{
		HBufC* emptyAddr = iEikonEnv->AllocReadResourceLC( 
		                    R_QTN_SML_SET_VAL_HOSTADDR_FIELD_NONE );
	    item->SetValue( emptyAddr->Des() );
	    CleanupStack::PopAndDestroy(); //emptyAddr
		}
    item->iLength = KNSmlMaxURLLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeText;
	item->iLatinInput = ETrue;
	item->iMandatory = ETrue;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfilePortItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfilePortItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfilePortItemL" );
    CNSmlDMProfileData* item = Item(ENSmlProfilePort);
    iProfile->GetHostAddress( iBuf, item->iNumberData );
	
	if ( item->iNumberData <= KPortMaxSize && item->iNumberData >= KPortMinSize )
	{
	item->SetValue( item->iNumberData );    	
	}
	else
	{
	item->iNumberData = KDefaultHttpPort;	
	item->SetValue( item->iNumberData );
	}

	item->iLength = KBufSize16;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeNumber;	
	item->iHidden = ( Item( ENSmlConnType )->iNumberData != EProfileInternet );	
	item->iLatinInput = ETrue;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileUserNameandPassWordItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileUserNameandPassWordItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileUserNameandPassWordItemL" );
	
    CNSmlDMProfileData* item = Item(ENSmlUsername);
    HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
		                                         R_QTN_SML_SET_VAL_FIELD_NONE );
    item->SetValue( emptyText->Des() );
    CleanupStack::PopAndDestroy( emptyText );
    TBuf<KNSmlMaxTextLength256> buf;
    if ( EditMode() || CopyMode() )
		{
        iProfile->GetUserName( buf );
        if( buf.Length() > 0 )      
           {
        item->SetValue( buf );
           } 
		}
	item->iMandatory = ETrue;
	item->iLength = KNSmlMaxUsernameLength;
	item->iLatinInput = ETrue;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeText;

    item = Item(ENSmlPassword);
	if ( EditMode() || CopyMode() )
		{
        TBuf<KNSmlMaxItemLength> buf;
        iProfile->GetPassword( buf );
        item->iValue.Copy( buf );
		}
	else
		{
		item->iValue = KNullDesC;
		}

	buf.Fill('*', 4 ); //mask
	item->SetValue( buf );	
	item->iLength = KNSmlMaxPasswordLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeSecret;	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileActiceAcceptedItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileActiveAcceptedItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileActiceAcceptedItemL" );
	
	CNSmlDMProfileData* item = Item(ENSmlServerAlertAllowed);
    CDesCArray* stringData =
                         iEikonEnv->ReadDesCArrayResourceL( R_NSML_YESNO_TYPE );
	CleanupStack::PushL( stringData );

	if ( EditMode() || CopyMode() )
		{
        if ( iProfile->SASyncState() == ESASyncStateDisable)
		    {
		    item->iNumberData = EFalse;
		    }
		else
		    {
		    item->iNumberData = ETrue;
		    }
		}
	else
		{
		item->iNumberData = ETrue;
		}
	item->iResource = R_NSML_YESNO_TYPE;
	item->SetValue( (*stringData)[item->iNumberData] );
	item->iLength = 0;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeListYesNo;
	CleanupStack::PopAndDestroy( stringData );

	item = Item(ENSmlServerAccept);
	if ( iDoc->SANSupport() )
	{   
		item->iHidden = ETrue;
		return;
	}	
	stringData = iEikonEnv->ReadDesCArrayResourceL( R_NSML_ACCEPT_YESNO_TYPE );
	CleanupStack::PushL( stringData );
	if ( EditMode() || CopyMode() )
		{
        if ( iProfile->SASyncState() != ESASyncStateEnable )
		    {
		    item->iNumberData = EFalse;
		    }
		else
		    {
		    item->iNumberData = ETrue;
		    }
		}
	else
		{
		item->iNumberData = EFalse;
		}
	item->iResource = R_NSML_ACCEPT_YESNO_TYPE;
	item->SetValue( (*stringData)[item->iNumberData] );
	item->iLength = 0;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeListYesNo;
	CleanupStack::PopAndDestroy( stringData );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileHttpsUserNameAndPasswordItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileHttpsUserNameAndPasswordItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileHttpsUserNameAndPasswordItemL" );
	
	CNSmlDMProfileData* item = Item( ENSmlHttpsUsername );
	HBufC* emptyText = iEikonEnv->AllocReadResourceLC(
		                                         R_QTN_SML_SET_VAL_FIELD_NONE );
	item->SetValue( emptyText->Des() );
	CleanupStack::PopAndDestroy( emptyText );
    
    TBuf<KNSmlMaxTextLength256> buf;
    if ( EditMode() || CopyMode() )
		{
        iProfile->GetHttpUserNameL( buf );
        if( buf.Length() > 0 )      
           {
        item->SetValue( buf );
           } 
		}
	item->iMandatory = ETrue;
	item->iLength = KNSmlMaxUsernameLength;
	item->iLatinInput = ETrue;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeText;
	if( !iProfile->HttpUsedL() ||  Item( ENSmlConnType )->iNumberData != EProfileInternet )
	    {
	    item->iHidden = ETrue;
	    }
    
	item = Item(ENSmlHttpsPassword);
	if ( EditMode() || CopyMode() )
		{
        TBuf<KNSmlMaxItemLength> buf;
        iProfile->GetHttpPasswordL( buf );
        item->iValue.Copy( buf );
		}
	else
		{
		item->iValue = KNullDesC;
		}

	buf.Fill( '*', KNumberofCharacters ); //mask with 4 stars
	item->SetValue( buf );	
	item->iLength = KNSmlMaxPasswordLength;
	item->iItemType = CNSmlDMProfileData::ENSmlTypeSecret;
	if( !iProfile->HttpUsedL() ||  Item( ENSmlConnType )->iNumberData != EProfileInternet    )
	    {
	    item->iHidden = ETrue;
	    }	
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::BuildProfileHttpsUsedItemL
// -----------------------------------------------------------------------------
//
void CNSmlDMDlgProfileView::BuildProfileHttpsUsedItemL()
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::BuildProfileHttpsUsedItemL" );
	
	CNSmlDMProfileData* item = Item( ENSmlHttpsUsed );
	CDesCArray* stringData = 
	        iEikonEnv->ReadDesCArrayResourceL( R_NSML_HTTPS_ACTIVE_YESNO_TYPE );
	CleanupStack::PushL( stringData );
	if ( EditMode() || CopyMode() )
		{
        if ( iProfile->HttpUsedL() )
		    {
		    item->iNumberData = ETrue;
		    }
		else
		    {
		    item->iNumberData = EFalse;
		    }
		}
	else
		{
		item->iNumberData = EFalse;
		}
	item->iResource = R_NSML_HTTPS_ACTIVE_YESNO_TYPE;
	item->SetValue( (*stringData)[item->iNumberData] );
	item->iLength = 0;
	item->iItemType = CNSmlDMProfileData::ENSmlTypehttpListYesNo;
	item->iHidden = ( Item( ENSmlConnType )->iNumberData != EProfileInternet );    
	CleanupStack::PopAndDestroy( stringData );			
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::GetItemForIndex
// -----------------------------------------------------------------------------
//
CNSmlDMProfileData* CNSmlDMDlgProfileView::GetItemForIndex( TInt aIndex )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::GetItemForIndex:" );
	
	TInt index = 0;
	while ( ( aIndex != IndexItem(index)->iIndex ) &&
	         ( index < iProfileList->Count() ) )
		{
        index++;
		}
	return (*iProfileList)[index];
	}
	
//-----------------------------------------------------------------------------
// CNSmlDMDlgProfileView::IndexItem
//-----------------------------------------------------------------------------
//
CNSmlDMProfileData* CNSmlDMDlgProfileView::IndexItem( TInt aItemId )
	{
	FLOG( "[OMADM] CNSmlDMDlgProfileView::Item:" );
	
	CNSmlDMProfileData* item = NULL;
	TInt count = iProfileList->Count();
	for ( TInt index = 0; index < count; index++ )
		{
		CNSmlDMProfileData* temp = iProfileList->At(index);
		if ( temp->iItemId == aItemId )
			{
			item = temp;
			break;
			}
		}
	__ASSERT_DEBUG( item, TUtil::Panic( KErrGeneral ) );
	return item;
	}

//-----------------------------------------------------------------------------
// CNSmlDMTextEditor::CNSmlDMTextEditor()
//-----------------------------------------------------------------------------
//
CNSmlDMTextEditor::CNSmlDMTextEditor(TInt aResourceID, 
                                                     TDes& aText, 
                                                     TInt aTextSettingPageFlags)                      
 : CAknTextSettingPage(aResourceID, aText, aTextSettingPageFlags)
	{
		
	}
//-----------------------------------------------------------------------------
// CNSmlDMTextEditor::OfferKeyEventL()
//-----------------------------------------------------------------------------
//
TKeyResponse CNSmlDMTextEditor::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
                                                 TEventCode aType)
    {
		    if(EKeyEnter == aKeyEvent.iCode)
		    {
		    	ProcessCommandL(EAknSoftkeySelect);
		    	return EKeyWasConsumed;
		    }
		    return CAknTextSettingPage::OfferKeyEventL(aKeyEvent, aType);
    }
//-----------------------------------------------------------------------------
// CNSmlDMTextEditor::~CNSmlDMTextEditor()
//-----------------------------------------------------------------------------
//    
CNSmlDMTextEditor::~CNSmlDMTextEditor()
  {
	
  }    
// End of File
