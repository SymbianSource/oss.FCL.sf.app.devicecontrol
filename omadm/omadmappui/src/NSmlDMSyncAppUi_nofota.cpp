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
* Description:  Methods for Device Manager Application UI
*
*/


#include <e32property.h>
#include <APGWGNAM.H>
#include <gsfwviewuids.h>
#include "nsmldmsyncinternalpskeys.h"

#include <aknnotewrappers.h>

#include <aknnavide.h>
#include <stringloader.h>
#include <textresolver.h>

#include "nsmldmsyncappui.h"
#include "nsmldmsyncapp.h"
#include "nsmldmsyncappengine.h"
#include "nsmldmprofilesview.h"
#include "nsmldmsyncdocument.h"
#include "nsmldmdlgprofileview.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmsyncdialog.h"
#include "nsmldmsyncprofile.h"
#include <nsmldmsync.rsg>
#include <FeatMgr.h>

#include <hlplch.h>
#include <csxhelp/dm.hlp.hrh>


// ============================ MEMBER FUNCTIONS ==============================
//

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ConstructL()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ConstructL()" );

    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    iSyncDocument = (CNSmlDMSyncDocument*) iDocument;
    iSyncAppEngine = iSyncDocument->AppEngine();
    iSyncAppEngine->RequestSyncStatus( this );

	iNaviDecorator = 0;

    iProfilesView = new (ELeave) CNSmlDMProfilesView();
    iProfilesView->ConstructL(); // this );
    AddViewL( iProfilesView ); // Transfer ownership to CAknViewAppUi
    
    ActivateLocalViewL( iProfilesView->Id() );

	iDialogUp       = 0;
	iDestroyed      = 1;

    FLOG( "[OMADM] CNSmlDMSyncAppUi::ConstructL() completed" );
    }

// ------------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------------

CNSmlDMSyncAppUi::~CNSmlDMSyncAppUi()
    {
    FLOG( "[OMADM] ~CNSmlDMSyncAppUi()" );

	delete iNaviDecorator;
	if ( iDestroyed == 0 )
		{
		delete iAppView;
		}
	}



