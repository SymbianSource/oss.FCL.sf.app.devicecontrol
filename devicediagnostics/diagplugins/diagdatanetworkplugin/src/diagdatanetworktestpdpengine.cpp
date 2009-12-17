/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the module for the Diagnostics Data Network Test PDP-Engine
*
*/


// INCLUDES

// Class Declaration 
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include "diagdatanetworktestpdpengine.h"   // CDiagDataNetworkTestPDPEngine

// System includes
#include <centralrepository.h>              // CRepository
#include <logcli.h>                         // CLogFilter   
#include <DiagNetworkRegStatusWatcher.h>    // CDiagNetworkRegStatusWatcher
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include <diaglogeventeraser.h>             // CDiagLogEventEraser

// User includes
#include "diagdatanetworkprivatecrkeys.h"
#include <devdiagdatanetworktestpluginrsc.rsg>
#include	"diagdatanetworktestplugin.hrh"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// Symbian OS default constructor
// ---------------------------------------------------------------------------   
CDiagDataNetworkTestPDPEngine* CDiagDataNetworkTestPDPEngine::NewL(
    MDiagDataNetworkTestCallBackInterface& aInterface)
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::NewL()")
    
    CDiagDataNetworkTestPDPEngine* self = 
                        new (ELeave) CDiagDataNetworkTestPDPEngine(aInterface);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }


// ----------------------------------------------------------------------------
// Standard C++ Constructor
// NOTE that CActive(CActive::EPriorityUserInput) is called in 
// CDiagPDPTestEngine::CDiagPDPTestEngine()
// ----------------------------------------------------------------------------
//
CDiagDataNetworkTestPDPEngine::CDiagDataNetworkTestPDPEngine(
                    MDiagDataNetworkTestCallBackInterface& aInterface) 
    :iInterface(aInterface)
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::CDiagDataNetworkTestPDPEngine()")
    
    // Add this object to active scheduler
    CActiveScheduler::Add( this );         
    }
    
// ---------------------------------------------------------------------------
// Symbian OS second phase constructor
// ---------------------------------------------------------------------------    
void CDiagDataNetworkTestPDPEngine::ConstructL()
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::ConstructL()")
    SetStateL( EStateUnknown );
    idialogOn = EFalse;
	idialogDismissed = EFalse;
    }
    

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------    
CDiagDataNetworkTestPDPEngine::~CDiagDataNetworkTestPDPEngine()
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::~CDiagDataNetworkTestPDPEngine()")
            
    // call CActive::DoCancel() to stop any ongoing async request
    Cancel();
    
    delete iLogEventEraser;
    iLogEventEraser = NULL;
    
    delete iNetRegStatusWatcher;
    iNetRegStatusWatcher = NULL;        
        
    iPrimaryAPNIapIdArray.Close();
    iSecondaryAPNIapIdArray.Close();    
      
    iConnection.Close();
    iSocketServ.Close();     
    if ( iWaitDialog )
    delete iWaitDialog;   
    }


// ----------------------------------------------------------------------------
// This function reports the total steps of the engine class
// ----------------------------------------------------------------------------    
TUint CDiagDataNetworkTestPDPEngine::TotalSteps()
    {
    return EStateMax;
    }    


// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::RunDataNetworkTestL()
//
// This function is the entry point to Data Network Test.
// --------------------------------------------------------------------------- 
void CDiagDataNetworkTestPDPEngine::RunDataNetworkTestL()
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::RunDataNetworkTestL()")
    
    delete iNetRegStatusWatcher;
    iNetRegStatusWatcher = NULL;    
    
    iNetRegStatusWatcher = CDiagNetworkRegStatusWatcher::NewL(*this);
    
    // start the network registration status observer
    // the initial network registration status wiil be obtained via
    // callback interface InitialNetworkRegistrationStatusL()
    SetStateL( EStateWaitForInitialNetworkRegStatus );
    iNetRegStatusWatcher->StartObserver();
    }
    
    
// ----------------------------------------------------------------------------
// This function is used to inform the intial network registration status
// ----------------------------------------------------------------------------
void CDiagDataNetworkTestPDPEngine::InitialNetworkRegistrationStatusL( 
    const TBool& aRegistered )
    {    
    if( aRegistered )
        {
        LOGSTRING("CDiagDataNetworkTestPDPEngine::InitialNetworkRegistrationStatusL() aRegistered == ETrue")
        
        StartPdpEstablishmentL();
        }
    else
        {
        LOGSTRING("CDiagDataNetworkTestPDPEngine::InitialNetworkRegistrationStatusL() aRegistered == EFalse")
        
        // intial registration status is not registered
        // report the test result as Failed
        ReportResultToPluginL( EFalse );        
        }
    }
    
