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
* Description:  This is the implementation class for the engine of Battery charger cable
*                 test plug-in
*
*/


// INCLUDE FILES
// Class declaration
#include "diagbatterychargercableengine.h"

// System includes
#include <DiagFrameworkDebug.h>                     // Debugging Macros
#include <StringLoader.h>                           // StringLoader
#include <e32property.h>                            // RProperty
#include <hwrmpowerstatesdkpskeys.h>                // P&S keys of HW Resource Manager
#include <DiagPluginWaitingDialogWrapper.h>         // CDiagPluginWaitingDialogWrapper
#include <aknmessagequerydialog.h>                  // CAknQueryDialog
#include <devdiagbatterychargercabletestpluginrsc.rsg> // Resource definitions


// User includes
#include "diagbatterychargercabletestplugin.hrh"    // command ids
#include "diagbatterychargercallbackinterface.h"    //MDiagBatteryChargerTestCallBackInterface


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
// CDiagBatteryChargerCableTestEngine::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------   
CDiagBatteryChargerCableTestEngine* CDiagBatteryChargerCableTestEngine::NewL(
    MDiagBatteryChargerTestCallBackInterface& aInterface,
    TBool aIsSinglePluginExecution)
    {
    CDiagBatteryChargerCableTestEngine* self = 
        new (ELeave)CDiagBatteryChargerCableTestEngine(
                                    aInterface, aIsSinglePluginExecution);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
    
// ----------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::CDiagBatteryChargerCableTestEngine()
//
// Constructor
// ----------------------------------------------------------------------------
//    
CDiagBatteryChargerCableTestEngine::CDiagBatteryChargerCableTestEngine(
    MDiagBatteryChargerTestCallBackInterface& aInterface,
    TBool aIsSinglePluginExecution)
    :   CActive( EPriorityStandard ),
        iInterface(aInterface),
        iWaitingDialogWrapper( NULL ),
        iIsSinglePluginExecution(aIsSinglePluginExecution)
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::~CDiagBatteryChargerCableTestEngine
//
// Destructor
// ----------------------------------------------------------------------------
CDiagBatteryChargerCableTestEngine::~CDiagBatteryChargerCableTestEngine()
    {
    Cancel();
    
    delete iWaitingDialogWrapper; //  deleting will dismiss active dialog
    iWaitingDialogWrapper = NULL;
    }
    
// ---------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------    
void CDiagBatteryChargerCableTestEngine::ConstructL()
    {
    // nothing to do here
    }
    

// ----------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::RunBatteryChargerTestL
//
// This function starts the Battery charger cable test
// ----------------------------------------------------------------------------
void CDiagBatteryChargerCableTestEngine::RunBatteryChargerTestL()
    {    
    TInt userResponse;
    
    if ( !ConnectAndPressOkL( userResponse ) )
        {
        // Return immediately. "this" object may have been deleted.
        LOGSTRING( "CDiagBatteryChargerCableTestEngine : Connect Battery charger and press ok dialog was dismissed internally" )
        return;
        }

    switch( userResponse )
        {
        // User selects OK "Connect charger & press ok" dialog
        case EAknSoftkeyOk: 
            {            
            //TInt keySelect;
            // check if headset cable is connected
            if( CheckIfChargerDetected() )
                {
                // battery charger is detected
                LOGSTRING( "CDiagBatteryChargerCableTestEngine : Headset detected" );
                /*
                if ( !RemoveAllCablesL( keySelect ) )
                    {
                    LOGSTRING( "CDiagBatteryChargerCableTestEngine : Remove All Cable dialog was dismissed internally pressed" )
                    return;
                    }

                if ( keySelect != EAknSoftkeyOk )
                    {
                    LOGSTRING( "CDiagBatteryChargerCableTestEngine : EndKey has pressed" )
                    return;	
                    }
                */ // ADO & Platformization Changes  

                LOGSTRING( "CDiagBatteryChargerCableTestEngine : Report result Success" )    
                ReportResultToPluginL( CDiagResultsDatabaseItem::ESuccess );
                }
            else
                { 
                // no cable detected
                if ( !AskIfRetryL( userResponse ) )
                    {
                    // Ask for retry dialog dismissed by framework internally
                    // Return immediately
                    LOGSTRING( "CDiagBatteryChargerCableTestEngine Test Ask for retry dialog dismissed by framework internally" )
                    return;
                    }
                
                switch(userResponse)
                    {
                    
                    //case EAknSoftkeyOk: // ADO & Platformization Changes
                    	case EBatteryTestConnectBatteryChargerNo:
                        {
                        LOGSTRING( "CDiagBatteryChargerCableTestEngine : user selected OK" )
                        ReportResultToPluginL(CDiagResultsDatabaseItem::EFailed);
                        }
                        break;
                    //case EBatteryTestConnectBatteryChargerBack: //  ADO & Platformization Changes
                    	case EBatteryTestConnectBatteryChargerYes:
                        {
                        LOGSTRING( "CDiagBatteryChargerCableTestEngine : user selected Back" )
                        // user selected "Back" to retry
                        // do a self-trigger to start again
                        TRequestStatus* status = &iStatus;
                        SetActive();
                        User::RequestComplete( status, KErrNone );
                        }
                        break;
                    default:
                        {
                        // the dialog was dismissed by cancel key
                        LOGSTRING( "CDiagBatteryChargerCableTestEngine : user selected cancel key" )
                        }
                        break;
                    };
                }
            }
            break;
        
        // User selects SKIP  on "Connect charger & press ok" dialog
        case EBatteryTestConnectBatteryChargerCancel: 
        case EBatteryTestConnectBatteryChargerSkip:
            {
            if(iIsSinglePluginExecution)
                {
                LOGSTRING( "CDiagBatteryChargerCableTestEngine Test skipped by user" )
                ReportResultToPluginL(CDiagResultsDatabaseItem::ESkipped);  
                }
            else
                {
                if( iInterface.AskCancelExecutionL(userResponse) )
                    {
                    // cancel dialog was dismissed by user action
                    if(userResponse)
                        {
                        LOGSTRING( "CDiagBatteryChargerCableTestEngine Test cancelled by user" )
                        return;
                        }
                    else
                        {
                        // currently no way to distinguish between "No" and "Cancel" key
                        LOGSTRING( "CDiagBatteryChargerCableTestEngine : User selected No to Cancel dialog" )
                        // do a self-trigger to start again
                        TRequestStatus* status = &iStatus;
                        SetActive();
                        User::RequestComplete( status, KErrNone );
                        }
    
                    }
                else
                    {
                    // Though this should not happedn, Cancel All Test execution query dialog was dismissed internally by the framework,
                    // not by user action. Return immediately
                    LOGSTRING( "CDiagBatteryChargerCableTestEngine : Cancel query dialog was dismissed internally by the framework, not by user action." )
                    return;
                    }
                }
            }
            break;
        
        // User selectes CANCEL command (Red Key) on "Connect charger & press ok" dialog
        default:
            {
            // Do nothing
            LOGSTRING( "CDiagBatteryChargerCableTestEngine : Connect Battery charger and press ok dialog was dismissed by end key" )
            }
            break;
        };
    }
    
// ----------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::CheckIfChargerDetected
//
// This function detects the battery charger status
// ----------------------------------------------------------------------------
TBool CDiagBatteryChargerCableTestEngine::CheckIfChargerDetected()
    {
    TInt charger = -1, retval = EFalse;
    TInt error = RProperty::Get( KPSUidHWRMPowerState, KHWRMChargingStatus, charger ) ;
    
    if( error == KErrNone )
        {
        LOGSTRING2( "CDiagBatteryChargerCableTestEngine charging status = %d", charger )
        switch(charger)
            {                        
            case EChargingStatusCharging:           // Device is charging.
            case EChargingStatusAlmostComplete:     // Charging almost completed.  
            case EChargingStatusChargingComplete:   // Charging completed.  
            case EChargingStatusChargingContinued : // Charging continued after brief interruption. 
                {
                retval = ETrue;
                }
                break;
                
                
            case EChargingStatusError:        // Some error has occurred when charger is connected or charging.
            case EChargingStatusNotConnected: // Charger not connected/uninitialized. 
            case EChargingStatusNotCharging:  // Charger is connected, device not charging.
            default:
                {
                retval =  EFalse;
                }
                    
            }
        }
    else
        {
        LOGSTRING( "CDiagBatteryChargerCableTestEngine : Error in obtaining charger status ")
        retval =  EFalse;
        }
    
    return retval;
    }

// ----------------------------------------------------------------------------
// CDiagBatteryChargerCableTestEngine::ReportResultToPluginL
//
// This function reports the result to plugin
// ----------------------------------------------------------------------------
void CDiagBatteryChargerCableTestEngine::ReportResultToPluginL(
                                    CDiagResultsDatabaseItem::TResult aResult)
    {
    iInterface.ReportTestResultL(aResult);
    }


// ---------------------------------------------------------------------------------------
// CDiagBatteryChargerCableTestPlugin::ConnectAndPressOkL
// This function shows the query dialog : Connect your battery charger. Press OK when ready
// --------------------------------------------------------------------------------------- 
TBool CDiagBatteryChargerCableTestEngine::ConnectAndPressOkL( TInt& aUserResponse )
    {
    
    ASSERT( iWaitingDialogWrapper == NULL );

    CAknQueryDialog* dialog = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dialog );

    dialog->PrepareLC( R_DIAG_BATTERY_CHARGER_CABLE_TEST_CONNECT_AND_PRESS_OK );
    
    if( iIsSinglePluginExecution )
        {
        CEikButtonGroupContainer& cba = dialog->ButtonGroupContainer();
        cba.SetCommandSetL( R_CBA_CONNECT_AND_PRESS_OK_SINGLE_EXECUTION );        
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
// CDiagBatteryChargerCableTestPlugin::RemoveAllCables
// This function shows the query dialog : Please remove all cables
// --------------------------------------------------------------------------------------- 
TBool CDiagBatteryChargerCableTestEngine::RemoveAllCablesL( TInt& aUserResponse )
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
    
// ---------------------------------------------------------------------------------------
// CDiagBatteryChargerCableTestPlugin::AskIfRetryL
// This function shows the query dialog : No cable detected. Press OK to continue tests
// or press Back to retry this test.
// ---------------------------------------------------------------------------------------
TBool CDiagBatteryChargerCableTestEngine::AskIfRetryL( TInt& aUserResponse )
    {
    return DisplayQueryDialogWithHeaderL(
                                       R_DIAG_BATTERY_CHARGER_CABLE_TEST_NO_CABLE_DETECTED, 
                                       aUserResponse);                                
    }                  

   
// ---------------------------------------------------------------------------------------
// CDiagBatteryChargerCableTestPlugin::DisplayQueryDialogWithHeaderL
// This function shows the query dialog. It returns ETrue, if the dialog is dismissed by user
// if the dialog was dismissed because of time-out, it returns EFalse. 
// The user response (softkey) is returned in the reference argument aUserResponse
// ---------------------------------------------------------------------------------------    
    
TBool CDiagBatteryChargerCableTestEngine::DisplayQueryDialogWithHeaderL( 
        TInt aDialogResourceId,
        TInt& aUserResponse )
    {
    ASSERT( iWaitingDialogWrapper == NULL );
  
    CAknMessageQueryDialog * dialog = new ( ELeave ) 
									CAknMessageQueryDialog ( CAknQueryDialog::ENoTone );
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
void CDiagBatteryChargerCableTestEngine::RunL()
    {
    if(iStatus == KErrNone)
        {
        RunBatteryChargerTestL();
        }
    }

// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::DoCancel
// Handles cancel request
// ---------------------------------------------------------------------------------------    
void CDiagBatteryChargerCableTestEngine::DoCancel()
    {
    // no implementation needed
    }
    
// End of File

