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
* Description:  This is the class module for the Diag USB Cable Test Engine
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagusbcabletestengine.h"

// System includes
#include <DiagFrameworkDebug.h>                 // LOGSTRING
#include <DiagPluginWaitingDialogWrapper.h>     // CDiagPluginWaitingDialogWrapper
#include <devdiagusbcabletestpluginrsc.rsg>        // resources
#include <UsbWatcherInternalCRKeys.h>           // KUsbWatcherChangeOnConnectionSetting
#include <aknmessagequerydialog.h>              // CAknQueryDialog
#include <centralrepository.h>                  // CRepository


// User includes
#include "diagusbcabletestplugin.hrh"           // command ids
#include "avkon.hrh" // ADO & Platformization Changes

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
// CDiagUSBCableTestEngine::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------   
CDiagUSBCableTestEngine* CDiagUSBCableTestEngine::NewL(
    MDiagUSBTestCallBackInterface& aInterface,
    TBool aIsSinglePluginExecution)
    {
    LOGSTRING( "CDiagUSBCableTestEngine::NewL()" )
    
    CDiagUSBCableTestEngine* self = 
        new (ELeave)CDiagUSBCableTestEngine(aInterface, aIsSinglePluginExecution);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
    
// ----------------------------------------------------------------------------
// CDiagUSBCableTestEngine::CDiagUSBCableTestEngine()
//
// Constructor
// ----------------------------------------------------------------------------
//    
CDiagUSBCableTestEngine::CDiagUSBCableTestEngine(
    MDiagUSBTestCallBackInterface& aInterface,
    TBool aIsSinglePluginExecution)
    :   CActive( EPriorityStandard ),
        iInterface(aInterface),
        iWaitingDialogWrapper( NULL ),
        iIsSinglePluginExecution(aIsSinglePluginExecution)
    {
    LOGSTRING( "CDiagUSBCableTestEngine::CDiagUSBCableTestEngine()" )
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CDiagUSBCableTestEngine::~CDiagUSBCableTestEngine
//
// Destructor
// ----------------------------------------------------------------------------       
CDiagUSBCableTestEngine::~CDiagUSBCableTestEngine()
    {
    LOGSTRING( "CDiagUSBCableTestEngine::~CDiagUSBCableTestEngine()" )
    
    Cancel();
    
    if(iCenRep)
        {
        // if the cenrep was modified by us , restore the settings
        if(iCenrepModified)
            {
            TInt error = iCenRep->Set( KUsbWatcherChangeOnConnectionSetting, 1);
            iCenrepModified = EFalse;
            }
                
        delete iCenRep;
        iCenRep = NULL;
        }
    
    if(iWaitingDialogWrapper)
        {
        delete iWaitingDialogWrapper; //  deleting will dismiss active dialog
        iWaitingDialogWrapper = NULL;        
        }

    }
    
// ---------------------------------------------------------------------------
// CDiagUSBCableTestEngine::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------    
void CDiagUSBCableTestEngine::ConstructL()
    {
    LOGSTRING( "CDiagUSBCableTestEngine::ConstructL()" )
    
    // check if USB's "Ask on Connection" is on
    // if ON, set if OFF, remember to turn in ON again at the end of test
        
    TInt retval;
    iCenRep = CRepository::NewL( KCRUidUsbWatcher );
    iCenrepModified = EFalse;
    iCenRep->Get( KUsbWatcherChangeOnConnectionSetting, retval);
    if(retval)
        {
         //if setting is 1,set it to 0, and remember to reset it back when application exits.
        iCenrepModified = ETrue;
        TInt error = iCenRep->Set( KUsbWatcherChangeOnConnectionSetting, 0);
        }        
    }
    

// ----------------------------------------------------------------------------
// CDiagUSBCableTestEngine::RunUSBCableTestL
//
// This function starts the USB cable test
// ----------------------------------------------------------------------------           
void CDiagUSBCableTestEngine::RunUSBCableTestL()
    {
    TInt userResponse;

    if ( !ConnectAndPressOkL( userResponse ) )
        {
        //"Connect and Press OK" dialog was dismissed internally by the framework
        // return immediately
        LOGSTRING( "CDiagUSBCableTestEngine Connect USB and confirm dialog was dismissed internally" )
        return;
        }

    switch( userResponse )
        {
        // user selects OK for "Connect and Press OK" dialog
        case EAknSoftkeyOk:
            {
            //TInt keySelect;
            // check if cable is connected
            if( CheckIfUSBCableDetectedL() )
                {
                // USB cable is detected
                /*
                if ( !RemoveAllCablesL( keySelect ) )
                    {
                    LOGSTRING( "CDiagUSBCableTestEngine : Remove All Cable dialog was dismissed internally pressed" )
                    return;
                    }

                if ( keySelect != EAknSoftkeyOk )
                    {
                    LOGSTRING( "CDiagUSBCableTestEngine : EndKey has pressed" )
                    return;	
                    }
                 */ // ADO & Platformization Changes   

                LOGSTRING( "CDiagUSBCableTestEngine : Report result Success" )
                ReportResultToPluginL( CDiagResultsDatabaseItem::ESuccess );
                }
            else
                { 
                // no cable detected
                if ( !AskIfRetryL( userResponse ) )
                    {
                    // Retry dialog was dimissed internally by the framework
                    // return immediately
                    LOGSTRING( "CDiagUSBCableTestEngine Ask for retry dialog was dismissed internally" )
                    return;
                    }


                
               if( userResponse == EAknSoftkeyYes )
                    {
                    // User selects Back to retry the test
                    // will come here in case of Back or Cancel key
                    TRequestStatus* status = &iStatus;
                    SetActive();
                    User::RequestComplete( status, KErrNone );
                    }
                else
                    {
                    //Retry dialog was dismissed by Cancel / End key
                    ReportResultToPluginL(CDiagResultsDatabaseItem::EFailed);
                    LOGSTRING( "CDiagUSBCableTestEngine Retry dialog was dismissed by Cancel / End key" )
                    return;
                    }
                }
            }
            break;
        
        // user selects CANCEL softkey for "Connect and Press OK" dialog
        case EUsbTestConnectUSBSkip:
        case EUsbTestConnectUSBCancel:
	            {
    	        LOGSTRING( "CDiagUSBCableTestEngine User Cancelled the test execution" )
    	        if(iIsSinglePluginExecution)
    	            {
    	            LOGSTRING( "CDiagUSBCableTestEngine User skipped the test execution" )
    	            ReportResultToPluginL(CDiagResultsDatabaseItem::ESkipped);
    	            }
    	        else
    	            {
    	            TInt aOption = 1;
                    if(iInterface.AskCancelExecutionL(userResponse,aOption))
                        {
                        if(userResponse)
                            {
                            LOGSTRING( "CDiagUSBCableTestEngine Test cancelled by user" )
                            return;
                            }
                        else
                            {
                            // currently no way to distinguish between "No" and "Cancel" key
                            LOGSTRING( "CDiagUSBCableTestEngine : User selected No to Cancel dialog" )
                            // do a self-trigger to start again
                            TRequestStatus* status = &iStatus;
                            SetActive();
                            User::RequestComplete( status, KErrNone );
                            }
                        }
                    else
                        {
                        // though the cancel dialog should not be dismissed internally, this is
                        // just a safeguard to be sure we dont do anything
                        LOGSTRING( "CDiagUSBCableTestEngine Test cancel dialog dismissed internally" )
                        break;
                        }
    	            }

	            }
            break;
        
        // user selects CANCEL End Key for "Connect and Press OK" dialog
        default:
            {
            LOGSTRING( "CDiagUSBCableTestEngine Connect USB and confirm dialog was cancelled by the end key" )
            }
            break;
        };
    }
    
// ----------------------------------------------------------------------------
// CDiagUSBCableTestEngine::CheckIfUSBCableDetectedL
//
// This function reports the result to plugin
// ----------------------------------------------------------------------------
TBool CDiagUSBCableTestEngine::CheckIfUSBCableDetectedL()
    {
    TBool bCableDetected = EFalse;
    TInt err = iUsbManager.Connect();
    if(err == KErrNone)
        {
        // get the USB service state
        TUsbServiceState UsbServiceState;
        err = iUsbManager.GetServiceState(UsbServiceState);
        LOGSTRING2( "CDiagUSBCableTestEngine::USB service state: %d", UsbServiceState)
        
        if(err == KErrNone)
            {
            switch(UsbServiceState)
                {
                    case EUsbServiceStarting:
                    case EUsbServiceStarted:
                        {            
                        // get the USB device state
                        TUsbDeviceState UsbDeviceState;
                        err = iUsbManager.GetDeviceState(UsbDeviceState);
                        LOGSTRING2( "CDiagUSBCableTestEngine::USB Device state: %d", UsbDeviceState)
                        if(err == KErrNone)
                            {
                            if(UsbDeviceState > EUsbDeviceStatePowered)
                                {
                                bCableDetected = ETrue;
                                }
                            else
                                {
                                bCableDetected = EFalse;
                                }
                            }
                        else
                            {
                            LOGSTRING( "CDiagUSBCableTestEngine Error in obtaining USB Device state")
                            bCableDetected = EFalse;
                            }
                        }
                        break;
                        
                    case EUsbServiceIdle:
                    case EUsbServiceStopping:
                    case EUsbServiceFatalError:
                    default:
                        {
                        bCableDetected = EFalse;
                        }
                        break;
                };
            }
        else
            {
            LOGSTRING( "CDiagUSBCableTestEngine Error in obtaining USB Service state")
            bCableDetected = EFalse;
            }
        
        iUsbManager.Close();
        }
    else
        {
         LOGSTRING( "CDiagUSBCableTestEngine::iUsbManager.Connect() failed" )
        bCableDetected = EFalse;
        }
    
    return bCableDetected;
    }  

// ----------------------------------------------------------------------------
// CDiagUSBCableTestEngine::ReportResultToPluginL
//
// This function reports the result to plugin
// ----------------------------------------------------------------------------       
void CDiagUSBCableTestEngine::ReportResultToPluginL(
                                            CDiagResultsDatabaseItem::TResult aResult)
    {
    iInterface.ReportTestResultL(aResult);
    }

// --------------------------------------------------------------------------------------------------------------
// CDiagUSBCableTestPlugin::ConnectAndPressOkL
// This function shows the query dialog : "Connect an USB data cable from phone to the PC. Press OK when ready."
// --------------------------------------------------------------------------------------------------------------    
TBool CDiagUSBCableTestEngine::ConnectAndPressOkL( TInt& aUserResponse )
    {
    ASSERT( iWaitingDialogWrapper == NULL );

    CAknQueryDialog* dialog = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dialog );
    dialog->PrepareLC( R_DIAG_USB_CABLE_TEST_CONNECT_AND_PRESS_OK );
    if( iIsSinglePluginExecution )
        {
        CEikButtonGroupContainer& cba = dialog->ButtonGroupContainer();
        cba.SetCommandSetL( R_CBA_SINGLE_EXECUTION );        
        }        
    CleanupStack::Pop( dialog );

    // dialog ownership to bepassed to iWaitingDialogWrapper
    // CDiagPluginWaitingDialogWrapper::NewL will push dialog into clean up stack
    iWaitingDialogWrapper = CDiagPluginWaitingDialogWrapper::NewL( dialog );
    dialog = NULL;

    TBool isDialogDismissedByUser = iWaitingDialogWrapper->RunLD( aUserResponse );
    if ( isDialogDismissedByUser )
        {
        // local variable can be accessed only if it was returned due to
        // user response. iWaitingDialogWrapper self destructs.
        iWaitingDialogWrapper = NULL;
        }

    return isDialogDismissedByUser;    
    }

// ---------------------------------------------------------------------------------------
// CDiagUSBCableTestPlugin::RemoveAllCables
// This function shows the query dialog : Please remove all cables
// --------------------------------------------------------------------------------------- 
TBool CDiagUSBCableTestEngine::RemoveAllCablesL( TInt& aUserResponse )
    {
    ASSERT( iWaitingDialogWrapper == NULL );

    CAknQueryDialog* dialog = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );

    dialog->PrepareLC( R_DIAG_CABLE_TEST_REMOVE_ALL_CABLES );

    // dialog ownership to bepassed to iWaitingDialogWrapper
    // CDiagPluginWaitingDialogWrapper::NewL will push dialog into clean up stack
    iWaitingDialogWrapper = CDiagPluginWaitingDialogWrapper::NewL( dialog );
    dialog = NULL;

    TBool isDialogDismissedByUser = iWaitingDialogWrapper->RunLD( aUserResponse );
    if ( isDialogDismissedByUser )
        {
        // local variable can be accessed only if it was returned due to
        // user response. iWaitingDialogWrapper self destructs.
        iWaitingDialogWrapper = NULL;
        }

    return isDialogDismissedByUser;
    }     
    
