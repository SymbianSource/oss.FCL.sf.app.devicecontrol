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
* Description:  Methods for the firmware update view 
*
*/



// INCLUDE FILES
#include <AknIconArray.h>
#include <AknQueryDialog.h>
#include <AknsUtils.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <SyncMLNotifierDomainCRKeys.h>

#include <e32property.h> 
#include "NSmlDMSyncApp.h"
#include "nsmldmsyncinternalpskeys.h"

#include "NSmlDMSyncPrivateCRKeys.h"

#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMFotaModel.h"
#include "NSmlDMFotaView.h"
#include "NSmlDMFotaContainer.h"
#include "NSmlDMFotaSettingsDialog.h"
#include "NSmlDMdef.h"

#include "NSmlDMSyncUi.hrh"
#include <NSmlDMSync.rsg>
#include <nsmldmsync.mbg>
#include "NSmlDMSyncDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::ConstructL()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::ConstructL()" );
    CNSmlDMSyncDocument* doc = ( CNSmlDMSyncDocument* ) AppUi()->Document();
    if( doc->IsServerAlertSync() ) //For embedded application opening
       {       	      
       BaseConstructL( R_CP_SMLSYNC_FOTA_VIEW );
       }
    else     //Independent application opening
       {
       BaseConstructL( R_SMLSYNC_FOTA_VIEW );	
       }       
    iExitPress = -1;
    FeatureManager::InitializeLibL();
    iFotaDLObserver = new (ELeave) CDLObserver;
    iFotaDLObserver->ConstructL(this);
    iFotaDLObserver->StartL(EFalse);
    FLOG( "[OMADM]\t CNSmlDMFotaView::ConstructL() completed" );
    }

// Destructor
CNSmlDMFotaView::~CNSmlDMFotaView()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::~CNSmlDMFotaView()" );
   
    if(iFotaDLObserver)
    	{
    	iFotaDLObserver->Cancel();
    	delete iFotaDLObserver;
    	iFotaDLObserver = NULL;
    	}    
    
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;
     FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::Id
// -----------------------------------------------------------------------------
//
TUid CNSmlDMFotaView::Id() const
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::Id()" );
    return KNSmlDMFotaViewId;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::HandleCommandL