// ----------------------------------------------------------------------------
// This interface is used to inform the clients of any changes in the network 
// registration
// ----------------------------------------------------------------------------
void CDiagDataNetworkTestPDPEngine::NetworkRegistrationStatusChangeL( 
    const TBool& aRegistered )
    {
    if(!aRegistered)
        {
        // device has lost network coverage, 
        LOGSTRING("CDiagDataNetworkTestPDPEngine::NetworkRegistrationStatusChangeL() aRegistered == EFalse")
        LOGSTRING("device has lost network coverage")
        
        // stop the observer and fail the test
        iNetRegStatusWatcher->StopObserver();
        
        // cancel any ongoing asynchronous request
        Cancel();
        
        ReportResultToPluginL(CDiagResultsDatabaseItem::EFailed);
        }
    else
        {
        // continue the test, device is still registered
        LOGSTRING("CDiagDataNetworkTestPDPEngine::NetworkRegistrationStatusChangeL() aRegistered == ETrue")
        }        
    }
    
    
// ----------------------------------------------------------------------------
// This function is used to start checking if connection parameters exist
// and then connection establishment, if necessary
// ----------------------------------------------------------------------------  
void CDiagDataNetworkTestPDPEngine::StartPdpEstablishmentL()
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::StartPdpEstablishmentL()");
        
    SetStateL( EStateCheckingConnecitonParams );
    
    //TInt err = KErrGeneral;
    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iConnection.Open(iSocketServ));
    
    LOGSTRING("CDiagDataNetworkTestPDPEngine : socket server session opened OK ");  
    
    // Check for Active Connections 
    
    TUint connectionCount = 0;
    TPckgBuf<TConnectionInfo> connectionInfo;
    User::LeaveIfError(iConnection.EnumerateConnections(connectionCount));
    LOGSTRING("iConnection.EnumerateConnections");
    // if connection active report pass.
    if(connectionCount > 0)
    ReportResultToPluginL( ETrue );
    else
                EstablishPDPContextL();

    }    

// ---------------------------------------------------------------------------
// This function is the concrete implementation of pure virtual function
// CDiagPDPTestEngine::ReadAPNsFromCenrepL(). Here the primary and secondary APNs
// are read from the central repository. The function sets the reference argument
// aError as KErrNone if both APNs are found in the cenrep.
// ---------------------------------------------------------------------------    
    
void CDiagDataNetworkTestPDPEngine::ReadAPNsFromCenrepL(TInt& aError)
    {      
    LOGSTRING("CDiagDataNetworkTestPDPEngine::ReadAPNsFromCenrepL()");
         
    TInt errPrimary = KErrGeneral;
    TInt errSeondary = KErrGeneral;
           
    CRepository  *cenRep;
    cenRep = CRepository::NewL( KCRUidDiagDataNetworkTestPlugin );    
    CleanupStack::PushL( cenRep );        
    iCenrepPrimaryAPN.Zero();
    iCenrepSecondaryAPN.Zero();
    errPrimary = cenRep->Get( KDiagDataNetworkTestPluginPrimaryAPN,iCenrepPrimaryAPN );
    errSeondary = cenRep->Get( KDiagDataNetworkTestPluginSecondaryAPN,iCenrepSecondaryAPN);
    
    // check both APNs exist in Cenrep
    if(errPrimary == KErrNone && errSeondary == KErrNone  )
        {
        aError = KErrNone;
        }        
    else
        {
        aError = KErrGeneral;
        }
    CleanupStack::PopAndDestroy(cenRep);  
    cenRep = NULL; 
    
    
    LOGSTRING("CDiagDataNetworkTestPDPEngine::ReadAPNsFromCenrepL()  Primary APN = ");
    LOGTEXT(iCenrepPrimaryAPN)

    LOGSTRING("CDiagDataNetworkTestPDPEngine::ReadAPNsFromCenrepL()  Secondary APN = ");
    LOGTEXT(iCenrepSecondaryAPN)
    }
    
