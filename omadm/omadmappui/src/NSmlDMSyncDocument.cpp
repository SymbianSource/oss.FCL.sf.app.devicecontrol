/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for CNSmlDMSyncDocument
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <eikenv.h>
#include <centralrepository.h>
#include <SyncMLNotifierDomainCRKeys.h>

#include <e32const.h>
#include <DevManInternalCRKeys.h>
#include <e32property.h>
#include "nsmldmsyncinternalpskeys.h"
#include "NSmlDMSyncApp.h"
#include "NSmlDMSyncAppEngine.h"
#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncAppUi.h"
#include "NSmlDMSyncProfileList.h"
#include "NSmlDMSyncProfile.h"
#include "NSmlDMSyncDebug.h"
#include <NSmlDMSync.rsg>
#include <SyncService.h>
#include <SyncServiceParams.h>
#include <coreapplicationuisdomainpskeys.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// Desctructor
// -----------------------------------------------------------------------------
//
CNSmlDMSyncDocument::~CNSmlDMSyncDocument()
    {
    FLOG( "[OMADM] CNSmlDMSyncDocument::~CNSmlDMSyncDocument" );
    
    TInt err = RProperty::Delete(KPSUidNSmlDMSyncApp,KServerAlertType);
    FTRACE( FPrint(
            _L("CNSmlDMSyncDocument::~CNSmlDMSyncDocument KServerAlertType key deletion with error err = %d"),err ) );
    err = RProperty::Delete(KPSUidNSmlDMSyncApp, KFotaDLStatus);
    FTRACE( FPrint(
            _L("CNSmlDMSyncDocument::~CNSmlDMSyncDocumentKFotaDLStatus key deletion with error err = %d"),err ) );
   //IAD****
   
   err = RProperty::Delete(KPSUidNSmlDMSyncApp, KDMIdle);         
            
    err = RProperty::Delete(KPSUidNSmlDMSyncApp, KFotaDLRefresh);
    FTRACE( FPrint(
            _L("CNSmlDMSyncDocument::~CNSmlDMSyncDocumentKFotaDLRefresh key deletion with error err = %d"),err ) );
    err = RProperty::Delete(KPSUidNSmlDMSyncApp, KDmJobCancel);
    
    err = RProperty::Delete(KPSUidNSmlDMSyncApp, KSilentSession);
    
    FTRACE( FPrint(
            _L("CNSmlDMSyncDocument::~CNSmlDMSyncDocument KDmJobCancel key deletion with error err = %d"),err ) );
    delete iDbNotifier;
    delete iSyncProfileList;
	delete iAppEngine;
	if ( iProfileList )
	    {
	    iProfileList->Reset();
	    delete iProfileList;
	    }
    delete iServer;
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::ConstructL()
    {
    FLOG( "[OMADM] CNSmlDMSyncDocument::ConstructL" );
    
    iAppEngine = CNSmlDMSyncAppEngine::NewL( this );
    iSyncProfileList = CNSmlDMSyncProfileList::NewL( iAppEngine );
    iSilent = EFalse;
    iDbEventsBlocked = EFalse;
    iDbNotifier = CNSmlDMDbNotifier::NewL( iAppEngine->Session(), this );
    iDbNotifier->RequestL();

    iProfileList = new (ELeave) CArrayFixFlat< TNSmlDMProfileItem >(1);
    iCurrentIndex = 0;
    iEikEnv = CEikonEnv::Static();
    iOldProfileId = KErrNotFound;
    //SAN Spport
    TInt SanSupport( KErrNone );
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys) );    
    if( err == KErrNone )
       {
    centrep->Get( KDevManSANUIBitVariation, SanSupport );
       }
    delete centrep;
    iSanSupport = SanSupport;
    
        _LIT_SECURITY_POLICY_S0( KWritePolicy, KUidSmlSyncApp.iUid );
        _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
        _LIT_SECURITY_POLICY_C1( KWritePolicy1, ECapabilityWriteDeviceData );
        RProperty::Define( KPSUidNSmlDMSyncApp,
                           KServerAlertType,
                           RProperty::EInt,
                           KReadPolicy,
                           KWritePolicy );
    
     RProperty::Set( KPSUidNSmlDMSyncApp,
                				    KServerAlertType,
                    				KErrNotFound );   	
	err  = RProperty::Define( KPSUidNSmlDMSyncApp,
			KFotaDLRefresh,
			RProperty::EInt,KReadPolicy,KWritePolicy1); 
	TInt err1 = KErrNotFound;
	//if key defined then only set from DMUI, other wise fota sets it
	if( err == KErrNone ) 
		{
		iDMDefinesFotaDLKey = ETrue;
		err1 =      RProperty::Set( KPSUidNSmlDMSyncApp,
				KFotaDLRefresh,KErrNone );    		 
		}
	FTRACE( FPrint(
			_L("KFotaDLRefresh setting err = %d, err1 = %d"),err, err1 ) );