// Redirects commands to the appui class.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::HandleCommandL( TInt aCommand )
    {
    FTRACE( FPrint( _L("[OMADM]\t CNSmlDMFotaView::HandleCommandL(), aCommand = 0x%x"),
        aCommand ) );

    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            UpdateExitL();
            TInt value=-1;
            TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,value);
            AppUi()->HandleCommandL( EEikCmdExit );
            break;
            }
        /*case EAknCmdExit: 
        case EEikCmdExit:
            {
            STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->CloseGs(); //control panel            
            break;
            }*/

        case ENSmlMenuCmdFotaCheck:
            {            
            if(TUtil::ShowNativeDialogL(EPrivacyPolicy) == KErrNone)
                {
                InitiateFotaCheckL();
                }
            break;
            }
        case ENSmlMenuCmdFotaInstall:
            {
            InitiateFwUpdInstallL();
            break;
            }
        case ENSmlMenuCmdFotaContinue:
            {
            RefreshL(EFalse);
            InitiateFwUpdContinueL();
            break;
            }
        case ENSmlMenuCmdFotaSettings:
            {
            TInt value = ETrue;                 //IAD: Making DM busy
            TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KDMIdle,value);	
            CNSmlDMFotaSettingsDialog* settingDialog = CNSmlDMFotaSettingsDialog::NewL();
            settingDialog->ExecuteLD( R_NSML_FOTA_SETTINGS_DIALOG );
            break;
            }
 
        case ENSmlMenuCmdFotaServers:
            {
            TInt value = ETrue;    //IAD: Making DM busy
            TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KDMIdle,value);	
             AppUi()->ActivateLocalViewL( KNSmlDMProfilesViewId );
             break;
            }    
 
        case EEikCmdExit:
        case EAknCmdExit:
            {
            UpdateExitL();	
            STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi() )->CloseGs(); //control panel            
            break;
            }
        default:
            {
            UpdateExitL();	
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
        
    FLOG( "[OMADM]\t CNSmlDMFotaView::HandleCommandL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::StepL
// Does nothing.
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::StepL()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::IsProcessDone
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaView::IsProcessDone() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::InitiateFotaCheckL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::InitiateFotaCheckL()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFotaCheck()" );
    TInt profileId( KErrNotFound );
    TBool isValidId( EFalse );    
    profileId = iFotaModel->DefaultFotaProfileIdL();

    if ( profileId >= KErrNone )
        {
         isValidId = iFotaModel->VerifyProfileL( profileId );
        }

    if( ( profileId == KErrNotFound ) || ( !isValidId ) )
        {
    	profileId = ShowFotaProfileQueryL();
    	if( profileId == KErrNotFound )    
    	  return;
        }
        __ASSERT_ALWAYS( iFotaModel, TUtil::Panic( KErrGeneral ) );
        
        iFotaModel->EnableFwUpdRequestL( profileId );
        ( ( CNSmlDMSyncAppUi* ) AppUi() )->StartSyncL( profileId, ETrue );
    	iFotaDLObserver->StartL(ETrue);
    
    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFotaCheck() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::ShowFotaProfileQueryL
// -----------------------------------------------------------------------------
//
TInt CNSmlDMFotaView::ShowFotaProfileQueryL()
    {
    FLOG( "[OMADM] CNSmlDMFotaView::ShowFotaProfileQueryL()" );

    TInt retval( KErrNotFound );

    CDesCArray* items = new (ELeave) CDesCArrayFlat( KNSmlDefaultGranularity );
    CleanupStack::PushL( items );
    items->Reset();

    // Profile list owned by the dialog
    CArrayFixFlat<TInt>* profileIdList =
        new ( ELeave ) CArrayFixFlat<TInt>( KNSmlDefaultGranularity );
    CleanupStack::PushL( profileIdList );

    FormatProfileQueryListItemsL( items, profileIdList );

    if ( items->Count() > 1 )
        {
    	CEikFormattedCellListBox* listBox = new ( ELeave )
    	    CAknDoubleLargeGraphicPopupMenuStyleListBox;
    	CleanupStack::PushL( listBox );

    	CAknPopupList* popupList = CAknPopupList::NewL( listBox, 
    							   R_AVKON_SOFTKEYS_OK_CANCEL__OK,
            				 AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
    	CleanupStack::PushL( popupList );

    	listBox->ConstructL( popupList, ( EAknListBoxSelectionList | EAknListBoxLoopScrolling ) );
    	listBox->Model()->SetItemTextArray( items );
    	listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
        listBox->HandleItemAdditionL();
    	
        listBox->CreateScrollBarFrameL( ETrue );
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    	CAknIconArray* icons = new (ELeave) CAknIconArray( 10 );
    	CleanupStack::PushL( icons );
        CreateIconArrayL( icons );
    	listBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    	CleanupStack::Pop( icons );

        HBufC* popupTitle = iCoeEnv->AllocReadResourceLC(
            R_QTN_FOTA_LIST_QUERY_SELECT_SERVER_PROFILE );
        popupList->SetTitleL( popupTitle->Des() );
        CleanupStack::PopAndDestroy( popupTitle );
        popupTitle = NULL;

        CleanupStack::Pop( popupList );
        TBool queryResult = popupList->ExecuteLD();
        popupList = NULL;

        if ( queryResult )
            {
            retval = profileIdList->At( listBox->CurrentItemIndex() );
            }
        CleanupStack::PopAndDestroy( listBox );  // Destroys also the icon array
        }
    else if ( items->Count() == 1 )
        {
        retval = profileIdList->At(0);
        }
    else
        {
        
        TInt ServersViewVariable( 0 );
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
        if ( centrep )
            {
            centrep->Get( KNSmlDMServersMenuKey, ServersViewVariable );         
            delete centrep;
            }
        if ( err != KErrNone ) 
            {
            User::Leave( err );
            }    
        if( ServersViewVariable == 0)   
            {
            CleanupStack::PopAndDestroy( profileIdList );
            CleanupStack::PopAndDestroy( items );
         	return KErrNotFound;
            }
        
        
		HBufC* newProfileString = 
		            iEikonEnv->AllocReadResourceLC( R_SML_APPUI_NOTE_TEXT_NEW );

		CAknQueryDialog* newProfileDialog = new ( ELeave ) CAknQueryDialog;

		if ( newProfileDialog->ExecuteLD( R_SML_CONFIRMATION_QUERY,
		                                  newProfileString->Des() ) )
			{	
			
			TInt viewtitle = 1;
			( (CNSmlDMSyncAppUi*) AppUi() )->
			                         SetTitleCall( viewtitle );	
			                       
			( (CNSmlDMSyncAppUi*) AppUi() )->
			    ShowEditProfileDialogL( ESmlNewProfile );
			}
		newProfileDialog = NULL;
		CleanupStack::PopAndDestroy( newProfileString );
		
        }

    CleanupStack::PopAndDestroy( profileIdList );
    CleanupStack::PopAndDestroy( items );

    FTRACE( FPrint(
        _L("[OMADM]\t CNSmlDMFotaView::ShowFotaProfileQueryL() completed, retval = %d"),
        retval ) );
    return retval;
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::FormatProfileQueryListItemsL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::FormatProfileQueryListItemsL( CDesCArray*          aItems,
                                                    CArrayFixFlat<TInt>* aProfileIdList )
    {
    FLOG( "[OMADM] CNSmlDMFotaView::FormatProfileQueryListItemsL()" );
    TInt index( 0 );
    
    CNSmlDMSyncDocument* doc = ( CNSmlDMSyncDocument* ) AppUi()->Document();
    
    doc->RefreshProfileListL( ETrue ); // Include hidden profile
    CArrayFixFlat<TNSmlDMProfileItem>* profileList = doc->ProfileList( index );

    TBuf<KNSmlMaxTextLength256> text;

    for ( index = 0; index < profileList->Count(); index++ )
        {
        text.Zero();
        TInt iconIndex( EProfileInternet ); //internet

        if ( ( *profileList )[index].iBearer != EProfileInternet )
            {
            iconIndex = EProfileObexBlueTooth;	
            }
        text.Num( iconIndex );
        text.Append( KNSmlTab );
        text.Append( ( *profileList )[index].iProfileName );
        text.Append( KNSmlTab );
        
        aItems->AppendL( text );
        aProfileIdList->AppendL( ( *profileList )[index].iProfileId );
        }
    FLOG( "[OMADM] CNSmlDMFotaView::FormatProfileQueryListItemsL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::CreateIconArrayL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::CreateIconArrayL( CAknIconArray* aIconArray )
    {
    FLOG( "[OMADM] CNSmlDMFotaView::CreateIconArrayL()" );

    HBufC* appIconFilePath = HBufC::NewLC( 5 + KDC_APP_BITMAP_DIR().Length() 
                                             + KDMAppIconFileName().Length() );
    TPtr ptr = appIconFilePath->Des();
    ptr.Append( KZDrive );
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KDMAppIconFileName );

	aIconArray->AppendL( AknsUtils::CreateGulIconL( 
	                             AknsUtils::SkinInstance(),
	                             KAknsIIDQgnPropSmlHttp,
	                             *appIconFilePath,
	                             EMbmNsmldmsyncQgn_prop_sml_http,
	                             EMbmNsmldmsyncQgn_prop_sml_http_mask ) );
	                             
	aIconArray->AppendL( AknsUtils::CreateGulIconL( 
	                          AknsUtils::SkinInstance(),
	                          KAknsIIDQgnPropSmlHttpOff,
	                          *appIconFilePath,
	                          EMbmNsmldmsyncQgn_prop_sml_http_off,
	                          EMbmNsmldmsyncQgn_prop_sml_http_off_mask ) );

	if ( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmObex ) )
		{
		aIconArray->AppendL( AknsUtils::CreateGulIconL( 
		                            AknsUtils::SkinInstance(),
		                            KAknsIIDQgnPropSmlBt,
		                            *appIconFilePath,
		                            EMbmNsmldmsyncQgn_prop_sml_bt,
		                            EMbmNsmldmsyncQgn_prop_sml_bt_mask ) );
		                            
		aIconArray->AppendL( AknsUtils::CreateGulIconL(
		                        AknsUtils::SkinInstance(),
		                        KAknsIIDQgnPropSmlBtOff,
		                        *appIconFilePath,
		                        EMbmNsmldmsyncQgn_prop_sml_bt_off,
		                        EMbmNsmldmsyncQgn_prop_sml_bt_off_mask ) );
		}
	
    CleanupStack::PopAndDestroy( appIconFilePath );

    FLOG( "[OMADM] CNSmlDMFotaView::CreateIconArrayL() completed" );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMFotaView::InitiateFwUpdInstallL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::InitiateFwUpdInstallL()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFwUpdInstallL()" );
    
    TInt pkgId = iFotaModel->RetrieveFwUpdPkgIdListL(
        EStartingUpdate );
    
    if ( pkgId != KErrNotFound )
        {
        TInt retval = iFotaModel->InitiateFwUpdInstall( pkgId, KErrNotFound );

        // No profile id was stored in the fota engine. Trying to select a suitable
        // profile, or asking user.
        if ( retval == KErrNotFound )
            {
            TBool isValidId( EFalse );
            TInt profileId = iFotaModel->DefaultFotaProfileIdL();

            if ( profileId >= KErrNone )
                {
                 isValidId = iFotaModel->VerifyProfileL( profileId );
                }

            if( ( profileId == KErrNotFound ) || ( !isValidId ) )
                {
            	profileId = ShowFotaProfileQueryL();
                }
            
            // Retrying the update with the chosen profile id.
            iFotaModel->InitiateFwUpdInstall( pkgId, profileId );
            }
        
        }

    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFwUpdInstallL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::InitiateFwUpdContinueL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::InitiateFwUpdContinueL()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFwUpdContinueL()" );    
    iFotaDLObserver->StartL(ETrue);
    iFotaModel->ContinueFwUpdInstall();
    FLOG( "[OMADM]\t CNSmlDMFotaView::InitiateFwUpdContinueL() completed" );
    }
        
// -----------------------------------------------------------------------------
// CNSmlDMFotaView::DoActivateL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                         TUid /*aCustomMessageId*/,
                                   const TDesC8& /*aCustomMessage*/ )
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::DoActivateL()" );

    CNSmlDMSyncDocument* doc = ( CNSmlDMSyncDocument* ) AppUi()->Document();
    __ASSERT_ALWAYS( doc, TUtil::Panic( KErrGeneral ) );
    
    	MenuBar()->SetContextMenuTitleResourceId( 
    	                              R_SMLSYNC_CONTXT_MENUBAR_FOTA_VIEW );
    
    //MANUAL CHECK
    if( !ManualCheckAllowedL() )
       {    	    
    Cba()->MakeCommandVisible(EAknSoftkeyContextOptions,EFalse);
    Cba()->DrawDeferred();    
       }
    iFotaModel = doc->FotaModel();
    iContainer = CNSmlDMFotaContainer::NewL( this, ClientRect(), iFotaModel );
    AppUi()->AddToStackL( *this, iContainer );


	CAknTitlePane* titlePane = 
	    (CAknTitlePane*)(AppUi()->StatusPane())->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
    HBufC* titleText = iEikonEnv->AllocReadResourceLC( R_CP_APP_TITLE );	
	titlePane->SetTextL( titleText->Des() );
	CleanupStack::PopAndDestroy(titleText);	
    
    FLOG( "[OMADM]\t CNSmlDMFotaView::DoActivateL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::DoDeactivate
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::DoDeactivate()
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::DoDeactivate()" );

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

    delete iContainer;
    iContainer = NULL;

    FLOG( "[OMADM]\t CNSmlDMFotaView::DoDeactivate() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
	{
	FLOG( "[OMADM] CNSmlDMFotaView::DynInitMenuPaneL()" );
     
    if ( aResourceId == R_SMLSYNC_MENU_COMMON && 
                    ! FeatureManager::FeatureSupported( KFeatureIdHelp ) )
      {   
          aMenuPane->SetItemDimmed( EAknCmdHelp , ETrue );
          
      }        
    if ( aResourceId == R_SMLSYNC_MENU_FOTA_VIEW 
        || aResourceId == R_SMLSYNC_CONTXT_MENU_FOTA_VIEW )
        {
        __ASSERT_ALWAYS( iFotaModel, TUtil::Panic( KErrGeneral ) );
          
        // Set all variated items first dimmed and show them according to the
        // configuration flags.
  
        aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaCheck, ETrue );
        aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaInstall, ETrue );
        aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaContinue, ETrue );
        TInt configFlags( 0 );
        
        TInt ServersViewVariable( 0 ),SettingsVariable(0);
        
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
        FTRACE( FPrint(
            _L("[OMADM]\t CNSmlDMFotaView::DynInitMenuPaneL() centrep open returned %d"),
            err ) );

        if ( centrep )
            {
            centrep->Get( KNSmlDMFotaConfigurationKey, configFlags );
            centrep->Get( KNSmlDMServersMenuKey, ServersViewVariable );
            centrep->Get( KNSmlDMSettingsMenuKey, SettingsVariable );
            delete centrep;
            }

        // If there was problems with the centrep key, the context menu will
        // be empty. However, the framework does not handle the situation correctly
        // and we'll have to leave here in order not to break things.
        // The Options menu shows at least the Exit item and can be shown.
        if ( err != KErrNone && aResourceId == R_SMLSYNC_CONTXT_MENU_FOTA_VIEW ) 
            {
            User::Leave( err );
            }
      
      if( aResourceId == R_SMLSYNC_MENU_FOTA_VIEW )
      {      	
      if(SettingsVariable )
        {
        aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaSettings, EFalse );
        }
      else
        {
        aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaSettings, ETrue );	
        }      
      }
           
        // if ( configFlags & KNSmlDMFotaEnableManualCheckFlag )
      if ( ManualCheckAllowedL() )
            {
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaCheck, EFalse );
            }
      if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) ) //NFUI
      {
        if ( iFotaModel->FindFwUpdPkgWithStateL(
            EDownloadProgressingWithResume ) != KErrNotFound )
            {
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaInstall, ETrue );          
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaCheck, ETrue );
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaContinue, EFalse );          
            }
        else if ( iFotaModel->FindFwUpdPkgWithStateL(
            EStartingUpdate ) != KErrNotFound )
            {
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaInstall, EFalse );          
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaCheck, ETrue );          
            aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaContinue, ETrue );          
            }
        }
   
      if( aResourceId == R_SMLSYNC_MENU_FOTA_VIEW )
        {
        if( ServersViewVariable )
           {
           aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaServers, EFalse );	
           }
        else
           {
           aMenuPane->SetItemDimmed( ENSmlMenuCmdFotaServers, ETrue );	
           }
        
        }
      
	    }
	   FLOG( "[OMADM] CNSmlDMFotaView::DynInitMenuPaneL() completed" );
	}