// ------------------------------------------------------------------------------
// CNSmlDMSyncAppUi::DynInitMenuPaneL
// ------------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::DynInitMenuPaneL( TInt /*aResourceId*/,
                                         CEikMenuPane* /*aMenuPane*/ )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::DynInitMenuPaneL:" );
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleKeyEventL
// ------------------------------------------------------------------------------
//
TKeyResponse CNSmlDMSyncAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                            	TEventCode aType )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleKeyEventL:" );

	//EEventKeyDown ends settdialog
	if ( iDialogUp != 0 ) 
		{
		return EKeyWasConsumed;
		}
	if ( aType == EEventKey )
		{
		switch ( aKeyEvent.iCode )
			{
			case EKeyUpArrow:
				{
				if ( !Synchronise() )
					{
					HandleUpScrollL();
					}
				}
		    	break;

			case EKeyDownArrow:
				{
				if ( !Synchronise() )
					{
					HandleDownScrollL();
					}
				}
			    break;

			case EKeyPhoneEnd:
			case EKeyEscape:
				{
				if ( Synchronise() )
					{
					iSyncAppEngine->CancelSyncL();
					}						
				}
			    break;
			default:
      
				return EKeyWasNotConsumed;		
			}
		}
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleKeyEventL() completed" );
    return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::HandleCommandL( TInt aCommand )
    {
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::HandleCommandL() command = 0x%x"),
        aCommand ) );

    iSyncDocument->DisableDbNotifications( ETrue );
	iBusy = ETrue;

    switch ( aCommand )
        {
     case EAknCmdHelp:              //ENSmlMenuCmdMainHelp:
		  	{
			  	if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
           {
                 HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppHelpContextL() );
			           iBusy = EFalse;
			     }    
			 
			
			  }
    		break;
		case EAknCmdExit:
        case EEikCmdExit:		// quit application
			{
			TInt value=-1;
            TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,value);
			Exit();
			}
		    break;
		    
		case ENSmlMenuCmdNewProfile:	// create new profile
			{
			if (TUtil::SettingEnforcementState())
				{
				ShowResourceErrorNoteL( R_QTN_DM_INFO_NOTE_LOCKED_SETTING );
				break;
				}
            HandleCommandNewProfileL();
			}
		    break;

		case ENSmlMenuCmdEraseProfile:	// delete profile
			{
			if (TUtil::SettingEnforcementState())
				{
				ShowResourceErrorNoteL( R_QTN_DM_INFO_NOTE_LOCKED_SETTING );
				break;
				}
            HandleCommandEraseProfileL();
			}
    		break;
		
		case ENSmlMenuCmdOpenLog:			// open syncronisation log view
			{
			CNSmlDMSyncDialog::ShowDialogL( 
            			      iSyncDocument->ProfileItem()->iProfileId,
			                  iSyncAppEngine->Session() );
			iBusy = EFalse;
			}
		    break;

		case ENSmlMenuCmdOpenSettings:		// open profile for editing
			{
			if (TUtil::SettingEnforcementState())
				{
				ShowResourceErrorNoteL( R_QTN_DM_INFO_NOTE_LOCKED_SETTING );
				break;
				}
			ShowEditProfileDialogL( ESmlEditProfile );
			}
		    break;
		
		case ENSmlMenuCmdStartSync:			// start synchronisation
			{
            if ( ShowConfirmationNoteL( R_QTN_SML_CONF_STARTQUERY ) )
                {
                StartSyncL();
                }
			}
		    break;
		
		default:
            break;      
        }
    iSyncDocument->DisableDbNotifications( EFalse );
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleCommandL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleUpScrollL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::HandleUpScrollL()
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleUpScrollL:" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleDownScrollL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::HandleDownScrollL()
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleDownScrollL:" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ShowEditProfileDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ShowEditProfileDialogL( TNSmlEditMode aMode,
                                               TInt /*aStartUp*/)
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowEditProfileDialogL:" );

	TInt profileId( KErrNotFound );
	if ( aMode != ESmlNewProfile )
		{
		profileId = iSyncDocument->ProfileItem()->iProfileId;
		}
    
    iAppView = CNSmlDMDlgProfileView::NewL( 
                                    iSyncDocument,
	                                aMode,
	                                profileId );
	  	                               
	TRAPD( error, iAppView->ExecuteLD( R_NSML_SETTING_DIALOG ) );
	if ( error != KErrNone )
		{
		ShowErrorNoteL( error );
		}
	else
		{
		iBusy = ETrue;
		iActiveView = CNSmlDMSyncAppUi::SettingsView;
		AddToStackL( iAppView );
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ProcessCommandParametersL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppUi::ProcessCommandParametersL( TApaCommand /*aCommand*/,
                                                   TFileName& aDocumentName,
                                                   const TDesC8& /*aTail*/ )
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ProcessCommandParametersL:" );

	 // This is to prevent dummy document creation
	aDocumentName.Zero();
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::StartSyncL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::StartSyncL(
    const TInt aProfileId, // = KErrNotFound
    const TBool /*aUseFotaProgressNote*/ )
	{
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::StartSyncL() aProfileId = %d"),
        aProfileId ) );

	if ( aProfileId == KErrNotFound )
	    {
        iConfedProfileId = iSyncDocument->ProfileItem()->iProfileId;
	    }
	else
	    {
	    iConfedProfileId = aProfileId;
	    }

    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::StartSyncL() used profileId = %d"),
        iConfedProfileId ) );
        
	CNSmlDMSyncProfile* profile = 
	            iSyncAppEngine->OpenProfileL( iConfedProfileId, ESmlOpenReadWrite );

    TInt connectionBearer = profile->BearerType();
    
    TBuf<KNSmlMaxProfileNameLength> serverName;
    profile->GetName( serverName );
	
	if ( profile->SASyncState() == ESASyncStateDisable )
		{
		if ( ShowConfirmationNoteL( R_QTN_SML_CONF_ACTIVEQUERY ) )
			{
			profile->SetSASyncStateL( ESASyncStateEnable );
			profile->SaveL();
			}
		else
			{
			iSyncAppEngine->CloseProfile();
			return;
			}
		}

    iSyncAppEngine->CloseProfile();		
    
    // Since the FOTA is not supported, the simpler progress note
    // is also never used and the EFalse can be given as parameter.
    TRAPD( error, iSyncAppEngine->SynchronizeL( serverName, 
                                                iConfedProfileId, 
                                                connectionBearer,
                                                EFalse ) );
	
	if (error == KErrNone)
		{
		iActiveView = CNSmlDMSyncAppUi::ProgressView;
		}
	else
		{
		ShowErrorNoteL( error );
		iActiveView = CNSmlDMSyncAppUi::MainView;
		}

    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::StartSyncL() completed, error = %d"),
        error ) );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ChangeViewL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ChangeViewL(TBool aRefresh)
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ChangeViewL:" );

	iActiveView = CNSmlDMSyncAppUi::MainView;
	if ( aRefresh != EFalse )
		{
		iSyncDocument->RefreshProfileListL();
        iProfilesView->RefreshL();
		}
	CAknTitlePane* titlePane = 
	    (CAknTitlePane*) StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
	HBufC* titleText = NULL;    

	if ( iViewTitle )
	{
	titleText = iEikonEnv->AllocReadResourceLC( R_SML_APP_TITLE );
	iViewTitle = 0;	
	}
	else
	{
	titleText = iEikonEnv->AllocReadResourceLC( R_SML_PROFILES_TITLE );	
	}	
	
	titlePane->SetTextL( titleText->Des() );
	CleanupStack::PopAndDestroy(titleText);
	iBusy = EFalse;

    FLOG( "[OMADM] CNSmlDMSyncAppUi::ChangeViewL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ShowErrorNoteL( TInt aError ) const
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowErrorNoteL:" );

	TBuf<KNSmlMaxTextLength256> errMsg;	
	SetErrorMessageL( aError, errMsg );	
	CAknInformationNote* queryDialog = new (ELeave) CAknInformationNote;
	queryDialog->ExecuteLD( errMsg );
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowErrorNoteL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::SetErrorMessageL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::SetErrorMessageL( TInt aError, TDes& aMsg ) const
	{
    FLOG( "[OMADM] CNSmlDMSyncAppUi::SetErrorMessageL:" );
    CTextResolver* textResolver = CTextResolver::NewLC();
    
    aMsg.Append( textResolver->ResolveErrorString( aError,
	                                         CTextResolver::ECtxNoCtx ) );
	
	CleanupStack::PopAndDestroy( textResolver );

	HBufC* appTitle = iEikonEnv->AllocReadResourceLC( R_SML_APP_TITLE );
	aMsg.Insert( 0, appTitle->Des() );
	CleanupStack::PopAndDestroy( appTitle );
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::SetErrorMessageL() completed, aError = %d, aMsg ="),
        aError ) );
    FTRACE( FPrint(
        _L("[OMADM] \"%S\""),
        &aMsg ) );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HelpContextL
