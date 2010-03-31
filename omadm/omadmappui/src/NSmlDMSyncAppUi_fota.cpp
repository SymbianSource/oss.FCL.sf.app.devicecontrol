/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <apgwgnam.h>
#include <gsfwviewuids.h>
#include <aknnotewrappers.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <textresolver.h>
#include <DevManInternalCRKeys.h> //SAN support
#include <centralrepository.h>
#include <barsread.h>
#include <e32property.h>
#include <NSmlDMSync.rsg>
#include "nsmldmsyncinternalpskeys.h"
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncAppEngine.h"
#include "NSmlDMFotaModel.h"
#include "NSmlDMFotaView.h"
#include "NSmlDMProfilesView.h"
#include "NSmlDMSyncDocument.h"
#include "NSmlDMDlgProfileView.h"
#include "NSmlDMSyncDebug.h"
#include "NSmlDMSyncDialog.h"
#include "NSmlDMSyncProfile.h"
#include "NSmlDMSyncPrivateCRKeys.h"
#include <featmgr.h>

#include <hlplch.h>
#include <csxhelp/dm.hlp.hrh>

#include <aknmessagequerydialog.h>     // For CAknMessageQueryDialog

// For KCalenHoursInDay = 24 and other time-related constants.
#include <calenconstants.h>           

// Array granuality for displaying R_QTN_FOTA_NOTE_TIME_RESTRICTION note.
const TInt KNSmlDMFotaNoteArrayGranuality = 3;

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

    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);
    iSyncDocument = (CNSmlDMSyncDocument*) iDocument;
    iSyncAppEngine = iSyncDocument->AppEngine();
    iSyncAppEngine->RequestSyncStatus( this );
    iFotaModel = CNSmlDMFotaModel::NewL( iSyncDocument );
    iProfilesView = new (ELeave) CNSmlDMProfilesView(); 

    iViewTitle = 0;
    
    iProfilesView->ConstructL(); // this );
    AddViewL( iProfilesView ); // Transfer ownership to CAknViewAppUi

    TInt configFlags( 0 );
    CRepository* centrep = NULL;
    centrep = CRepository::NewL( KCRUidNSmlDMSyncApp );
    centrep->Get( KNSmlDMFotaConfigurationKey, configFlags );
    delete centrep;

    if ( configFlags & KNSmlDMFotaEnableFotaViewFlag )
        {
    	iNaviDecorator = 0;
        // Show tabs for main views from resources
        CEikStatusPane* sp = StatusPane();

        // Fetch pointer to the default navi pane control
        iNaviPane = ( CAknNavigationControlContainer* )
            sp->ControlL( TUid::Uid(EEikStatusPaneUidNavi) );

   
        CNSmlDMFotaView* fotaView = new (ELeave) CNSmlDMFotaView;
        iMainView = fotaView;
        CleanupStack::PushL( fotaView );
        fotaView->ConstructL();
        AddViewL( fotaView ); // Transfer ownership to CAknViewAppUi
        CleanupStack::Pop( fotaView );

        ActivateLocalViewL( fotaView->Id() );
        }
    else
        {
        ActivateLocalViewL( iProfilesView->Id() );
        }
    
 
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

    delete iFotaModel;
	delete iNaviDecorator;
	if ( iDestroyed == 0 )
		{
		delete iAppView;
		}
	}