// -----------------------------------------------------------------------------
// CNSmlDMFotaView::RefreshL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::RefreshL(TBool aPostponeDisplay)
    {
    FLOG( "[OMADM]\t CNSmlDMFotaView::RefreshListBox()" );

    if ( iContainer )
        {
        iContainer->RefreshL(aPostponeDisplay);
        }
    FLOG( "[OMADM]\t CNSmlDMFotaView::RefreshListBox() completed" );
    }	


// -----------------------------------------------------------------------------
// CNSmlDMFotaView::ManualCheckAllowed
// -----------------------------------------------------------------------------
//
TBool CNSmlDMFotaView::ManualCheckAllowedL()
	{
	CRepository* cenrep = NULL;
    TRAPD( error, cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );        
    if ( cenrep )
        {
        cenrep->Get( KNsmlDmManualCheckAllowed, iManCheckAllowed );
        delete cenrep;
        }
    if ( error != KErrNone ) 
        {
        User::Leave( error );
        }
	if ( iManCheckAllowed == 1)
	    {
		return ETrue;		
	    }
 
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMFotaView::HandleForegroundEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::HandleForegroundEventL(TBool aForeground)
	{
	
	// For Application Management purpose whenever there is switch between app.
	
	
	
	FLOG( "[OMADM]\t CNSmlDMFotaView::HandleForegroundEventL():Begin" );
	if( !aForeground )
		{
		FLOG( "[OMADM]\t CNSmlDMFotaView::HandleForegroundEventL() not in FG :end" );
	    return;	
		}	
	
	
	if( iExitPress != -2 ) // == 1 some what fast as no call to refreshL based on iexitPress
		{
		iExitPress = -1;	
		STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi())->CheckFotaDlL();
		FLOG( "[OMADM]\t CNSmlDMFotaView::HandleForegroundEventL(): CheckAMDlL Begin" );
		STATIC_CAST( CNSmlDMSyncAppUi*, iEikonEnv->EikAppUi())->CheckAMDlL();
		FLOG( "[OMADM]\t CNSmlDMFotaView::HandleForegroundEventL(): CheckAMDlL END" );
		}
	iFotaDLObserver->StartL(EFalse);
	
	if ( !ManualCheckAllowedL() && iExitPress != -2 )	//for MSK display
	    {
	    if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) )
	       {
	       if ( iFotaModel->FindFwUpdPkgWithStateL(
            EStartingUpdate ) != KErrNotFound )
	    	{
	        EnableMskL( ETrue );	
	    	}	
	       }	    
	    iExitPress = -1;	
	    }
	CAknView::HandleForegroundEventL(aForeground);
	FLOG( "[OMADM]\t CNSmlDMFotaView::HandleForegroundEventL():completed" );
	}    
	