// -----------------------------------------------------------------------------
//

CArrayFix<TCoeHelpContext>* CNSmlDMSyncAppUi::HelpContextL() const
	{
	CArrayFixFlat<TCoeHelpContext>* array = 
	                            new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
	array->AppendL( TCoeHelpContext( KUidSmlSyncApp, KDM_HLP_MAIN_VIEW ) );
	array->AppendL( TCoeHelpContext( KUidSmlSyncApp, KFOTA_HLP_SETTINGS ) );
	array->AppendL( TCoeHelpContext( KUidSmlSyncApp, KDM_HLP_SERVERS_VIEW ) );
	array->AppendL( TCoeHelpContext( KUidSmlSyncApp, KDM_HLP_SETTINGS ) );
	return array;
	}


// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ShowResourceErrorNoteL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ShowResourceErrorNoteL(TInt aResource) const
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowResourceErrorNoteL:" );

	HBufC* errorText = iEikonEnv->AllocReadResourceLC( aResource );
	CAknInformationNote* queryDialog = new (ELeave) CAknInformationNote;
	queryDialog->ExecuteLD( errorText->Des() );
	CleanupStack::PopAndDestroy( errorText );

    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowResourceErrorNoteL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::FotaModel
// This function is included for header file compliance with the version that
// implements the FOTA feature. Should not be created in any case and
// therefore returns always NULL.
// -----------------------------------------------------------------------------
//
CNSmlDMFotaModel* CNSmlDMSyncAppUi::FotaModel() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::MarkFwUpdChangesStartL
// This function is included for header file compliance with the version that
// implements the FOTA feature. Does nothing here.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::MarkFwUpdChangesStartL()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::FwUpdStatesChangedL
// This function is included for header file compliance with the version that
// implements the FOTA feature. Returns always EFalse.
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppUi::FwUpdStatesChangedL()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppUi::ShowConfirmationNoteL( TInt aResource ) const
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::ShowConfirmationNoteL:" );

	HBufC* queryText = iEikonEnv->AllocReadResourceLC( aResource );
	CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog();

	TBool retValue = queryDialog->ExecuteLD( R_STARTQUERY_NOTE,
	                                         queryText->Des() );
	CleanupStack::PopAndDestroy( queryText );
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::ShowConfirmationNoteL() completed, retval = %d"),
        retValue ) );
	return retValue;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleCommandNewProfileL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::HandleCommandNewProfileL()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleCommandNewProfileL:" );
    
	if ( iSyncDocument->ProfileCount() != 0 )
		{
		HBufC* delQuery = StringLoader::LoadL( 
		    R_QTN_SML_APPUI_COPY,
		    iSyncDocument->ProfileItem()->iProfileName );
		CleanupStack::PushL( delQuery );
		
		CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog();
		iDialogUp = 1;
		
		if ( queryDialog->ExecuteLD( R_STARTQUERY_NOTE, delQuery->Des() ) )  
			{
			iDialogUp = 0;
			ShowEditProfileDialogL( ESmlCopyProfile );					
			}
		else
			{
			iDialogUp = 0;
			ShowEditProfileDialogL( ESmlNewProfile );	
			}
		CleanupStack::PopAndDestroy( delQuery );
		}
	else
		{
		iDialogUp = 0;
		ShowEditProfileDialogL( ESmlNewProfile );	
		}
	
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleCommandNewProfileL() completed" );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::HandleCommandEraseProfileL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::HandleCommandEraseProfileL()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleCommandEraseProfileL:" );
    
	HBufC* delQuery = StringLoader::LoadL(
	    R_SML_APPUI_NOTE_TEXT_DEL,
	    iSyncDocument->ProfileItem()->iProfileName );
	CleanupStack::PushL( delQuery );
	
	CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog( );
	iDialogUp = 1;

	if ( queryDialog->ExecuteLD( R_STARTQUERY_NOTE, delQuery->Des() ) ) 
		{
		TInt index = 0;
		TInt result = iSyncDocument->DeleteProfile( index );
		if ( result == KErrNone )
			{
            iProfilesView->RefreshL();
			}
		else
			{
			ShowErrorNoteL( result );
			}
		}
	iDialogUp = 0;
	iBusy = EFalse;
	CleanupStack::PopAndDestroy( delQuery );
    FLOG( "[OMADM] CNSmlDMSyncAppUi::HandleCommandEraseProfileL() completed" );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::Synchronise
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppUi::Synchronise()
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::Synchronise:" );
    return iSyncAppEngine->SyncRunning();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::SyncComplete
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::SyncComplete( TNSmlStatus aStatus )
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::SyncComplete()" );

    TRAP_IGNORE( iSyncDocument->RefreshProfileListL() );
    // The listbox should get at least an empty array in any case, so
    // the result checking is not needed.
    TRAP_IGNORE( iProfilesView->RefreshL() );

    TInt Value;
    TInt r=RProperty::Get(KUidSmlSyncApp,0,Value);    
    if ( iSyncDocument->IsServerAlertSync() && Value == 1) //for Server alert launch case    
        {
        FLOG(  "[OMADM] CNSmlDMSyncAppUi::SyncComplete: RunAppShutter" );
        CAknEnv::Static()->RunAppShutter();        
        }      

    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::CloseGs
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::CloseGs()
    {    
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task(taskList.FindApp( KUidSmlSyncApp ) );
    task.SetWgId( CEikonEnv::Static()->RootWin().Identifier() ); 
    task.SendToBackground();
    CAknEnv::Static()->RunAppShutter();	
    }


// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::ExitCallL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::ExitCallL( )
    {
    //Dummy Should not be used if fota disabled
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::CheckFotaDlL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::CheckFotaDlL()
	{
	//Dummy Should not be used if fota disabled
	}

void CNSmlDMSyncAppUi::CheckAMDlL()
  {
  	
  	FLOG( "CNSmlDMSyncAppUi::CheckAMDlL : Start" );
  	const TUid KAppMgmtServerUid = {0x1020781C};
	
            
            TInt silentoperation = 1;
        	RProperty::Get( KPSUidNSmlDMSyncApp,
                				    KServerAlertType,
                    				silentoperation);
	    
	 	
	 	const TUint KAMServerUIEnabled = 0x00001;
	 	TInt newValue;
		TInt err = RProperty::Get(KAppMgmtServerUid, KAMServerUIEnabled, newValue);
		
		if(!err && newValue && (silentoperation==KErrNotFound || silentoperation==1))
		{
	    FLOG( "CNSmlDMSyncAppUi::CheckAMDlL : Entered:" );
	    
	    if(silentoperation ==0)
	    FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :silentoperation ==0" );
	    else
	    FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :silentoperation not equal to 0" );
			
			TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    		TApaTask task = taskList.FindApp(KAppMgmtServerUid );
    
    		if(task.Exists())
    		{
    		  FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :task.Exists() : BringToForeground" );
        	task.BringToForeground();
        }      
     } 
  	
  }
// End of File
