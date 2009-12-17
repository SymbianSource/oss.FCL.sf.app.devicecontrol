/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of DM UI component
* 	This is part of omadmappui.
*
*/


// INCLUDES
#include <NSmlDMSync.rsg>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <textresolver.h>   // for CTextResolver
#include <StringLoader.h>
#include <e32property.h>

#include <data_caging_path_literals.hrh>  // for resource and bitmap directories
#include <SyncMLErr.h>      // sync error codes


#include <AknsUtils.h>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>

#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncHandler.h"
#include "NSmlDMAuthQueryDlg.h"
#include "NSmlDMSyncState.h"
#include "NSmlDMSyncDebug.h"
#include "NSmlDMSyncDialog.h"
#include "nsmldmsyncinternalpskeys.h"

#include <nsmldmsync.mbg>
#include <featmgr.h>
#ifdef __SCALABLE_ICONS
_LIT( KBitmapFileName,  "z:nsmldmsync.mif" );
#else
_LIT( KBitmapFileName,  "z:nsmldmsync.mbm" );
#endif

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler* CNSmlDMSyncHandler::NewL( RSyncMLSession* aSyncSession,
                                              CNSmlDMSyncAppEngine* aAppEngine,
                                              CNSmlDMSyncDocument* aSyncDocument )
    {
	FLOG( "CNSmlDMSyncHandler::NewL()" );

    CNSmlDMSyncHandler* self =
        new (ELeave) CNSmlDMSyncHandler( aSyncSession,
                                         aAppEngine,
                                         aSyncDocument );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler::~CNSmlDMSyncHandler()
    {
    FLOG( "CNSmlDMSyncHandler::~CNSmlDMSyncHandler()" );

    
    if ( iFotaProgressDialog )
		{
	    TRAP_IGNORE( iFotaProgressDialog->CancelProgressDialogL() );
	    delete iFotaProgressDialog;
		}		            


	if ( iProgressDialog )
		{
	    TRAP_IGNORE( iProgressDialog->CancelProgressDialogL() );
	    delete iProgressDialog;
		}
	delete iState;
	delete iActiveCaller;

	if ( iSyncRunning )
	    {
	    TRAP_IGNORE( iSyncJob.StopL() );
        iSyncJob.Close();
	    }
	  FeatureManager::UnInitializeLib();
	FLOG( "CNSmlDMSyncHandler::~CNSmlDMSyncHandler() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::ConstructL( void )
    {
    FLOG( "CNSmlDMSyncHandler::ConstructL" );
   FeatureManager::InitializeLibL();   
	iState = CNSmlDMSyncState::NewL();
	iActiveCaller = CNSmlDMActiveCaller::NewL(this);
	iServerAlertedSync = EFalse;
	iSyncRunning = EFalse;
	iSyncError = KErrNone;
	
	iCheckUpdate = EFalse;
	
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::CNSmlDMSyncHandler
// -----------------------------------------------------------------------------
//
CNSmlDMSyncHandler::CNSmlDMSyncHandler( RSyncMLSession* aSyncSession,
                                        CNSmlDMSyncAppEngine* aAppEngine,
                                        CNSmlDMSyncDocument* aSyncDocument ) 
    : iSyncSession( aSyncSession ),
    iSyncAppEngine( aAppEngine ),
    iSyncDocument( aSyncDocument ),iFotaProgressLaunched(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HandleDialogEventL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HandleDialogEventL( TInt aButtonId )
    {
    FLOG( "CNSmlDMSyncHandler::HandleDialogEventL" );
    
	if ( aButtonId == EEikBidCancel )
		{
		FLOG( "CNSmlDMSyncHandler::HandleDialogEventL cancelling" );
		if ( SyncRunning() )
			{
	        TInt err = RProperty::Set( KPSUidNSmlDMSyncApp,
				KDmJobCancel, KErrCancel );
	        FTRACE( FPrint(
	              _L("CNSmlDMSyncHandler::CancelSynchronizeL() KDmJobCancel = -3, err = %d"),
	                			err ) );
			FLOG( "CNSmlDMSyncHandler::HandleDialogEventL cancelling stop job" );
			TRAP_IGNORE( iSyncJob.StopL() );
		    
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseCanceling );
	        UpdateDialogL();
			}
		}
    FLOG( "CNSmlDMSyncHandler::HandleDialogEventL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aJobId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
	{
	FLOG( "CNSmlDMSyncHandler::SynchronizeL Server alert" );
	
	iServerName = aServerName;
	iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iJobId = aJobId;
    iUseFotaProgressNote = aUseFotaProgressNote;
    
    iSyncJob.OpenL( Session(), iJobId );
    iSyncJobId = iSyncJob.Identifier();
    FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizeL--server alert, iSyncJobId = %d" ),
	     iSyncJobId ) ); 
	  //SAN Support
	  iServerAlertedSync = ETrue;  
	  _LIT_SECURITY_POLICY_S0( KWritePolicy, KUidSmlSyncApp.iUid );
        _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
        RProperty::Define( KPSUidNSmlDMSyncApp,
                           KNSmlDMSyncUiInitiatedJobKey,
                           RProperty::EInt,
                           KReadPolicy,
                           KWritePolicy ); 
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KNSmlDMSyncUiInitiatedJobKey,
                    iSyncJobId );                       
	  //SAN Support
    SynchronizeL();
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL( TDesC& aServerName,
                                       const TInt aProfileId,
                                       const TInt aConnectionBearer,
                                       const TBool aUseFotaProgressNote )
    {
    FLOG( "CNSmlDMSyncHandler::SynchronizeL" );

    iServerName = aServerName;
    iConnectionBearer = aConnectionBearer;
    iProfileId = aProfileId;
    iUseFotaProgressNote = aUseFotaProgressNote;

    iSyncJob.CreateL( Session(), iProfileId );
    iSyncJobId = iSyncJob.Identifier();
    SynchronizeL();
    
    TInt dummyValue;
    TInt err = RProperty::Get( KPSUidNSmlDMSyncApp,
                               KNSmlDMSyncUiInitiatedJobKey,
                               dummyValue );
    if ( err == KErrNotFound )
        {
        _LIT_SECURITY_POLICY_S0( KWritePolicy, KUidSmlSyncApp.iUid );
        _LIT_SECURITY_POLICY_C1( KReadPolicy, ECapabilityReadDeviceData );
        RProperty::Define( KPSUidNSmlDMSyncApp,
                           KNSmlDMSyncUiInitiatedJobKey,
                           RProperty::EInt,
                           KReadPolicy,
                           KWritePolicy );
        }
    FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizeL--UI initiated, iSyncJobId = %d" ),
	     iSyncJobId ) );    
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KNSmlDMSyncUiInitiatedJobKey,
                    iSyncJobId );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeL()
	{
	FLOG( "CNSmlDMSyncHandler::SynchronizeL" );
	
	TInt err = KErrNone;
	
	// for MSyncMLEventObserver events
	TRAP( err, Session().RequestEventL(*this) );
	if ( err != KErrNone )
		{
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave(err);
		}
    
    // for MSyncMLProgressObserver events
    TRAP( err, Session().RequestProgressL(*this) );
	if ( err != KErrNone )
		{
		Session().CancelEvent();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}
	
	State()->SetSyncPhase( CNSmlDMSyncState::EPhaseConnecting );
	TInt IsDMBusy = EFalse;   //IAD: Making DM Idle
	TInt r2=RProperty::Get(KUidSmlSyncApp,KDMIdle,IsDMBusy);
	if (IsDMBusy)
	   iNotinFotaView = 1;    //Not in fota view 
	else
	   iNotinFotaView = 0;    //already busy because in other view
	IsDMBusy =ETrue; 
  r2=RProperty::Set(KUidSmlSyncApp,KDMIdle,IsDMBusy);	   
	
	TRAP( err, ShowProgressDialogL() );
	if ( err != KErrNone )
		{
		Session().CancelEvent();
		Session().CancelProgress();
		iSyncJob.StopL();
		iSyncJob.Close();
		User::Leave( err );
		}
	FLOG( "CNSmlDMSyncHandler::SynchronizeL Sync is running" );

    iSyncDocument->MarkFwUpdChangesStartL();
	
	iSyncRunning = ETrue;
	iSyncError = KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SynchronizeCompletedL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SynchronizeCompletedL( TInt aError )
	{
    FTRACE( FPrint(
        _L("[OMADM]\t CNSmlDMSyncHandler::SynchronizeCompletedL(), aError = %d"),
        aError ) );
	
	if ( !SyncRunning() )
		{
		FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL already completed" );
		return;  // sync has already completed
		}
		
    iSyncJob.Close();
    
     FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::SynchronizecompletedL, iSyncJobId = %d" ),
	     KNSmlDMNoUserInitiatedJob ) );
    // Error code can be ignored.
    RProperty::Set( KPSUidNSmlDMSyncApp,
                    KNSmlDMSyncUiInitiatedJobKey,
                    KNSmlDMNoUserInitiatedJob );
	
	iSyncRunning = EFalse;
	iSyncError = aError;

    
    iCheckUpdate = EFalse;
    
    
	// close progress dialog
	if ( iProgressDialog )
        {
        FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL close dialog" ); 
        iProgressDialog->CancelProgressDialogL();
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    
    if (  iFotaProgressDialog )
        {
        __ASSERT_ALWAYS( iSyncDocument, TUtil::Panic( KErrGeneral ) );
        
        iFotaProgressDialog->CancelProgressDialogL();
        TInt configFlags( EGenericSession );
        //TInt SetGenValue(0);
        CRepository* centrep = NULL;
        TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
        if ( centrep )
            {
            if( err == KErrNone )
             {
            centrep->Get( KDevManSessionType, configFlags );
             }
            delete centrep;
            } 
        /*if ( err != KErrNone ) 
            {
            User::Leave( err );
            }
        else
            {
            
            centrep->Set( KDevManSessionType, SetGenValue );
            }      
        if(centrep)    
           {
           delete centrep;
           }*/
        delete iFotaProgressDialog;
        iFotaProgressDialog = NULL;
        // configFlags=2 for FOTA Package not downloaded case ,1- successful download
        if ( !iSyncDocument->FwUpdStatesChangedL() && (aError == KErrNone) 
        				&& configFlags == EFotaPkgNotFound )
            {
            FLOG( "[OMADM]\t CNSmlDMSyncHandler::SynchronizeCompletedL(): No changes in FW Update states." );
            
            HBufC* stringHolder =
                StringLoader::LoadLC( R_QTN_FOTA_NO_NEW_UPDATES );

            CAknInformationNote* infoNote =
                new (ELeave) CAknInformationNote;

            infoNote->SetTimeout( CAknNoteDialog::ELongTimeout );
            
            // We are not interested in the result.
            infoNote->ExecuteLD( *stringHolder );
            CleanupStack::PopAndDestroy( stringHolder );
            }
        }  
      

	iUseFotaProgressNote = EFalse;
    iSyncJob.Close();
    
	// handle error in HandleActiveCallL (when active caller completes)
	iActiveCaller->Start( aError );

	FLOG( "CNSmlDMSyncHandler::SynchronizeCompletedL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSessionEvent
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSessionEvent( TEvent aEvent,
                                               TInt aIdentifier,
                                               TInt aError,
                                               TInt /*aAdditionalData*/ )
	{
	FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), aEvent = %d, aIdentifier = %d, aError = %d" ),
	    aEvent, aIdentifier, aError ) );
	

	if ( aEvent == EJobStop
	    || aEvent == EJobStartFailed
	    || aEvent == EJobRejected )
		{
    	FTRACE( FPrint( _L(
    	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent(), iSyncJobId = %d" ),
    	    iSyncJobId ) );
    	    
    	 
    	TInt value =EFalse; 
    	TInt r=RProperty::Get( KPSUidNSmlDMSyncApp, KFotaDLStatus, value );	  
    	FTRACE( FPrint(
					_L("KFotaDLStatus = %d"),r ) );  
					
    	if(!iNotinFotaView && value != 1 )    
        r =  RProperty::Set( KPSUidNSmlDMSyncApp,
				 			KDMIdle, KErrNone ); 
									
							
							
							
							
		if ( iSyncJobId == aIdentifier )
			{
			FLOG( "CNSmlDMSyncHandler::OnSyncMLSessionEvent EJobStop close" );
			//For holding the DM Job session successful / error for future purpose
			TInt err =  RProperty::Set( KPSUidNSmlDMSyncApp,
							KDmJobCancel, KErrNone );   
			FTRACE( FPrint(
					_L("KDmJobCancel setting in OnSyncMLSessionEvent err = %d"),err ) );
            TRAP_IGNORE( SynchronizeCompletedL( aError ) );
			}
		}
    FLOG( "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSessionEvent() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSyncError
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSyncError( TErrorLevel aErrorLevel,
                                            TInt aError,
                                            TInt /*aTaskId*/,
                                            TInt /*aInfo1*/,
                                            TInt /*aInfo2*/)
	{
	FTRACE( FPrint( _L(
	    "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSyncError(), aErrorLevel = %d, aError = %d"),
	    aErrorLevel, aError ) );
	
	if ( aErrorLevel == ESmlFatalError )
		{
        TRAP_IGNORE( SynchronizeCompletedL( aError ) );
		}
    FLOG( "[OMADM]\tCNSmlDMSyncHandler::OnSyncMLSyncError() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLSyncProgress
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLSyncProgress( TStatus aStatus,
                                               TInt /*aInfo1*/,
                                               TInt /*aInfo2*/ )
	{
	FTRACE( RDebug::Print(
	          _L("CNSmlDMSyncHandler::OnSyncMLSyncProgress (%d)"), aStatus ) );
	
	switch (aStatus)
		{
		case ESmlConnecting:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseConnecting );
		    break;
		case ESmlConnected:
		    break;
		case ESmlLoggingOn:
		    break;
		case ESmlLoggedOn:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseSynchronizing );
		    break;
		case ESmlDisconnected:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseDisconnecting );
		    break;
		case ESmlCompleted:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseDisconnecting );
		    break;
		case ESmlProcessingServerCommands:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseSending );
		    break;
		case ESmlReceivingServerCommands:
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseReceiving );
		    break;
		
		case ESmlSendingModificationsToServer:
		    {
		    if( iFotaProgressDialog && iCheckUpdate == EFalse )
		    {
		    State()->SetSyncPhase( CNSmlDMSyncState::EPhaseFinal );	
		    }
		    
		    break;
		    }
		
		default:
		    break;
		}
    TRAP_IGNORE( UpdateDialogL() );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::OnSyncMLDataSyncModifications
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::OnSyncMLDataSyncModifications( TInt /*aTaskId*/,
                   const TSyncMLDataSyncModifications& /*aClientModifications*/,
                   const TSyncMLDataSyncModifications& /*aServerModifications*/ )
    {
    FLOG( "CNSmlDMSyncHandler::OnSyncMLDataSyncModifications" );

    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::GetStatusDialogTextL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::GetStatusDialogTextL( TDes& aBuffer )
    {
    FLOG( "CNSmlDMSyncHandler::GetStatusDialogTextL()" );
    
	aBuffer = KNullDesC;
	TInt phase = State()->SyncPhase();

    switch ( phase )
        {
        case CNSmlDMSyncState::EPhaseConnecting:
            {
            ReadL( aBuffer, R_SYNCSTATUS_SYNC_CONN );  // "Connecting"
            break;
            }
        case CNSmlDMSyncState::EPhaseSynchronizing:
            {
            if(iProgressDialog)
            ReadL( aBuffer, R_SYNCSTATUS_SYNC_SEND ); // "Synchronizing"
            
            else if(iFotaProgressDialog ) 
            ReadL( aBuffer, R_QTN_FOTA_WAIT_CHECKING_UPDATE );
            
            break;
            }
        case CNSmlDMSyncState::EPhaseDisconnecting:
            {
     
            TInt configFlags( EGenericSession );
            CRepository* centrep = NULL;
            TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
            if ( centrep )
                {
                centrep->Get( KDevManSessionType, configFlags );
                delete centrep;
                }
            if ( err != KErrNone ) 
                {
                User::Leave( err );
                }
            if( iFotaProgressDialog &&  configFlags == EGenericSession )
               {
               ReadL( aBuffer, R_SYNCSTATUS_SYNC_DISC );  // "Disconnecting"
               }
            else if( iProgressDialog )
               {
               ReadL( aBuffer, R_SYNCSTATUS_SYNC_DISC );  // "Disconnecting"
               }
             
            break;
            }
        case CNSmlDMSyncState::EPhaseCanceling:
            {
            ReadL( aBuffer, R_SYNCSTATUS_CANC );  // "Canceling"
            break;
            }
        case CNSmlDMSyncState::EPhaseSending:   // ReadProgressText handles both
        case CNSmlDMSyncState::EPhaseReceiving: // cases.
            {

            if( iProgressDialog )
               {
            	ReadProgressTextL( aBuffer, State()->Content(), State()->Progress(),
                               State()->TotalProgress(), phase );
               }
            TInt configFlags( EGenericSession );
            CRepository* centrep = NULL;
            TRAPD( err, centrep = CRepository::NewL( KCRUidDeviceManagementInternalKeys ) );
            if( centrep )
                {
                centrep->Get( KDevManSessionType, configFlags );
                delete centrep;
                }
            if( err != KErrNone ) 
               {
               User::Leave( err );
               }
            if( iFotaProgressDialog && iCheckUpdate && !iFotaProgressLaunched )
               {
               ReadProgressTextL( aBuffer, State()->Content(), State()->Progress(),
                               State()->TotalProgress(), phase );
               TFileName bitmapName;
               TParse parse;
    	       parse.Set(KBitmapFileName, &KDC_APP_BITMAP_DIR, NULL);
    	       TUtil::StrCopy(bitmapName, parse.FullName());
	           CEikImage* image = iFotaProgressDialog->CreateImageLC(KAknsIIDQgnNoteSml,bitmapName,
		                                                EMbmNsmldmsyncQgn_note_sml,
		                                              EMbmNsmldmsyncQgn_note_sml_mask);
	           iFotaProgressDialog->SetImageL(image);
               CleanupStack::Pop(image);
               }
             
            break;
            }
         
         case CNSmlDMSyncState::EPhaseFinal:
            {
            iCheckUpdate = ETrue;
            ReadL( aBuffer, R_QTN_FOTA_WAIT_CHECKING_UPDATE );  // "Check for update
            TFileName bitmapName;
       	    TParse parse;
    	    parse.Set(KBitmapFileName, &KDC_APP_BITMAP_DIR, NULL);
    	    TUtil::StrCopy(bitmapName, parse.FullName());
	        CEikImage* image = iFotaProgressDialog->CreateImageLC(KAknsIIDQgnMenuDm,bitmapName,
		                                               EMbmNsmldmsyncQgn_menu_dm,
		                                         EMbmNsmldmsyncQgn_menu_dm_mask);
	        iFotaProgressDialog->SetImageL(image);
            CleanupStack::Pop(image);
            break;
            }
         
        default:
            {
            break;
            }
        }
    FLOG( "CNSmlDMSyncHandler::GetStatusDialogTextL() completed" );
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::UpdateDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::UpdateDialogL()
	{
	FLOG( "CNSmlDMSyncHandler::UpdateDialogL()" );
	
	if ( iProgressDialog )
	    {
	    FLOG( "CNSmlDMSyncHandler::UpdateDialogL(): dialog exists update" );
    	TBuf<KBufSize256> stringHolder;
    	
    	TInt phase = State()->SyncPhase();
    	
        GetStatusDialogTextL( stringHolder );
        if ( stringHolder.Compare( KNullDesC ) != 0 ) // If correct text found
            {
            Dialog()->UpdateTextL( stringHolder );
            }
    	
    	if ( phase == CNSmlDMSyncState::EPhaseSending ||
    	     phase == CNSmlDMSyncState::EPhaseReceiving )
    		{
    		if (State()->Progress() == 1)
    			{
    			if ( State()->ProgressKnown() )
    				{
	    			Dialog()->CancelAnimation();
    	    		Dialog()->SetFinalProgress(State()->TotalProgress());
         	    	Dialog()->SetProgress(State()->Progress());
    				}
    			else
    				{
        			Dialog()->SetFinalProgress(0);
	    	        Dialog()->StartAnimationL();
    				}
    			}
    		else if ( State()->ProgressKnown() )
    			{
    			Dialog()->SetProgress( State()->Progress() );
    			}
    		}
	    }
	 
	 else if( iFotaProgressDialog )   
	 {
	 FLOG( "CNSmlDMSyncHandler::UpdateDialogL(): fota dialog" );
     TBuf<KBufSize256> stringHolder;	
     TInt phase = State()->SyncPhase();	
     GetStatusDialogTextL( stringHolder );
     if ( stringHolder.Compare( KNullDesC ) != 0 ) // If correct text found
         {
         iFotaProgressDialog->UpdateTextL( stringHolder );
         }
    	
     if ( phase == CNSmlDMSyncState::EPhaseSending ||
    	     phase == CNSmlDMSyncState::EPhaseReceiving )
    	 {
    		if (State()->Progress() == 1)
    			{
    			if ( State()->ProgressKnown() )
    				{
	    			iFotaProgressDialog->CancelAnimation();
    	    	    iFotaProgressDialog->SetFinalProgress(State()->TotalProgress());
         	    	iFotaProgressDialog->SetProgress(State()->Progress());
    				}
    			else
    				{
        			iFotaProgressDialog->SetFinalProgress(0);
	    	        iFotaProgressDialog->StartAnimationL();
    				}
    			}
    		else if ( State()->ProgressKnown() )
    			{
    			iFotaProgressDialog->SetProgress( State()->Progress() );
    			}
    	 }
	 }
	 
	FLOG( "CNSmlDMSyncHandler::UpdateDialogL() completed" );
	}


// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HandleActiveCallL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HandleActiveCallL()
	{
    FLOG( "CNSmlDMSyncHandler::HandleActiveCallL" );
    
    // HandleSyncErrorL will set this to true if
    // sync will be retried.
    iRetrySync = EFalse;

	if ( (iSyncError != KErrNone) && (iSyncError != KErrCancel) )
		{
		TRAP_IGNORE( HandleSyncErrorL() );
		}
	if ( iRetrySync == EFalse )
		{
	    // Inform parent that sync is done.
	    iSyncAppEngine->SyncCompleted( ENSmlSyncComplete );		
		}
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HandleSyncErrorL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HandleSyncErrorL()
	{
	FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL" );
	
	if ( iSyncError != KDMErr )
		{
    	GetErrorText( iBuf, iSyncError, iConnectionBearer );
    	CAknErrorNote* queryDialog = new (ELeave) CAknErrorNote(ETrue);
    	if( iSyncDocument->SANSupport() && iSyncDocument->SilentSession() )
    	   {
    	    queryDialog->SetTone(CAknErrorNote::ENoTone);    	
    	   }
        queryDialog->ExecuteLD( iBuf );
		}
	if ( ( iSyncError == SyncMLError::KErrAuthenticationFailure ) ||
	    ( iSyncError == SyncMLError::KErrTransportAuthenticationFailure ) )
        {
        TBool httpQuery( iSyncError == SyncMLError::KErrTransportAuthenticationFailure );
        
	    FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL ask username" );

	    CNSmlDMMultiQuery* dlg = CNSmlDMMultiQuery::NewL( iSyncAppEngine,
	                                                      iProfileId,
	                                                      httpQuery );
	    if ( dlg->ExecuteLD( R_NSMLDM_AUTHDIALOG ) )
		    {
		    // ok
		    FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL ask username ok retry sync" );
		    iRetrySync = ETrue;
            // We have already closed job so create a new.
          	iSyncJob.CreateL( Session(), iProfileId );
          	iSyncJobId = iSyncJob.Identifier();
		    SynchronizeL();
		    }
	    else
		    {
		    FLOG( "CNSmlDMSyncHandler::HandleSyncErrorL ask username cancel" );
		    }
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::GetErrorTextL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::GetErrorText( TDes& aErrorText,
                                       TInt aErrorCode,
                                       TInt aTransport )
	{
    FTRACE( RDebug::Print(
          _L("CNSmlDMSyncHandler::GetErrorText Errorcode (%d)"), aErrorCode) );
    
    aErrorText = KNullDesC();
   
    TInt id = R_QTN_DM_LOG_ERR_SYSTEMERROR;  // "System Error"
   
	switch ( aErrorCode )
		{
		case SyncMLError::KErrTransportAuthenticationFailure:
			{
			if ( aTransport == EAspBearerInternet )
				{
				// "Http authentication failed"
				id = R_QTN_NSML_LOG_ERR_CLIENTAUTH;  
				}
			else
				{
				// "Invalid user name or password"
				id = R_QTN_DM_LOG_ERR_CLIENTAUTH;
				}
		    break;
			}
		
		case SyncMLError::KErrAuthenticationFailure:
			{
			// "Invalid user name or password"
			id = R_QTN_DM_LOG_ERR_CLIENTAUTH;
		    break;
			}	

		case KErrServerBusy:
		    {
		    // "Synchronisation server was busy"
		    id = R_QTN_DM_LOG_ERR_SERVERBUSY;
		    break;
		    }
		    
		case SyncMLError::KErrServerDatabaseNotFound:
		    {
		    // "Invalid remote database"
		    id = R_QTN_DM_LOG_ERR_SESSIONABORT;
		    break;
		    }

		case SyncMLError::KErrConnectionError:
		    {
		    // "Server is not responding"
		    id = R_QTN_DM_LOG_ERR_SERVERNOTRESPONDING;
		    break;
		    }
		    
		case SyncMLError::KErrTransportSvrNoUri:
		    {
		    // "Invalid host address"
		    id = R_QTN_DM_LOG_ERR_URIINVALID;
		    break;
		    }
		case SyncMLError::KErrTransportDisconnected:
		{
			id = R_QTN_DM_LOG_ERR_TIMEDOUT;
			break;
		}
		
		case SyncMLError::KErrTransportRequest:
		case SyncMLError::KErrTransportHeader:
		case SyncMLError::KErrTransportSvrError:
		case SyncMLError::KErrCannotCommunicateWithServer:
		    {
		    // "Error in communication"
		    id = R_QTN_DM_LOG_ERR_COMM;
		    break;
		    }
		    
		case SyncMLError::KErrVerDTDMissing:
		case SyncMLError::KErrProtoMissing:
		case SyncMLError::KErrVersionDTDNotSupported:
		case SyncMLError::KErrProtoNotSupported:
		    {
		    // "Unsupported SyncML software version in server"
		    id = R_QTN_DM_LOG_ERR_VERSIONNOTSUPPORTED;  
		    break;
		    }
       
		case SyncMLError::KErrSessionIdNotValid:
		case SyncMLError::KErrMappingInfoMissing:
		case SyncMLError::KErrMappingInfoNotAcked:
		case SyncMLError::KErrStatusCommandForCommandNotSent:
		case SyncMLError::KErrStatusCommandNotTheSame:
		case SyncMLError::KErrMissingStatusCommands:
		case SyncMLError::KErrFailedToAddRecord:
		case SyncMLError::KErrFailedToReplaceRecord:
		case SyncMLError::KErrFailedToDeleteRecord:
		case SyncMLError::KErrXmlParsingError:
		case SyncMLError::KErrReferenceToolkitError:
		case SyncMLError::KErrObjectTooLargeToSendToServer:
		case SyncMLError::KErrFailedToGetRecord:
		case SyncMLError::KErrUserInteractionFailed:
		case SyncMLError::KErrStatusSizeMismatch:
		case SyncMLError::KErrFailedToCloseDatabase:
		case SyncMLError::KErrDatabaseAdapterIndexError:
		case SyncMLError::KErrClientDatabaseNotFound:
		    {
		    // "Error in sync server"
	   	    id = R_QTN_DM_LOG_ERR_SERVERERROR;
		    break;
		    }
		case SyncMLError::KErrPhoneOnOfflineMode:
		    {
		    // "Phone in offline"
	   	    id = R_QTN_DM_LOG_ERR_OFFLINE;
		    break;	        
		    }

		case SyncMLError::KErrInvalidCredentials:
		    {
		    // Server authentication failed
	   	    id = R_QTN_DM_LOG_ERR_SERVERTAUTH;
		    break;	        
		    }
	        
		//TARM Enabled build Server profile Cert not stored or invalid    
        case SyncMLError::KErrInvalidCert:
	      {
	      	if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
	      	{
	      	id =  R_QTN_DM_ERROR_UNTRUSTED_CERT ;
	      	}
	      	break;
	      }
        default:
            {
            if ( ( aErrorCode == KErrDiskFull ) ||
                 ( aErrorCode == KErrNoMemory ) )
                {
                TRAPD( retVal, SetErrorMessageL( aErrorCode, aErrorText ) );
                if ( retVal == KErrNone )
                    {
                    return;
                    }
                }
            // "System Error"
            id = R_QTN_DM_LOG_ERR_SYSTEMERROR;
		    break;
            }
		}
		
	TRAP_IGNORE( ReadL( aErrorText, id ) );
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ShowProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::ShowProgressDialogL( )
	{
	FLOG( "CNSmlDMSyncHandler::ShowProgressDialogL" );
	
	delete iProgressDialog;
	iProgressDialog = NULL;
	
	if ( iUseFotaProgressNote )
	    {
      
        iFotaProgressDialog = CNSmlDMProgressDialog::NewL( this );
        iFotaProgressLaunched = ETrue;
        TBuf<KBufSize> buf;
    	GetStatusDialogTextL( buf );
       	TFileName bitmapName;
       	TParse parse;
    	parse.Set(KBitmapFileName, &KDC_APP_BITMAP_DIR, NULL);
    	TUtil::StrCopy(bitmapName, parse.FullName());
    	
	    if ( iServerAlertedSync )
		    {
		     iFotaProgressDialog->LaunchProgressDialogL(
                                    buf,
		                            KAknsIIDQgnNoteSml,
		                            bitmapName,
		                            EMbmNsmldmsyncQgn_note_sml_server,
		                            EMbmNsmldmsyncQgn_note_sml_server_mask );		                            
		    }
	    else
		    {
		    iFotaProgressDialog->LaunchProgressDialogL( 
		                            buf,
		                            KAknsIIDQgnNoteSml,
		                            bitmapName,
		                            EMbmNsmldmsyncQgn_note_sml,
		                            EMbmNsmldmsyncQgn_note_sml_mask );
    		}
	    iFotaProgressLaunched = EFalse;
		UpdateDialogL();
        
	    }
    else
        {
    	iProgressDialog = CNSmlDMProgressDialog::NewL( this );

    	TBuf<KBufSize> buf;
    	GetStatusDialogTextL( buf );

       	TFileName bitmapName;
       	TParse parse;
    	parse.Set(KBitmapFileName, &KDC_APP_BITMAP_DIR, NULL);
    	TUtil::StrCopy(bitmapName, parse.FullName());
    	
	if ( iServerAlertedSync )
		{
		iProgressDialog->LaunchProgressDialogL(
                                    buf,
		                            KAknsIIDQgnNoteSml,
		                            bitmapName,
		                            EMbmNsmldmsyncQgn_note_sml_server,
		                            EMbmNsmldmsyncQgn_note_sml_server_mask );		                            
		}
	else
		{
		iProgressDialog->LaunchProgressDialogL( 
		                            buf,
		                            KAknsIIDQgnNoteSml,
		                            bitmapName,
		                            EMbmNsmldmsyncQgn_note_sml,
		                            EMbmNsmldmsyncQgn_note_sml_mask );
    		}
		UpdateDialogL();
        }
        
	FLOG( "CNSmlDMSyncHandler::ShowProgressDialogL() completed" );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::HideProgressDialogL
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::HideProgressDialogL()
    {
    FLOG( "CNSmlDMSyncHandler::HideProgressDialogL" );


    if ( iFotaProgressDialog )
		{
		FLOG( "CNSmlDMSyncHandler::HideProgressDialogL dialog exists hide it" );
	    iFotaProgressDialog->CancelProgressDialogL();
	    delete iFotaProgressDialog;
	    iFotaProgressDialog = NULL;
		}
    
	if ( iProgressDialog )
		{
		FLOG( "CNSmlDMSyncHandler::HideProgressDialogL dialog exists hide it" );
	    iProgressDialog->CancelProgressDialogL();
	    delete iProgressDialog;
	    iProgressDialog = NULL;
		}
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SetErrorMessageL
// 
// Set synchronisation error message text.
// -----------------------------------------------------------------------------
//
void CNSmlDMSyncHandler::SetErrorMessageL(TInt aError, TDes& aMsg)
	{
	FLOG( "CNSmlDMSyncHandler::SetErrorMessageL" );
	
	CTextResolver* textResolver = CTextResolver::NewLC(); 
	
    aMsg.Append( textResolver->ResolveErrorString( aError,
	                                         CTextResolver::ECtxNoCtx ) );

	CleanupStack::PopAndDestroy( textResolver );

	TBuf<KBufSize> buf;
    ReadL( buf, R_SML_APP_TITLE );
	TUtil::StrInsert( aMsg, buf );
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession& CNSmlDMSyncHandler::Session()
	{
	__ASSERT_DEBUG(iSyncSession, TUtil::Panic(KErrGeneral));
	
	return *iSyncSession;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::Dialog
// -----------------------------------------------------------------------------
//
CNSmlDMProgressDialog* CNSmlDMSyncHandler::Dialog()
	{
	__ASSERT_DEBUG(iProgressDialog, TUtil::Panic(KErrGeneral));

	return iProgressDialog;
	}


// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::SyncRunning
// -----------------------------------------------------------------------------
//
TBool CNSmlDMSyncHandler::SyncRunning()
	{
	FTRACE( RDebug::Print(
	          _L("CNSmlDMSyncHandler::SyncRunning status (%d)"), iSyncRunning ) );
	          
	return iSyncRunning;
	}

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::State
// -----------------------------------------------------------------------------
//
CNSmlDMSyncState* CNSmlDMSyncHandler::State()
	{
	__ASSERT_DEBUG(iState, TUtil::Panic(KErrGeneral));

	return iState;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ReadL
// -----------------------------------------------------------------------------
//	
void CNSmlDMSyncHandler::ReadL( TDes& aText, TInt aResourceId )
    {
    FLOG( "CNSmlDMSyncHandler::ReadL" );
    
    HBufC* buf = StringLoader::LoadLC( aResourceId );
    TUtil::StrCopy( aText, *buf );
    CleanupStack::PopAndDestroy( buf );
    }
    
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::ReadProgressTextL
// -----------------------------------------------------------------------------
//	
void CNSmlDMSyncHandler::ReadProgressTextL( TDes& aText,
                                            const TDesC& /*aContent*/,
                                            TInt /*aCurrent*/,
                                            TInt aFinal,
                                            TInt aPhase )
	{
	FLOG( "CNSmlDMSyncHandler::ReadProgressTextL" );
	
	aText = KNullDesC;

	HBufC* buf = NULL;
    
    TBool send = EFalse;
    if ( aPhase == CNSmlDMSyncState::EPhaseSending )
    	{
    	send = ETrue;
    	}
	if ( send && aFinal == -1 )
		{
	    buf = StringLoader::LoadLC( R_SYNCSTATUS_SYNC_SEND, iServerName );
		}
	if ( send && aFinal >= 0 )
		{
		buf = StringLoader::LoadLC( R_SYNCSTATUS_SYNC_SEND, iServerName );	
		}
	if ( !send && aFinal == -1 )
		{
	    buf = StringLoader::LoadLC( R_SYNCSTATUS_SYNC_RECV, iServerName );
		}
	if ( !send && aFinal >= 0 ) 
		{
	    buf = StringLoader::LoadLC( R_SYNCSTATUS_SYNC_RECV, iServerName );	
		}

	if ( buf )
		{
	    TUtil::StrCopy( aText, *buf );
	    CleanupStack::PopAndDestroy( buf );
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::CancelSynchronizeL
// -----------------------------------------------------------------------------
//		
void CNSmlDMSyncHandler::CancelSynchronizeL()
    {
    FLOG( "CNSmlDMSyncHandler::CancelSynchronizeL" );
    
    if ( iSyncRunning )
        {
        iSyncJob.StopL();
        TInt err = RProperty::Set( KPSUidNSmlDMSyncApp,
        			KDmJobCancel, KErrCancel);
        FTRACE( FPrint(
            _L( "CNSmlDMSyncHandler::CancelSynchronizeL() KDmJobCancel=-3, err = %d" ),
                err ) );
        SynchronizeCompletedL( KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CNSmlDMSyncHandler::DialogDismissedL
// -----------------------------------------------------------------------------
//		
void CNSmlDMSyncHandler::DialogDismissedL( TInt aButtonId )
    {
    FTRACE( FPrint(
        _L( "CNSmlDMSyncHandler::DialogDismissedL(), aButtonId = %d" ),
        aButtonId ) );
    
    HandleDialogEventL( aButtonId );
    
    FLOG( "CNSmlDMSyncHandler::DialogDismissedL() completed" );
    }

// End of File