// -----------------------------------------------------------------------------
// CNSmlDMFotaView::EnableMskL
// -----------------------------------------------------------------------------
//
void CNSmlDMFotaView::EnableMskL ( TBool aEnable )	
	{
	CEikButtonGroupContainer* cbaGrp= Cba();
	if (cbaGrp)
		{
		if (aEnable)
			{						
			cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions,ETrue);	    	
			}
		else
			{
			cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions,EFalse);
			}		
		}
	}

// ----------------------------------------------------
// CNSmlDMFotaView::UpdateExitL
// ----------------------------------------------------
//	
void CNSmlDMFotaView::UpdateExitL()
	{
	iExitPress = -2;	
	}	


// -----------------------------------------------------------------------------
// CDLObserver::CDLObserver()
// -----------------------------------------------------------------------------

inline CDLObserver::CDLObserver()
	:CActive(EPriorityStandard)
	{
	
	}
	
	
// -----------------------------------------------------------------------------
// CDLObserver::ConstructL()
// -----------------------------------------------------------------------------

void CDLObserver::ConstructL(CNSmlDMFotaView* iFotaView)
	{
	CActiveScheduler::Add(this);	
    iView = iFotaView;
	}

// -----------------------------------------------------------------------------
// CDLObserver::Start()
// -----------------------------------------------------------------------------
void CDLObserver::StartL(TBool aResetKey)
	{	
	FLOG("CDLObserver::Start- begin");
	if(!IsActive())
		{
		//set key value to -1			
		FLOG("logging new request");
		TInt err = iProperty.Attach(KPSUidNSmlDMSyncApp,KFotaDLRefresh);
		FTRACE( FPrint(_L("CDLObserver::StartL attached to P&S key with error as %d"),err ) );		
		CNSmlDMSyncDocument* doc = 
		    ( CNSmlDMSyncDocument* ) iView->AppUi()->Document();
		if( aResetKey && doc->DMDefinesFotaDLKey())		
		 {		 
		 TInt value = -1;
		 err = iProperty.Set(KPSUidNSmlDMSyncApp,KFotaDLRefresh,value);
		 FTRACE( FPrint(_L("CDLObserver::StartL updated the value of p&S key with error %d"),err ) );
		 }
		iStatus=KRequestPending;
		iProperty.Subscribe(iStatus);
		FLOG("CDLObserver::StartL subscribed to p&S ");
		SetActive();	
		}		   
	}