//IAD ******//
err  = RProperty::Define( KPSUidNSmlDMSyncApp,
			KDMIdle,
			RProperty::EInt); 
err1 =  RProperty::Set( KPSUidNSmlDMSyncApp,
			KDMIdle,KErrNone );
 if (err1)
 {
  FTRACE( FPrint(
			_L("error in setting = %d, err1 = %d"),err, err1 ) );			

 }			 
FTRACE( FPrint(
			_L("KDMIdle setting err = %d, err1 = %d"),err, err1 ) );			
			  

	err  = RProperty::Define( KPSUidNSmlDMSyncApp,
			KFotaDLStatus,
			RProperty::EInt,KReadPolicy,KWritePolicy1); 
	err1 =  RProperty::Set( KPSUidNSmlDMSyncApp,
			KFotaDLStatus,KErrNotFound );   

    FLOG( "[OMADM] CNSmlDMSyncDocument::ConstructL() completed" );
	FTRACE( FPrint(
			_L("KFotaDLStatus setting err = %d, err1 = %d"),err, err1 ) );
	err  = RProperty::Define( KPSUidNSmlDMSyncApp,
			KDmJobCancel,
			RProperty::EInt,KReadPolicy,KWritePolicy1); 
	err1 =  RProperty::Set( KPSUidNSmlDMSyncApp,
			KDmJobCancel, KErrNone );   
			
	// Define new P&S key to know whether these DM session started in Silent or
	// User Interactive mode
	
	RProperty::Define( KPSUidNSmlDMSyncApp,
                           KSilentSession,
                           RProperty::EInt);
	FTRACE( FPrint(
			_L("KDmJobCancel setting err = %d, err1 = %d"),err, err1 ) );	
    FLOG( "[OMADM] CNSmlDMSyncDocument::ConstructL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncDocument* CNSmlDMSyncDocument::NewL( CAknApplication& aApp )
    {
    FLOG( "[OMADM] CNSmlDMSyncDocument::NewL:" );

	CNSmlDMSyncDocument* self = new (ELeave) CNSmlDMSyncDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  //self

    FLOG( "[OMADM] CNSmlDMSyncDocument::NewL() completed" );
    return self;
	}
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::CreateAppUiL
// -----------------------------------------------------------------------------
//
CEikAppUi* CNSmlDMSyncDocument::CreateAppUiL()
    {
    FLOG( "[OMADM] CNSmlDMSyncDocument::CreateAppUiL:" );
    
	return new (ELeave) CNSmlDMSyncAppUi;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::DeleteProfile
// -----------------------------------------------------------------------------
//
TInt CNSmlDMSyncDocument::DeleteProfile( TInt& aIndex )
	{
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::DeleteProfile(), index = %d"),
        aIndex ) );
        
    TInt retVal = KErrNotFound;
	if ( iProfileList->Count() <= 0 )
		{
		return retVal;
		}
	TRAP( retVal, iAppEngine->DeleteProfileL( 
	                            (*iProfileList)[iCurrentIndex].iProfileId ) );
	if ( retVal == KErrNone )
		{
		TRAP( retVal, ReadProfileListL() );
    	if ( retVal == KErrNone )
    		{		
    		if ( iCurrentIndex > iProfileList->Count() - 1 )
    			{
    			iCurrentIndex--;
    			}
    		aIndex = iCurrentIndex;
    		}
		}

    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::DeleteProfile() completed, retval = %d, index = %d"),
        retVal, iCurrentIndex ) );
	return retVal;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::ReadProfileListL()
	{
	ReadProfileListL( EFalse );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ReadProfileListL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::ReadProfileListL( TBool aIncludeHidden )
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::ReadProfileListL:" );

	iProfileList->Reset();

	TKeyArrayFix key(_FOFF( TNSmlDMProfileItem, iProfileName ), ECmpFolded16 );
	iProfileList->Sort(key);

    iSyncProfileList->Reset();
	
	TRAPD( error, iSyncProfileList->ReadProfileItemsL( aIncludeHidden ) );
	if ( error != KErrNone )
	    {
	    iSyncProfileList->Reset();
	    }
	
	//read profile values
	for ( TInt index = 0; index < iSyncProfileList->Count(); index++ )
		{
	    TNSmlDMProfileItem profile;
		profile.iProfileName = iSyncProfileList->Item( index ).iProfileName;
		if ( profile.iProfileName.Length() == 0 )
			{
			StringLoader::Load( profile.iProfileName, R_QTN_APPS_EMPTYPROFILE );
			}		
    	profile.iProfileId   = iSyncProfileList->Item( index ).iProfileId;
		profile.iBearer		 = iSyncProfileList->Item( index ).iBearer;
		profile.iSynced      = iSyncProfileList->Item( index ).iSynced;
		profile.iLastSync	 = iSyncProfileList->Item( index ).iLastSync;
		profile.iLastSuccessSync	 = iSyncProfileList->Item( index ).iLastSuccessSync;
		profile.iActive		 = iSyncProfileList->Item( index ).iActive;
        profile.iDeleteAllowed =  iSyncProfileList->Item( index ).iDeleteAllowed;
        profile.iProfileLocked =  iSyncProfileList->Item( index ).iProfileLocked;
        
		TInt start = 0;
		TBool found( EFalse );
		
		if ( iProfileList->Count() == 0 )
			{
			iProfileList->AppendL ( profile );		
			}
		else
			{
			while ( ( start < iProfileList->Count() ) && ( !found ) )
				{
				if ( iAppEngine->Compare( profile.iProfileName,
				     iProfileList->At( start ).iProfileName ) < 1 )
					{
					iProfileList->InsertL( start, profile );
					found = ETrue;
					}
				start++;
				}
			if ( !found ) //insert to the end of the list
				{
				iProfileList->AppendL( profile );
				}
			}
		}
	FLOG( "[OMADM] CNSmlDMSyncDocument::ReadProfileListL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ProfileItem
// -----------------------------------------------------------------------------
//
TNSmlDMProfileItem* CNSmlDMSyncDocument::ProfileItem() const
	{
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::ProfileItem() iProfileList = 0x%x"),
        iProfileList ) );
	
	if ( iProfileList->Count() > 0 )
		{
		return &( *iProfileList )[ iCurrentIndex ];
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ProfileItemAt
// -----------------------------------------------------------------------------
//
TNSmlDMProfileItem* CNSmlDMSyncDocument::ProfileItemAt( TInt aIndex ) const
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::ProfileItemAt:" );
	
	if ( ( iProfileList->Count() > 0 ) &&
	   ( ( aIndex >= 0 ) &&
	   ( aIndex < iProfileList->Count() ) ) )
		{
		return &( *iProfileList )[aIndex];
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::FotaModel
// -----------------------------------------------------------------------------
//
CNSmlDMFotaModel* CNSmlDMSyncDocument::FotaModel() const
    {
    return ((CNSmlDMSyncAppUi*) iAppUi)->FotaModel();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::MarkFwUpdChangesStartL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::MarkFwUpdChangesStartL()
    {
    ((CNSmlDMSyncAppUi*) iAppUi)->MarkFwUpdChangesStartL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::FwUpdStatesChangedL
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncDocument::FwUpdStatesChangedL()
    {
    return ((CNSmlDMSyncAppUi*) iAppUi)->FwUpdStatesChangedL();
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::ProfileList
// -----------------------------------------------------------------------------
//
CArrayFixFlat< TNSmlDMProfileItem >* CNSmlDMSyncDocument::ProfileList( 
                                                                  TInt& aIndex )
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::ProfileList:" );
	
	if ( iCurrentIndex < 0 )
		{
		iCurrentIndex = 0;
		}
	aIndex = iCurrentIndex; 

	FLOG( "[OMADM] CNSmlDMSyncDocument::ProfileList() completed" );
	return iProfileList; 
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::SetCurrentIndex
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::SetCurrentIndex( TInt aIndex ) 
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::SetCurrentIndex:" );
	
	if ( ( aIndex > -1 ) && ( aIndex < iProfileList->Count() ) )
		{
		iCurrentIndex = aIndex; 
		}
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::SetCurrentIndex() completed, index = %d"),
        iCurrentIndex ) );
	}

// -----------------------------------------------------------------------------
//  CNSmlDMSyncDocument::MoveIndexToProfile
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::MoveIndexToProfile( TInt aProfileId )
	{
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::MoveIndexToProfile(), aProfileId = %d"),
        aProfileId ) );
	
	TInt index = 0;
	TRAP_IGNORE( ReadProfileListL() );//new fix for the error  ELGO-6ZK8BD
	TInt profileCount = iProfileList->Count();
	
	while ( ( index < profileCount ) &&
	      ( (*iProfileList)[index].iProfileId != aProfileId ) )
		{
		index++;
		}
	if ( index < profileCount )
		{
		SetCurrentIndex( index );
		}
	else //Fix for SMAN-73XGBM
		{
		SetCurrentIndex( 0 );	
		}
    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::MoveIndexToProfile() completed, index = %d"),
        index ) );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::GetCopyProfileL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::GetCopyProfileL( TInt& aProfileId )
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::GetCopyProfileL:" );
	
	if ( iProfileList->Count() <= 0 )
		{
		return;
		}
	// save the original profileid, if it's not saved it can be deleted
	iOldProfileId = ( *iProfileList )[iCurrentIndex].iProfileId;

	CNSmlDMSyncProfile* profile = 
	  iAppEngine->CreateCopyProfileL( (*iProfileList )[iCurrentIndex].iProfileId );
    
    profile->SetNameL( GetNextProfileNameLC()->Des() );
	aProfileId = profile->ProfileId();
	profile->SaveL();
	iAppEngine->CloseProfile();
	
	CleanupStack::PopAndDestroy(); //name

	FLOG( "[OMADM] CNSmlDMSyncDocument::GetCopyProfileL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::GetNextProfileNameLC
// -----------------------------------------------------------------------------
HBufC* CNSmlDMSyncDocument::GetNextProfileNameLC()
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::GetNextProfileNameLC:" );
	
	TBool newNameFound = EFalse;
	HBufC* profileName = NULL;
	TInt nameIndex = 1;

	while ( !newNameFound )
		{
		profileName = StringLoader::LoadLC( R_QTN_APPS_NEWPROFILE, nameIndex );
		TNSmlDMProfileItem profileItem;
		profileItem.iProfileName = *profileName;
		TKeyArrayFix key(_FOFF(TNSmlDMProfileItem, iProfileName), ECmpFolded16 );

		TInt foundIndex;
		if ( iProfileList->Find( profileItem, key, foundIndex ) )
			{
			newNameFound = ETrue;
			}
		else
			{
			nameIndex++;
			CleanupStack::PopAndDestroy( profileName );
			}
		}

    FTRACE( FPrint(
        _L("[OMADM] CNSmlDMSyncDocument::GetNextProfileNameLC() completed, name = \"%S\""),
        profileName ) );
	return profileName;
	}