// ------------------------------------------------------------------------------
// CNSmlDMSyncAppUi::TabChangedL
// ------------------------------------------------------------------------------
//

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
    FTRACE( FPrint(
            _L("[OMADM] CNSmlDMSyncAppUi::HandleKeyEventL: = 0x%x"),aKeyEvent.iCode ) );
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
			    {			    			    
			    FLOG( "End key pressed" );
			    if ( iMainView )
			        {			
			        ExitCallL();
			        }
			    }
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
      case   EAknCmdHelp:                           
			{
			     
           if (FeatureManager::FeatureSupported( KFeatureIdHelp ))
			   {
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppHelpContextL());
      		    	iBusy = EFalse;
      	 }		 
			
			}
    		break;

	    case EAknSoftkeyExit: //For independent .exe app launch from file browser
		case EAknCmdExit:
        case EEikCmdExit:		// quit application
			{
			TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	        TApaTask task1(taskList.FindApp( KFotaServerAppUid));
			TInt value=-1;
            TInt r1=RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,value);
	        if(task1.Exists())
	        	{
	        	task1.EndTask();
	        	}
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
			if (TUtil::SettingEnforcementState()|| iSyncDocument->ProfileItem()->iProfileLocked)
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
			if(TUtil::ShowNativeDialogL(EPrivacyPolicy)==KErrNone)
			    {
                if ( ShowConfirmationNoteL( R_QTN_SML_CONF_STARTQUERY ) )
                    {
                    StartSyncL();
                    }
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
    const TBool aUseFotaProgressNote ) // = EFalse
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

    // If Fota check limitation feature is enabled, users are allowed to check
    // updates only once within the limitation set in the CenRep
    // (24 hours by default).
    if ( FeatureManager::FeatureSupported( KFeatureIdFfFotaCheckLimitation ) 
        && !IsUpdateAllowedL( *profile ) )
        {
        FLOG( "[OMADM] CNSmlDMSyncAppUi::StartSyncL Checking update is limited.\
            Can not proceed sync." );
        iSyncAppEngine->CloseProfile();
        return;
        }

    iSyncAppEngine->CloseProfile();		
    TRAPD( error, iSyncAppEngine->SynchronizeL( serverName, 
                                                iConfedProfileId, 
                                                connectionBearer,
                                                aUseFotaProgressNote ) );
	
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
// -----------------------------------------------------------------------------
//
CNSmlDMFotaModel* CNSmlDMSyncAppUi::FotaModel() const
    {
    return iFotaModel;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::MarkFwUpdChangesStartL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::MarkFwUpdChangesStartL()
    {
    __ASSERT_ALWAYS( iFotaModel, TUtil::Panic( KErrGeneral ) );
    iFotaModel->MarkFwUpdChangesStartL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::FwUpdStatesChangedL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncAppUi::FwUpdStatesChangedL()
    {
    __ASSERT_ALWAYS( iFotaModel, TUtil::Panic( KErrGeneral ) );
    return iFotaModel->FwUpdStatesChangedL();
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
		//This if loop check required whether keypad or touch (via stylus) being used incase of single click enabled
	    if(iProfilesView->MenuBar()->ItemSpecificCommandsEnabled())
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
void CNSmlDMSyncAppUi::SyncComplete( TNSmlStatus /* aStatus */ )                                      
                                     
    {
    FLOG( "[OMADM] CNSmlDMSyncAppUi::SyncComplete()" );

    TRAP_IGNORE( iSyncDocument->RefreshProfileListL() );
    // The listbox should get at least an empty array in any case, so
    // the result checking is not needed.
    TRAP_IGNORE( iProfilesView->RefreshL() );
    RProperty::Set( KPSUidNSmlDMSyncApp,
                				    KServerAlertType,
                    				KErrNotFound );
    iSyncDocument->ResetSession();                				
    TRAP_IGNORE( iMainView->RefreshL(ETrue) );
    TInt Value;
    TInt r=RProperty::Get(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,Value);    
    if ( iSyncDocument->IsServerAlertSync() && Value == 1) //for Server alert launch case    
        {
        FLOG(  "[OMADM] CNSmlDMSyncAppUi::SyncComplete: RunAppShutter" );
      //  TRAP_IGNORE( ExitCallL() );
           	FLOG(  "CNSmlDMSyncAppUi::SyncComplete--SANSupport existed" );
           	//new for fota case SAN Support
           	TInt configFlags( EGenericSession  );
        		CRepository* centrep = NULL;
        		TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
        		if ( centrep )
            	{
            	if( err == KErrNone )
            	{
            	centrep->Get( KDevManSessionType, configFlags );            
            	centrep->Set( KDevManSessionType, EGenericSession  );
              }
              delete centrep;
            	}      
            // Check P&S Key set by Application Management. 
           	// If P&S Key is set then indicates that DM UI should not be closed. Application Mgmt
           	// Server will take care of closing DM UI
           	const TUid KMyPropertyCat = {0x1020781C};
			
			const TUint KDMUINotClosed = 0x00005;
			//enum TMyPropertyKeys {EMyPropertyInteger, EMyPropertyArray};
	 	
	 	
	 		TInt valuekey = 0;
			RProperty::Get(KMyPropertyCat, KDMUINotClosed, valuekey);
		      
           	
           //new for fota case SAN Support
           if( configFlags != EFotaPkgFound && !valuekey) //no update found
              {
            	FLOG(  "[OMADM] CNSmlDMSyncAppUi::SyncComplete: RunAppShutter::Sansupport" );            	
            	TRAP_IGNORE( ExitCallL() );          	
        	 	  CAknEnv::Static()->RunAppShutter();
              }              
           }
    if ( Value == 3 )	
        {
       	Value = 2;
       	RProperty::Set(KPSUidNSmlDMSyncApp,KNSmlDMSyncUiLaunchKey,Value);
        }      

    }
// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::RefreshMainViewL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::RefreshMainViewL( )
    {    
    iMainView->RefreshL(ETrue);	
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
    iMainView->UpdateExitL();	    
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncAppUi::CheckFotaDlL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncAppUi::CheckFotaDlL()
	{
	FLOG( "CNSmlDMSyncAppUi::CheckFotaDlL : Begin" );
	if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) )
		{
		//When Download is going on
	    TInt value = KErrNotFound, err = KErrNotFound;
	    err = RProperty::Get(KPSUidNSmlDMSyncApp,KFotaDLStatus,value);
	    FTRACE( FPrint( _L("[OMADM]\t CNSmlDMSyncAppUi::CheckFotaDl() \
	    KFotaDLStatus val = %d & err = %d"),
	    value,err ) );
	    if(!err && value == 1)
	        {
	        FLOG( "CNSmlDMSyncAppUi::CheckFotaDl Ongoing Fota operation\
	        identified." );

	        TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	        TApaTask task=taskList.FindApp(TUid::Uid(0x102072c4));
	        if(task.Exists())
	            {
	            FLOG( "CNSmlDMSyncAppUi::CheckFotaDl Switching to FS \
	            Download" );
	            task.BringToForeground();
	            }
	        else
	            FLOG( "CNSmlDMSyncAppUi::CheckFotaDl Inconsistency problem\
	            need to debug" );
	        }							

	    else
	        {
	        FLOG("CNSmlDMSyncAppUi::CheckFotaDl  No ongoing Fota\
	        operation identified!");
	        }
	    }
	FLOG( "CNSmlDMSyncAppUi::CheckFotaDlL : End" );
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
		{
	    	FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :silentoperation ==0" );
		}
	    	else
		{
	    	FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :silentoperation not equal to 0" );
		}	
			
		TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    		TApaTask task = taskList.FindApp(KAppMgmtServerUid );
    		const TInt id = task.WgId();
		task.SetWgId(id);
		
		FTRACE( FPrint( _L("[OMADM]\t CNSmlDMSyncAppUi::CheckAMDlL() \
	    	WGID val = %d"),
	    	id ) );
		
    		if(task.Exists())
    		{
    		  FLOG( "CNSmlDMSyncAppUi::CheckAMDlL :task.Exists() : BringToForeground" );
        	task.BringToForeground();
        	}      
     		} 
  	
  }

TBool CNSmlDMSyncAppUi::IsUpdateAllowedL( CNSmlDMSyncProfile& aProfile )
    {
    FLOG( "CNSmlDMSyncAppUi::IsUpdateAllowedL : Start" );

    // Get the last successful sync time. It's a UTC time.
    // Note: LastSuccessSync() returns the time when the last successful check
    // for Fota update.
    TTime utcLastSuccessSync = aProfile.LastSuccessSync();

    // If utcLastSuccessSync is 0, there is no log available. Continue
    // sync.
    // Note: CNSmlDMSyncProfile::LastSuccessSync() returns 0 not Time::NullTTime().
    if ( utcLastSuccessSync.Int64() == 0 )
        {
        FLOG( "[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() LastSuccessSync is 0." );
        return ETrue;
        }

#ifdef _DEBUG
    TDateTime dt = utcLastSuccessSync.DateTime();
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() utcLastSuccessSync \
        %04d-%02d-%02d %02d:%02d:%02d"),
        dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second() ) );