// -----------------------------------------------------------------------------
// CDLObserver::RunL()
// -----------------------------------------------------------------------------
void CDLObserver::RunL()
	{
	FLOG("CDLObserver::RunL()- begin");
	TInt value;	
	iProperty.Attach(KPSUidNSmlDMSyncApp,KFotaDLRefresh);
	iProperty.Get(KPSUidNSmlDMSyncApp,KFotaDLRefresh,value);
	TInt state (EIdle);
	state = iView->iFotaModel->GetCurrentFwUpdState();
	FTRACE( FPrint(_L("current state of fota is %d"),state ) );
	//EFALSE for suspended,failed & completed
	//Download Ended either successfully or unsuccessfully
	if( value == EFalse ) 
		{			
		FLOG("CDLObserver::RunL()--status value = 0");			
		if (state == EDownloadProgressingWithResume || 
				state == EDownloadComplete ||
				state == EStartingUpdate ||
				state == EDownloadFailed)
			{
			TInt IsDMBusy = EFalse;    //IAD: Making DM Idle 
			TInt r2=RProperty::Set(KUidSmlSyncApp,KDMIdle,IsDMBusy);	
			iView->RefreshL(ETrue);				
			iProperty.Subscribe(iStatus);
			FLOG("CDLObserver::RunL()--issued request in the loop EFalse");
			SetActive();
			}
		}
	else if( value == 1 ) //resume case and update the UI
		{		
		FLOG("CDLObserver::RunL()--status value = 1");
		if (state == EDownloadProgressingWithResume || 
				state == EDownloadProgressing || 
				state == EStartingDownload )
			{
			TInt IsDMBusy = ETrue;    //IAD: Making DM Busy
			TInt r2=RProperty::Set(KUidSmlSyncApp,KDMIdle,IsDMBusy);		
			FLOG("CDLObserver::RunL():call to view refresh");
			iView->RefreshL(EFalse);
			}
		iProperty.Subscribe(iStatus);
		FLOG("CDLObserver::RunL()--issued request in the loop ETRUe");
		SetActive();

		}
	else 
		{			
		FTRACE( FPrint(_L("CDLObserver::RunL()-- else loop & value is %d"),value ) );
		if(!IsActive())
			{
			iStatus=KRequestPending;
			value = -1;
			iProperty.Set(KPSUidNSmlDMSyncApp,KFotaDLRefresh,value);
			iProperty.Subscribe(iStatus);
			FLOG("CDLObserver::RunL()--issued request again");
			SetActive();
			}				
		}

	}

// ----------------------------------------------------------------------------------------
// CDLObserver::DoCancel() 
// ----------------------------------------------------------------------------------------
void CDLObserver::DoCancel()
	{
	FLOG("CDLObserver::::DoCancel()");
    if( iStatus == KRequestPending )
    	{	
    	iProperty.Cancel();// new fix to be tested thoroughly
    	TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrCancel );
    	}
	}

// ----------------------------------------------------------------------------------------
// CDLObserver::~CDLObserver() 
// ----------------------------------------------------------------------------------------
CDLObserver::~CDLObserver()
	{ 
	FLOG("CDLObserver::~CDLObserver()");  
    Cancel();
    if (iProperty.Handle()) iProperty.Close();
	}


// ---------------------------------------------------------------------------
// CDLObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CDLObserver::RunError( TInt aError )
    {
    return aError;
    }

//  End of File