// -----------------------------------------------------------------------------
// DisableDbNotifications
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::DisableDbNotifications( TBool aEvent )
    {
    iDbEventsBlocked = aEvent;
    }

// -----------------------------------------------------------------------------
// HandleDbEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::HandleDbEventL( TNSmlDMDbEvent aEvent )
	{
	FLOG( "[OMADM] CNSmlDMSyncDocument::HandleDbEventL:" );
	
	if( !iDbEventsBlocked )
	    {
        switch ( aEvent.iType )
    		{
    		case CNSmlDMDbNotifier::EClose:
    			iAppEngine->SyncCompleted( ENSmlRefreshMainView );
    			break;
    		
    		case CNSmlDMDbNotifier::EUpdate:
                iSyncProfileList->ReadProfileItemL( aEvent.iProfileId );
                iAppEngine->SyncCompleted( ENSmlRefreshMainView );
    			break;

    		case CNSmlDMDbNotifier::EDelete:
                iSyncProfileList->Remove( aEvent.iProfileId );
                iAppEngine->SyncCompleted( ENSmlRefreshMainView );
    			break;

    		case CNSmlDMDbNotifier::EUpdateAll:
                iSyncProfileList->ReadProfileItemsL();
                iAppEngine->SyncCompleted( ENSmlRefreshMainView );
    			break;

    		default:
    			break;
    		}
	    }
	FLOG( "[OMADM] CNSmlDMSyncDocument::HandleDbEventL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::RestoreL
//
// Called (by the environment) when app is opened embedded.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncDocument::RestoreL( const CStreamStore& aStore,
                                    const CStreamDictionary& aStreamDic )
    {
	FLOG( "[OMADM] CNSmlDMSyncDocument::RestoreL:" );
	 
    TSyncParameters params;
    RStoreReadStream stream;
    stream.OpenLC( aStore, aStreamDic.At( KUidSyncParameterValue ) );
    params.InternalizeL( stream );
    CleanupStack::PopAndDestroy();  // stream
    switch ( params.iCommand )
        {
        case KSyncServCmdStartSync:
            {
            FLOG( "[OMADM] CNSmlDMSyncDocument::KSyncServCmdStartSync" );
            static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
   	        static _LIT_SECURITY_POLICY_C1(KAllowWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
            TInt value = 1;
            TInt r=RProperty::Define(KUidSmlSyncApp,KNSmlDMSyncUiLaunchKey,RProperty::EInt,KAllowAllPolicy,KAllowWriteDeviceDataPolicy);
	        if ( r!=KErrNone && r!=KErrAlreadyExists )
              {
    	       User::LeaveIfError(r);
              }
            if( r==KErrAlreadyExists || r==KErrNone )
             {
              TInt Value;
              TInt r=RProperty::Get(KUidSmlSyncApp,KNSmlDMSyncUiLaunchKey,Value);
            if( Value == 2 ) //DM UI launched from CP
             {
             FLOG( "[OMADM] CNSmlDMSyncDocument::RestoreL P& S value = 3:" );
             value = 3;	//DM UI launched from CP and now from server alert
             }
            else
             {FLOG( "[OMADM] CNSmlDMSyncDocument::RestoreL P& S value = 1:" );
             value = 1;	//DM UI launched as server alert             
             }              
            } 
            TInt r1=RProperty::Set(KUidSmlSyncApp,KNSmlDMSyncUiLaunchKey,value);   	  
            
	    	AppEngine()->OpenProfileL( params.iProfileId );
            TInt bearer = AppEngine()->Profile()->BearerType();
            TBuf<KNSmlMaxProfileNameLength> serverName;
            AppEngine()->Profile()->GetName( serverName );
            
            // Server alerted sync should be checked to prevent the sending
            // the application to background in case the user has opened it.
           if ( iSanSupport == EOn )
           {
           	RProperty::Set( KPSUidNSmlDMSyncApp,
                				    KServerAlertType,
                    				params.iSilent );            
            if ( params.iSilent == 1 )                 
            	{
                iSilent = ETrue;  // For already DM app open case also
            	}             				            
            if ( IsServerAlertSync()
                && params.iSilent == 1 
                 )
            	{
                TApaTaskList taskList( iEikEnv->WsSession() );
                TApaTask task(taskList.FindApp( KUidSmlSyncApp ) );
                task.SetWgId( iEikEnv->RootWin().Identifier() ); 
                task.SendToBackground();
            	}
           }
		else if ( IsServerAlertSync())
			{
			if(AppEngine()->Profile()->SASyncState() == ESASyncStateEnable )
                {
                	//new fix for DD and DL not shown for Accepted "yes"
                RProperty::Set( KPSUidNSmlDMSyncApp,
						KServerAlertType, 1 );	
                TApaTaskList taskList( iEikEnv->WsSession() );
                TApaTask task(taskList.FindApp( KUidSmlSyncApp ) );
                task.SetWgId( iEikEnv->RootWin().Identifier() ); 
                task.SendToBackground();
                }
			else
				{
				RProperty::Set( KPSUidNSmlDMSyncApp,
						KServerAlertType, 0 );	
				}
			}
		    AppEngine()->CloseProfile();

            // Simpler FOTA progress note is not used in server initiated
            // management sessions and EFalse can be given as parameter.
        	AppEngine()->SynchronizeL( serverName, 
        	                           params.iProfileId,
        	                           params.iJobId,
        	                           bearer,
        	                           EFalse );
        	break;
            }
        case KSyncServCmdShowProgress:
            {
            FLOG( "[OMADM] CNSmlDMSyncDocument::KSyncServCmdShowProgress" );
            if ( iSanSupport == EOn )
              {
           	  RProperty::Set( KPSUidNSmlDMSyncApp,
                				    KServerAlertType,
                    				params.iSilent );            
              if ( params.iSilent == 1 )                 
            	{
                iSilent = ETrue;  
            	}
              }
                         
            if ( params.iJobId == KSyncServEnabled)
                {                
                AppEngine()->ShowProgressDialogL( ETrue );
                }
            else
                {                
                AppEngine()->ShowProgressDialogL( EFalse );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    FLOG( "[OMADM] CNSmlDMSyncDocument::RestoreL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::IsServerAlertSync
// -----------------------------------------------------------------------------
//		
TBool CNSmlDMSyncDocument::IsServerAlertSync()
    {
    FLOG( "CNSmlDMSyncDocument::IsServerAlertSync" );
    
    return iEikEnv->StartedAsServerApp();
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncDocument::StartServerL
// -----------------------------------------------------------------------------
//		
void CNSmlDMSyncDocument::StartServerL()
    {
    FLOG( "CNSmlDMSyncDocument::StartServerL" );
   
	CApaApplication* app = Application();
	app->NewAppServerL( iServer ); 
	TBuf<KMaxFullName> DmAppName;
    CRepository* centrep = NULL;
    centrep = CRepository::NewL( KCRUidNSmlNotifierDomainKeys );
    centrep->Get( KNSmlDMAppName, DmAppName );
    delete centrep;
    iServer->ConstructL( DmAppName );
	
	FLOG( "CNSmlDMSyncDocument::StartServerL Done" );
    }

// -----------------------------------------------------------------------------    
// CNSmlDMSyncDocument::SANSupport()
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncDocument::SANSupport()
    {
    FLOG( "CNSmlDMSyncDocument::SANSupport" );   		
    if ( iSanSupport == EOn )
        {
        	return ETrue;
        }	
    FLOG( "CNSmlDMSyncDocument::SANSupportDone" );    
    return EFalse;    
	  
    }
    
// -----------------------------------------------------------------------------    
// CNSmlDMSyncDocument::SilentSession()
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncDocument::SilentSession()
    {
    FLOG( "CNSmlDMSyncDocument::SilentSession" );   		
    TBool silent = ETrue;
    TInt silentenabled = 0;
    TInt silentdisabled = 1; 
    if(iSilent == silent)
    {
    
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KSilentSession,
                    silentenabled );     
    } 
    else
    {
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KSilentSession,
                    silentdisabled ); 	
    }	

    // If trust is establish then default make application management silent

    TInt tarmtrustenabled = KErrNone;
    RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsTarmIndicator, tarmtrustenabled);


    if(tarmtrustenabled == ECoreAppUIsTarmTerminalSecurityOnIndicatorOn || 
       tarmtrustenabled ==ECoreAppUIsTarmMngActiveIndicatorOn )
    {

	 RProperty::Set( KPSUidNSmlDMSyncApp,
                    KSilentSession,
                    silentenabled ); 

    }
	
    return iSilent;    
    }    
    
// -----------------------------------------------------------------------------    
// CNSmlDMSyncDocument::ResetSession()
// -----------------------------------------------------------------------------
//   
void CNSmlDMSyncDocument::ResetSession()
	{
	FLOG( "CNSmlDMSyncDocument::ResetSession" );   		
    iSilent = EFalse;
    FLOG( "CNSmlDMSyncDocument::ResetSession" );        	
	}
	
// -----------------------------------------------------------------------------    
// CNSmlDMSyncDocument::CurrentIndex()
// -----------------------------------------------------------------------------
//	
TInt CNSmlDMSyncDocument::CurrentIndex()
  {
  return iCurrentIndex;	
  }	

// -----------------------------------------------------------------------------    
// CNSmlDMSyncDocument::DMDefinesFotaDLKey()
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncDocument::DMDefinesFotaDLKey()
	{
	return iDMDefinesFotaDLKey;
	}
// End of File