// ----------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::EstablishPDPContextL
//
// This the entry point to start establishing PDP context
// ----------------------------------------------------------------------------       
void CDiagDataNetworkTestPDPEngine::EstablishPDPContextL()
    {   
    LOGSTRING("CDiagDataNetworkTestPDPEngine::EstablishPDPContextL()")
 
        SetStateL( EStateWaitForConnectionCompletion );

        iPref.SetDialogPreference(ECommDbDialogPrefPrompt);
        //iPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        iPref.SetDirection( ECommDbConnectionDirectionOutgoing );   
        //TConnPref iPref(TConnPref::EConnPrefCommDb);
        iConnection.Start(iPref, iStatus);
        SetActive();
        TInt userResponse;
        ShowProgressNoteL( userResponse );
    } 


// ----------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::ShowVibrationNoteL
//
// Progress Note
// ----------------------------------------------------------------------------

TBool CDiagDataNetworkTestPDPEngine::ShowProgressNoteL(TInt aUserResponse)
    {
	if ( iWaitDialog )
	{
	delete iWaitDialog;
	iWaitDialog = NULL;
	}


	// WAIT NOTE DIALOG
	iWaitDialog = new (ELeave) CAknWaitDialog(
							(REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), 
							ETrue);

	iWaitDialog->PrepareLC(R_DATANW_PLUGIN_WAIT_NOTE);
	iWaitDialog->SetCallback(this);
	TBool isDialogDismissedByUser = iWaitDialog->RunLD();
	idialogOn = ETrue;
	return isDialogDismissedByUser;
    }
    
    
//*********************************************************
//*********************************************************
// CallBack from AKnWaitDialog when Cancel is pressed
//*********************************************************
//*********************************************************

void CDiagDataNetworkTestPDPEngine::DialogDismissedL( TInt aButtonId )
{
	LOGSTRING2("CDiagDataNetworkTestPDPEngine::DialogDismissedL() = %d",aButtonId);

	 
	if(aButtonId == EDataNetworkTestCancel)
	{
	
	
	if (idialogOn)
		{
		iConnection.Stop(RConnection::EStopAuthoritative);
	//	delete iWaitDilaog;
	//	iWaitDialog = NULL;
		idialogOn = EFalse; 
		idialogDismissed = ETrue;
		}
		
	 TInt   cancelButtonId;
     TBool  cancelResult = EFalse;
     iPrevState = iEngineState;
     SetStateL(EStateWaitDialogOn);
	 cancelResult= iInterface.DisplayDialog(cancelButtonId);
	 
	 iWaitDialog = NULL;
	 
	 if ( !cancelResult )
	 {
	 SetStateL(iPrevState);
	 return;
	 }
	 
	if(cancelButtonId == -2)
		{
		
		SetStateL(EStateCancelTest);
		TRequestStatus* status = &iStatus;
		// Complete own request
		User::RequestComplete( status, KErrNone );
		SetActive();
		
		//ReportResultToPluginL( CDiagResultsDatabaseItem::ECancelled );
		//HandleOngoingConnectionRequestL();
		
		}
     else
		{	 
		SetStateL(EStateDialogDismissed);
		TRequestStatus* status = &iStatus;
		// since this is a Cancel to start the test again
		// lets start believing that the DialogDismiss Cancel was never pressed
		idialogDismissed = EFalse;
		// Complete own request
		User::RequestComplete( status, KErrNone );
		SetActive();
		}
	 }
}
// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::RunL()
//
// Concrete implementation of CActive::DoCancel(). This function handles the asynchronous events.
// ---------------------------------------------------------------------------     
void CDiagDataNetworkTestPDPEngine::RunL()
    {
    switch(iEngineState)
        {   
        case EStateWaitForConnectionCompletion:
            {
            HandleOngoingConnectionRequestL();
            }
            break;
        case EStateWaitDialogOn:
        	break;
        case EStateCancelTest:
        	ReportResultToPluginL( CDiagResultsDatabaseItem::ECancelled );
        	break;
        case EStateDialogDismissed:
        	EstablishPDPContextL();
        	break;
        default:
            break;
        };      
    LOGSTRING("CDiagDataNetworkTestPDPEngine::RunL()");  
    }   
    
// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::DoCancel()
//
// This function handles cancel request from CActive
// ---------------------------------------------------------------------------     
void CDiagDataNetworkTestPDPEngine::DoCancel()
    {
    if( iNetRegStatusWatcher )
        {        
        iNetRegStatusWatcher->StopObserver();
        }
    if (iEngineState != EStateCancelTest)    
    iConnection.Stop();
    }
    
  
// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPDPEngine::HandleOngoingConnectionRequestL()
//
// This function handles ongoing asynchronous connection request and issues
// new request if required.
// ---------------------------------------------------------------------------     
void CDiagDataNetworkTestPDPEngine::HandleOngoingConnectionRequestL()
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::HandleOngoingConnectionRequestL()")
    
    if( iStatus == KErrNone || iStatus == KErrAlreadyExists )
        {        
        // connection established, report SUCCESS to the plugin        
        LOGSTRING("Connection established")
        LOGSTRING2("current IAP id = %d", iIAPiddefault)
        StartLogEraseOperationL( ETrue ); // ETrue means the test has Passed
        }
    else
        ReportResultToPluginL( EFalse );
      
    }    
    
// ----------------------------------------------------------------------------
// This function starts the log event erase process
// ----------------------------------------------------------------------------    
void CDiagDataNetworkTestPDPEngine::StartLogEraseOperationL(TBool aTestResult)
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::StartLogEraseOperationL()")

    iTestResult = aTestResult;
    SetStateL( EStateWaitForLogErase );
    
    TUid PacketEventType = { KLogPacketDataEventType };
    
    delete iLogEventEraser;
    iLogEventEraser = NULL;
    iLogEventEraser = CDiagLogEventEraser::NewL(*this);
    
    CLogFilter& LogFilter = iLogEventEraser->Filter();
    LogFilter.SetEventType(PacketEventType);
    LogFilter.SetStartTime(iStartTime);
    
    // start the log erasing process
    iLogEventEraser->StartAsyncEraseLogsL();
    }
    
// ----------------------------------------------------------------------------    
// This callback interface is used to determine if the log event is to be 
// deleted or not
// ----------------------------------------------------------------------------
TBool CDiagDataNetworkTestPDPEngine::IsEventToBeDeleted( const CLogEvent& aEvent )
    {
    LOGSTRING("CDiagDataNetworkTestPDPEngine::IsEventToBeDeleted()")
    
    TBool isEntryDeleted = EFalse;
    
    TBuf<KCommsDbSvrMaxFieldLength> eventAPNName;
    eventAPNName.Zero();
    eventAPNName.Copy(aEvent.RemoteParty()); 
    
    LOGSTRING("Log event APN name = ")  
    LOGTEXT(eventAPNName)  
    
    if( (eventAPNName.Compare( iCenrepPrimaryAPN )==0) || 
        (eventAPNName.Compare( iCenrepSecondaryAPN )==0) )    
        {
        // APN is matched, delete the log event entry
        isEntryDeleted = ETrue;
                 
        }

    LOGSTRING2("Log event deleted = %d", isEntryDeleted )        
    return isEntryDeleted;      
    }


// ----------------------------------------------------------------------------    
// This callback interface is used to notify that log events are erased
// ----------------------------------------------------------------------------
void CDiagDataNetworkTestPDPEngine::CompleteEventEraseL( TInt /*aError*/)
    {
    ReportResultToPluginL(iTestResult);
    }


// ---------------------------------------------------------------------------
// This function is sets the state of the engine and also reports the progress
// step
// --------------------------------------------------------------------------- 
void CDiagDataNetworkTestPDPEngine::SetStateL(TPDPEngineState aState)
    {
    LOGSTRING2("CDiagDataNetworkTestPDPEngine::SetStateL(), aState = %d", aState)
    
    iEngineState = aState;
    iInterface.ReportTestProgL( iEngineState );
    }

// ---------------------------------------------------------------------------
// This function is used to report the result of the test back to the plug-in
// class
// --------------------------------------------------------------------------- 
void CDiagDataNetworkTestPDPEngine::ReportResultToPluginL(TBool aResult)
    {
    LOGSTRING2("CDiagDataNetworkTestPDPEngine::ReportResultToPluginL(), aResult = %d", aResult)
    // Remove the waitDialog
    if(idialogOn)
    iWaitDialog->ProcessFinishedL(); 
    
    // Dialog dismissed by cancel
    if (idialogDismissed)
    {
    	iInterface.ReportTestResultL( CDiagResultsDatabaseItem::ESkipped );
    	return;
    }
    if(aResult)
        {
        iInterface.ReportTestResultL( CDiagResultsDatabaseItem::ESuccess );
        }        
    else
        {
        iInterface.ReportTestResultL( CDiagResultsDatabaseItem::EFailed ); 
        }               
    }


// End of File