#endif

    // Read Fota check limitation value in hours from CenRep.
    // If failed to get the value, the default value is 24 hours.
    TInt fotaCheckLimitation( KCalenHoursInDay );
    CRepository* cenrep = CRepository::NewL( KCRUidNSmlDMSyncApp );
    cenrep->Get( KNsmlDmFotaCheckLimitation, fotaCheckLimitation );
    delete cenrep;
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() Fota check limitation = %d"),
        fotaCheckLimitation ) );

    // Check the get value.
    if ( fotaCheckLimitation <= 0 )
        {
        // Something wrong with the value.  Set it to the default value.
        fotaCheckLimitation = KCalenHoursInDay;
        }

    // Read the current time for comparison. Also universal time.
    TTime utcCurrentTime;
    utcCurrentTime.UniversalTime();

#ifdef _DEBUG
    dt = utcCurrentTime.DateTime();
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() utcCurrentTime \
        %04d-%02d-%02d %02d:%02d:%02d"),
        dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second() ) );
#endif

    // Calculate the remainig time for the next available update check.
    TTimeIntervalMinutes passedMinutes;
    utcCurrentTime.MinutesFrom( utcLastSuccessSync, passedMinutes );
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() passedMinutes.Int() = %d"), 
        passedMinutes.Int() ) );

    // If Fota check limit is passed or passedMinutes is invalid, return EFalse
    // to proceed sync process.
    if ( passedMinutes.Int() > fotaCheckLimitation * KCalenMinutesInHour
        || passedMinutes.Int() < 0 )
        {
        FLOG( "[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() Proceed sync process." );
        return ETrue;
        }

    // Calculate the next available update check time.
    TInt remainingMinutes ( ( fotaCheckLimitation * KCalenMinutesInHour ) 
        - passedMinutes.Int() );
    TInt remainingHours( remainingMinutes / KCalenMinutesInHour );
        remainingMinutes -= remainingHours * KCalenMinutesInHour;
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncAppUi::IsUpdateAllowedL() \
        remainingHours = %d, remainingMinutes = %d"), 
        remainingHours, remainingMinutes ) );
    __ASSERT_DEBUG( ( remainingHours >= 0 ) && ( remainingMinutes >= 0 ), 
        _L("CNSmlDMSyncAppUi::IsUpdateAllowedL - Problem with Remaining \
        time calculation.") );

    // If calculated remaining time is valid, show the sorry note with
    // remaining time until next available time for checking update then
    // return ETrue to stop sync process.
    if ( remainingHours > 0 || remainingMinutes > 0 )
        {
        CArrayFix< TInt >* timeArray = 
            new( ELeave ) CArrayFixFlat< TInt >( KNSmlDMFotaNoteArrayGranuality );
        CleanupStack::PushL( timeArray );

        timeArray->AppendL( fotaCheckLimitation );
        timeArray->AppendL( remainingHours );
        timeArray->AppendL( remainingMinutes );

        HBufC* stringHolder =
            StringLoader::LoadLC( R_QTN_FOTA_NOTE_TIME_RESTRICTION, 
            *timeArray, iEikonEnv );

        CAknMessageQueryDialog* infoDialog = 
            CAknMessageQueryDialog::NewL( *stringHolder );
 
        // infoDialog is added to the cleanup stack in PrepareLC().
        // And the dialog is popped and destroyed in RunLD().
        infoDialog->PrepareLC( R_AVKON_MESSAGE_QUERY_DIALOG );

        // Only OK button should be shown since this is just a note.
        infoDialog->ButtonGroupContainer().MakeCommandVisible( 
            EAknSoftkeyCancel, EFalse );

        // Return value of RunLD() is ignored.
        infoDialog->RunLD();
  
        CleanupStack::PopAndDestroy( stringHolder );
        CleanupStack::PopAndDestroy( timeArray );
 
        // Return EFalse to stop sync process.
        FLOG( "CNSmlDMSyncAppUi::IsUpdateAllowedL : End - EFalse" );
        return EFalse;
        }

    // No remaining time left (or invalid calculation).
    // Proceed checking update process.
    FLOG( "CNSmlDMSyncAppUi::IsUpdateAllowedL : End - ETrue" );
    return ETrue;
    }

// End of File