// --------------------------------------------------------------------------------------------------------------------------
// CDiagUSBCableTestPlugin::AskIfRetryL
// This function shows the query dialog : "USB data cable not detected. Press OK to continue or press Back to retry this test."
// ---------------------------------------------------------------------------------------------------------------------------
TBool CDiagUSBCableTestEngine::AskIfRetryL( TInt& aUserResponse )
    {    
    return DisplayQueryDialogWithHeaderL( R_DIAG_USB_CABLE_TEST_NO_CABLE_DETECTED,
                                aUserResponse );
    }

// ---------------------------------------------------------------------------------------
// CDiagUSBCableTestPlugin::DisplayQueryDialogWithHeaderL
// This function shows the query dialog. It returns ETrue, if the dialog is dismissed by user
// if the dialog was dismissed because of time-out, it returns EFalse. The user response (softkey)
// is returned in the reference argument aUserResponse
// ---------------------------------------------------------------------------------------    
TBool CDiagUSBCableTestEngine::DisplayQueryDialogWithHeaderL( 
        TInt aDialogResourceId,
        TInt& aUserResponse )
    {
    ASSERT( iWaitingDialogWrapper == NULL );

    CAknMessageQueryDialog * dialog = new ( ELeave ) CAknMessageQueryDialog ( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dialog );
    dialog->PrepareLC( aDialogResourceId );
    CleanupStack::Pop( dialog );

    // dialog ownership to bepassed to iWaitingDialogWrapper
    // CDiagPluginWaitingDialogWrapper::NewL will push dialog into clean up stack
    iWaitingDialogWrapper = CDiagPluginWaitingDialogWrapper::NewL( dialog );
    dialog = NULL;

    TBool isDialogDismissedByUser = iWaitingDialogWrapper->RunLD( aUserResponse );
    if ( isDialogDismissedByUser )
        {
        // local variable can be accessed only if it was returned due to
        // user response. iWaitingDialogWrapper self destructs.
        iWaitingDialogWrapper = NULL;
        }

    return isDialogDismissedByUser;
    }

// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::RunL
// RunL() function is used in this plug-in to handle internal trigger
// ---------------------------------------------------------------------------------------    
void CDiagUSBCableTestEngine::RunL()
    {
    if(iStatus == KErrNone)
        {
        RunUSBCableTestL();
        }
    }

// ---------------------------------------------------------------------------
// CDiagUSBCableTestEngine::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagUSBCableTestEngine::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagUSBCableTestEngine::RunError( %d )", aError )    
		return KErrNone;
	}
	  
// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::DoCancel
// Handles cancel request
// ---------------------------------------------------------------------------------------    
void CDiagUSBCableTestEngine::DoCancel()
    {
    // no implementation needed
    }
    
// End of File

