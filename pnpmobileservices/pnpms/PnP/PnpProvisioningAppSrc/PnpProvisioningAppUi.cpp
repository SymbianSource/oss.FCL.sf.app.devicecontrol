/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/



// INCLUDE FILES
#include <avkon.hrh>
#include <apgcli.h>
#include <textresolver.h>
#include <rconnmon.h>
#include <ApUtils.h>
#include <pnpprovisioning.rsg>
#include <browseruisdkcrkeys.h>         // for KCRUidBrowser
#include <AknGlobalNote.h>              // for CAknGlobalNote
#include <AknGlobalConfirmationQuery.h> // for CAknGlobalConfirmationQuery

#include "PnpProvisioningApp.h"
#include "PnpProvisioningAppUi.h"
#include "PnpProvisioningDocument.h"
#include "pnpprovisioning.hrh"
#include "PnpLogger.h"
#include "cwaitdialogmonitor.h"


_LIT( KQuestionMark, "?" );
_LIT( KAmpersand, "&" );
_LIT( KStatus, "Status=" );
_LIT( KSpace, " " );

const TUint KCallbackDelay = 1000000;



// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CPnpProvisioningAppUi::ConstructL()
// 
// ----------------------------------------------------------
//
void CPnpProvisioningAppUi::ConstructL()
    {
    LOGSTRING( "CPnpProvisioningAppUi::ConstructL" );
    BaseConstructL( ENoScreenFurniture );
    //send to backround
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KUidPnpProvisioning );
    task.SendToBackground();
    this->StatusPane()->MakeVisible( EFalse );
    HideApplicationFromFSW();
    
    CEikonEnv::Static()->AddForegroundObserverL( *this );
    
    // if service activation enabled allow automatic settings configuration instead
    // of showing confirmation notes

    TBool activationenabled = ServiceActivationEnabledL();
    
    if(!activationenabled)
    {
    HBufC* msgConfirmSave = CEikonEnv::Static()->AllocReadResourceLC(
            R_TEXT_QUERY); 
            
        CAknGlobalConfirmationQuery* query = CAknGlobalConfirmationQuery::NewL();
        CleanupStack::PushL( query );
        TRequestStatus statusSave;
        query->ShowConfirmationQueryL(
            statusSave, 
            *msgConfirmSave, 
            R_AVKON_SOFTKEYS_YES_NO__YES,
            R_QGN_NOTE_QUERY_ANIM);
            
        User::WaitForRequest( statusSave );
        CleanupStack::PopAndDestroy( 2 );
        
        if( statusSave != EAknSoftkeyYes )
            {
            LOGSTRING("User rejected");
            Exit();
            }
    }
    ResolveApL( iApInUse );
    
    ShowWaitNoteL();

    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard  );
    iPeriodic->Start( KCallbackDelay, KCallbackDelay, TCallBack(CPnpProvisioningAppUi::PeriodicCallback, this) );
    LOGSTRING( "CPnpProvisioningAppUi::ConstructL - done" );
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::~CPnpProvisioningAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CPnpProvisioningAppUi::~CPnpProvisioningAppUi()
    {
    LOGSTRING( "~CPnpProvisioningAppUi" );

    // iPeriodic is deleted in the document's destructor
    if( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    if (iDoorObserver)
        {
        LOGSTRING( "calling iDoorObserver->NotifyExit" );
        iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::ENoChanges);
        }
        
    if( iGlobalWaitNote )
        {
        delete iGlobalWaitNote;
        }
        
    if( iWaitDialogMonitor )
        {
        iWaitDialogMonitor->Cancel();
        delete iWaitDialogMonitor;
        }
    LOGSTRING( "~CPnpProvisioningAppUi done" );
   }

// ------------------------------------------------------------------------------
// CPnpProvisioningAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CPnpProvisioningAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CPnpProvisioningAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }

        default:
            break;      
        }
    }
    
// -----------------------------------------------------------------------------
// CPnpProvisioningAppUi::HandleGainingForeground
//
// -----------------------------------------------------------------------------
//
void CPnpProvisioningAppUi::HandleGainingForeground()
    {
    LOGSTRING( "CPnpProvisioningAppUi::HandleGainingForeground" );
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KUidPnpProvisioning );
    if( task.Exists() )
        {
        task.SendToBackground();
        }
    }
    
// -----------------------------------------------------------------------------
// CPnpProvisioningAppUi::HandleLosingForeground
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPnpProvisioningAppUi::HandleLosingForeground()
    {
    LOGSTRING( "CPnpProvisioningAppUi::HandleLosingForeground" );
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::RedirectL
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::RedirectL( const TDesC& aUrl, THttpProvStates::TProvisioningStatus  aStatus )
    {
    // do the redirect thing
    LOGSTRING( "RedirectL" );

    // Parameters are separated by space
    // 1st parameter: type of the further parameters
    // 2nd parameter: URL or  the uid of bookmark/saved deck/pushed deck.
    // 3rd parameter: Access point Uid (in decimal format).
    // the 3rd parameter is optional, and only for specifying AP for URL-s

    HBufC* param = HBufC::NewLC( 300 );
    TPtr paramPtr = param->Des();
    paramPtr.Copy( _L( "4 " ) );
    
    
    paramPtr.Append( aUrl );

    // Assume there might already be parameters in the URL given
    if( aUrl.Find( KQuestionMark ) != KErrNotFound )
        {
        paramPtr.Append( KAmpersand );
        }
    else
        {
        paramPtr.Append( KQuestionMark );
        }

    paramPtr.Append( KStatus );
    paramPtr.AppendNum( (TInt) aStatus );

    // Only give a valid AP
    if( iApInUse != 0 )
        {
        paramPtr.Append( KSpace );
        paramPtr.AppendNumUC( iApInUse );
        }

#ifdef _DEBUG
    LOGSTRING( "redirecting params:" );
    for( TInt i(0); i < paramPtr.Length(); i += 128 )
        {
        LOGTEXT( paramPtr .Right( paramPtr .Length() - i ) );
        }
#endif

    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KCRUidBrowser );
    if ( task.Exists() )
        {
        LOGSTRING( "redirecting browser" );
        HBufC8* param8 = HBufC8::NewLC( param->Length() );
        param8->Des().Append( *param );
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        //task.BringToForeground();     
        CleanupStack::PopAndDestroy( param8 );
        }
    else 
        {
        LOGSTRING( "Could not find browser" );
        RApaLsSession   appArcSession;
        User::LeaveIfError(appArcSession.Connect());    // connect to AppArc server
        TThreadId id;
        appArcSession.StartDocument( *param, KCRUidBrowser, id );
        appArcSession.Close();
        }
    CleanupStack::PopAndDestroy( param ); // param
    LOGSTRING( "calling Exit()" ); 
    Exit();
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::ResolveApL
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::ResolveApL( TUint32& aAp )
    {
    LOGSTRING("Trying to get AP used");


    // PnPProvisioning app might be started by browsing to a service using
    // PAOS filter 
    // -> OnlineSupport app that normally sets the AP to DB might not have been
    // used. It is also possible that the AP that OnlineSupport app used has
    // been removed.

    // So we try to use the AP currently in use first; if none is in use read the AP
    // from shared data / cenrep.

    RConnectionMonitor connMon;
    connMon.ConnectL();
    CleanupClosePushL( connMon );
    TUint connectionCount(0);
    TRequestStatus status( KRequestPending );
    connMon.GetConnectionCount( connectionCount, status );
    // No user interaction possible at this point so no need to make the call asynchronously
    User::WaitForRequest( status );
    LOGSTRING("Trying to get AP used - 2");

    for( TUint i(1); i <= connectionCount; i++ )
        {
        LOGSTRING("Trying to get AP used - 3");
        TUint connectionId(0);
        TUint subConnectionCount(0);
        TInt err = connMon.GetConnectionInfo( i, connectionId, subConnectionCount );
        if( err != KErrNone )
            {
            LOGSTRING2( "err %i", err );
            }

        LOGSTRING("Trying to get AP used - 4");
        TUint ap(0);
        TRequestStatus status2( KRequestPending );
        connMon.GetUintAttribute( connectionId, 0, KIAPId, ap, status2 );
        User::WaitForRequest( status2 );
        if( status2.Int() != KErrNone )
            {
            LOGSTRING2( "err %i", status2.Int() );
            }
        LOGSTRING("Trying to get AP used - 5");
        if( ap != 0 )
            {
            LOGSTRING2("Trying to get AP used - found: %i", ap );

            CCommsDatabase* commDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
            CleanupStack::PushL( commDb );
            CApUtils* utils = CApUtils::NewLC( *commDb );
            //aAp = utils->WapIdFromIapIdL( ap );
            LOGSTRING2( "wap id: %i", aAp );
            
            // In some cases the connection has to be reset after saving
            // provisioned settings
            TInt err = connMon.SetBoolAttribute( 0, 0, KConnectionStopAll, ETrue );
            LOGSTRING2( "Connection stop error: %i", err );
            CleanupStack::PopAndDestroy( utils );
            CleanupStack::PopAndDestroy( commDb );
            CleanupStack::PopAndDestroy(); // connMon.Close()
            LOGSTRING("Trying to get AP used - done");
            return;
            }
        }
    CleanupStack::PopAndDestroy(); // connMon.Close()
    }
    
// ----------------------------------------------------
// CPnpProvisioningAppUi::DoExit()
// exits app.
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::DoExit()
    {
    LOGSTRING( "DoExit" );
    Exit();
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::PeriodicCallback
// ----------------------------------------------------
//
TInt CPnpProvisioningAppUi::PeriodicCallback( TAny* aPtr )
    {
    TRAPD( err, ( (CPnpProvisioningAppUi*)aPtr )->DoPeriodicCallbackL( aPtr ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "Error in DoPeriodicCallBackL: %i", err );
        if( err == KLeaveExit )
            User::Leave( KLeaveExit );
        }
    return KErrNone;
    }

// ----------------------------------------------------
// CPnpProvisioningAppUi::DoPeriodicCallbackL
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::DoPeriodicCallbackL( TAny* aPtr )
    {
    LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback" );

    CPnpProvisioningAppUi* ui = (CPnpProvisioningAppUi*)aPtr;
    CPnpProvisioningDocument* document = (CPnpProvisioningDocument*)ui->Document();
    TInt leavestatus = KErrNone;
    TInt noRedirect = 0;
    THttpProvStates::TProvisioningStatus status = THttpProvStates::EStatusOk;

    LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback - Is doc ready?" );

    if( !(document->Ready()) )
        {
        LOGSTRING( "Document not ready yet" );
        User::Leave( KErrNotReady );
        }

    ui->iPeriodic->Cancel();
    // document->SetPeriodic( ui->iPeriodic );
    TRAP( leavestatus, status = document->SaveSettingsL() );
    if( leavestatus != KErrNone )
        {
        LOGSTRING2( "SaveSettingsL leave code: %i", leavestatus );
        
        if( leavestatus == THttpProvStates::EStatusRejected )
            {
            // user has cancelled
            ui->DoExit();
            }
        else
            {
            ui->ShowErrorNoteL();
            }
        }

    LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback - get report url" );

    // try to get the redirect url
    TBuf<512> url;
    TRAPD( err, url.Copy( document->GetReportUrlL().Left(512) ) );
    if( err != KErrNone )
        {
        LOGSTRING2( "no report url:%i", err );
        
        TerminateBrowserL(err, noRedirect);
        
        ui->DoExit();
        }
    else
        {
        if( status != THttpProvStates::EStatusOk )
            {
            LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback - do redirect" );
            
            TerminateBrowserL(KErrGeneral, noRedirect);
            if(noRedirect == 1)
            ui->DoExit();
            ui->RedirectL( url, status );
            }
        else if( leavestatus != KErrNone )
            {
            LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback - do redirect with leave status" );
            
            TerminateBrowserL(leavestatus, noRedirect);
            if(noRedirect == 1)
            ui->DoExit();
            
            ui->RedirectL( url, THttpProvStates::TProvisioningStatus( leavestatus ) );
            }
        else
            {
            LOGSTRING2( "Provisioning status: %i", status );

            // Since Plat sec there is no need to restart Browser after provisioning
            // has been done.
            // In some cases the connection has to be reset, though
            // (it is done in ResolveApL)
            LOGSTRING( "Making redirect" );
            
           //Before Making redirect check if application launched PNPMS.
           //If application has not launched PNPMS client then do redirect
           // otherwise call for Service Activation and launch choosen application
           // closing browser
           
        
          		TerminateBrowserL(KErrNone,noRedirect);
          		
	    		if(noRedirect == 1)
            		  ui->DoExit();
	    		
            
            ui->RedirectL( url, THttpProvStates::EStatusOk );
            }
        }

    LOGSTRING( "CPnpProvisioningAppUi::PeriodicCallback - done" );
    }
    
//// ----------------------------------------------------
// CPnpProvisioningAppUi::ServiceActivationEnabled()
// ----------------------------------------------------
//
TBool CPnpProvisioningAppUi::ServiceActivationEnabledL()
{
	CPnpProvUtil *provUtil = CPnpProvUtil::NewLC();
	TUint32 uidval = 0;
	TRAPD(err, uidval = provUtil->GetApplicationUidL());
		              
	CleanupStack::PopAndDestroy();    //provutil     

	if(uidval && !err)
        {
	 	return ETrue;
	}
	
	return EFalse;
	
}


// ----------------------------------------------------
// CPnpProvisioningAppUi::TerminateBrowserIfErrorL()
// ----------------------------------------------------
//

void CPnpProvisioningAppUi::TerminateBrowserL(TInt aError, TInt& aNoRedirect)
{
	    // Terminate browser and launch choosen application based on status of
      // aNoRedirect status
      // If error status is KErrNone  the application is launched closing browser
      // If there are errors then terminate browser without launching application
	
	    	CPnpProvUtil *provUtil = CPnpProvUtil::NewLC();
        CPnpServiceActivation *serviceActivate = CPnpServiceActivation::NewLC();
    		TUint32 uidval = 0;
		
				TRAPD(err, uidval = provUtil->GetApplicationUidL());
                      
        if(uidval && !err)
        {
            if(aError == KErrNone)
            	TRAP_IGNORE(serviceActivate->LaunchApplicationL());
          	
          	TRAP_IGNORE(serviceActivate->KillBrowserL());
						TRAP_IGNORE(provUtil -> SetApplicationUidL(0));
			
	   				// Interger value is required in case of handling multiple cases in redirection
	   				// With current implementation only 2 values are supported 0 and 1
	   				aNoRedirect = 1;	
			
			 	
        }
        
        CleanupStack::PopAndDestroy(2);
        
        
}


// ----------------------------------------------------
// CPnpProvisioningAppUi::ShowErrorNoteL()
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::ShowErrorNoteL()
    {
    HBufC* msgTextSaved = CEikonEnv::Static()->
            AllocReadResourceLC( R_TEXT_CANNOT_SAVE ); 

    CAknGlobalNote* globalNote = CAknGlobalNote::NewL();
    CleanupStack::PushL( globalNote );
    globalNote->ShowNoteL( EAknGlobalErrorNote , *msgTextSaved );
    CleanupStack::PopAndDestroy( 2 );
    }
    
// ----------------------------------------------------
// CPnpProvisioningAppUi::ShowWaitNoteL()
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::ShowWaitNoteL()
    {
    HBufC* msgText = CEikonEnv::Static()->
        AllocReadResourceLC( R_TEXT_WAIT_SAVING ); 

    if (iWaitDialogMonitor)
        {
        iWaitDialogMonitor->Cancel();
        delete iWaitDialogMonitor;
        iWaitDialogMonitor = NULL;
        }
    // instantiate the active object CGlobalConfirmationObserver
    iWaitDialogMonitor = CWaitDialogMonitor::NewL( *this );
    
    // SetActive
    iWaitDialogMonitor->Start();
    
    if (!iGlobalWaitNote)
        {
        iGlobalWaitNote = CAknGlobalNote::NewL();
        }

     iGlobalWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY);
    iGlobalWaitNote->ShowNoteL(
        iWaitDialogMonitor->iStatus,
        EAknGlobalWaitNote,
        *msgText );
    
    CleanupStack::PopAndDestroy( msgText );
    }
    
// ----------------------------------------------------
// CPnpProvisioningAppUi::WaitDialogDismissedL()
// ----------------------------------------------------
//
void CPnpProvisioningAppUi::WaitDialogDismissedL( const TInt aStatusCode )
    {
    LOGSTRING2( "CPnpProvisioningAppUi::WaitDialogDismissedL aStatus is %i",aStatusCode );
    LOGSTRING2( "iWaitDialogMonitor->iStatus is %i" , iWaitDialogMonitor->iStatus.Int() );
    if( aStatusCode == EAknSoftkeyCancel || aStatusCode == EAknSoftkeyExit ||
    			aStatusCode == KErrCancel ) //End key results in KErrCancel
        {
        // user cancelled
        LOGSTRING( "CPnpProvisioningAppUi::WaitDialogDismissedL - user cancelled" );
        Exit();
        }
//Handling End Key
    if(aStatusCode == EAknSoftkeyEmpty && iWaitDialogMonitor->iStatus == KErrCancel )
    	{
    	LOGSTRING( "End key or User cancel done, but DOc tries to close instead of RunL of iWaitDialogMonitor " );
    	//Exit();
    	iWaitDialogMonitor->Cancel();
    	User::Leave( THttpProvStates::EStatusRejected );
    	}
        
    if( iGlobalWaitNote )
        {
        delete iGlobalWaitNote;
        iGlobalWaitNote = NULL;
        }
        
    if( iWaitDialogMonitor )
        {
        iWaitDialogMonitor->Cancel();
        delete iWaitDialogMonitor;
        iWaitDialogMonitor = NULL;
        }
    }

// End of File  
