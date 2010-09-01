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
* Description:  This is the class module for the Diag Headset Cable Test Engine
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagheadsetcableengine.h"

#include <StringLoader.h>                           // StringLoader
#include <HeadsetStatus.h>                          // CHeadsetStatus
#include <aknmessagequerydialog.h>                  // CAknMessageQueryDialog
#include <DiagFrameworkDebug.h>                     // debug macros
#include <devdiagheadsetcabletestpluginrsc.rsg>        // resources
#include <DiagPluginWaitingDialogWrapper.h>         // CDiagPluginWaitingDialogWrapper

#include "diagheadsetcabletestplugin.hrh"           // command ids

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
//
CDiagHeadsetCableTestEngine* CDiagHeadsetCableTestEngine::NewL(
                            MDiagHeadsetTestCallBackInterface& aInterface,
                            TBool aIsSinglePluginExecution)
    {
    LOGSTRING( "CDiagHeadsetCableTestEngine instance created" )
    CDiagHeadsetCableTestEngine* self = new (ELeave)
            CDiagHeadsetCableTestEngine(aInterface, aIsSinglePluginExecution);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
    
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CDiagHeadsetCableTestEngine::CDiagHeadsetCableTestEngine(
                            MDiagHeadsetTestCallBackInterface& aInterface,
                            TBool aIsSinglePluginExecution)
    :   CActive( EPriorityStandard ),
        iInterface(aInterface),
        iWaitingDialogWrapper( NULL ),
        iIsSinglePluginExecution(aIsSinglePluginExecution)
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CDiagHeadsetCableTestEngine::~CDiagHeadsetCableTestEngine()
    {
    LOGSTRING( "CDiagHeadsetCableTestEngine instance destroyed" )
    Cancel();
    
    delete iWaitingDialogWrapper; //  deleting will dismiss active dialog
    iWaitingDialogWrapper = NULL;
    }
    
// ---------------------------------------------------------------------------
// Symbian OS second phase constructor
// ---------------------------------------------------------------------------    
void CDiagHeadsetCableTestEngine::ConstructL()
    {
    // nothting needed here
    }

// ----------------------------------------------------------------------------
// This function starts the Headset cable test
// ----------------------------------------------------------------------------
void CDiagHeadsetCableTestEngine::RunHeadsetCableTestL()
    {
    TInt userResponse;
    if ( !ConnectAndPressOkL( userResponse ) )
        {
        // "Connect Headset and Press OK" dialog was dismisses internally by framework
        // return immediately
        LOGSTRING( "CDiagHeadsetCableTestEngine : Connect Headset and press ok dialog dismissed internally" )
        return;
        }

    switch( userResponse )
        {
            // User select OK for "Connect Headset and Press OK" dialog
            case EAknSoftkeyOk:
                {
                //TInt keySelect;
                // check if headset cable is connected
                if( CheckIfHeadsetDetectedL() )
                    {
                    // Headset cable is detected.
                    LOGSTRING( "CDiagHeadsetCableTestEngine : Headset detected" )
                //    if ( !RemoveAllCablesL( keySelect ) )
                //        {
                //        LOGSTRING( "CDiagHeadsetCableTestEngine : Remove All Cable dialog was dismissed internally pressed" )
                //        return;
                //        }

                //    if ( keySelect != EAknSoftkeyOk )
                //        {
                //        LOGSTRING( "CDiagHeadsetCableTestEngine : EndKey has pressed" )
                //        return;	
                //        }  
                    LOGSTRING( "CDiagHeadsetCableTestEngine : Report result Success" )    
                    ReportResultToPluginL(CDiagResultsDatabaseItem::ESuccess);
                    }
                else
                    { 
                    // Headset cable is not detected
                    if ( !AskIfRetryL( userResponse ) )
                        {
                        // Retry Back dialog was dismissed internally by framework internally
                        LOGSTRING( "CDiagHeadsetCableTestEngine : Ask for retry dialog dismissed internally" )
                        return;
                        }

                    //if( userResponse == EAknSoftkeyOk )  // ADO & Platformization Changes
                    if( userResponse == EHeadsetTestConnectHeadsetNo )
                        {
                        // User selects Ok to confirm, Test fails
                        ReportResultToPluginL(CDiagResultsDatabaseItem::EFailed);
                        }
                   // else if( userResponse == EHeadsetTestConnectHeadsetBack ) // ADO & Platformization Changes
                   	  else if( userResponse == EHeadsetTestConnectHeadsetYes )
                        {
                        // User selectes Back to retry the test
                        TRequestStatus* status = &iStatus;
                        SetActive();
                        User::RequestComplete( status, KErrNone );
                        }
                    else
                        {
                        // User cancels the test using Canceld end key, return immediately
                        return;
                        }
                    }
                }
                break;
            // User select CANCEL softkey for "Connect Headset and Press OK" dialog
            case EHeadsetTestConnectHeadsetSkip:
            case EHeadsetTestConnectHeadsetCancel:
                {
                    if(!iIsSinglePluginExecution)
                        {
                         if( iInterface.AskCancelExecutionL(userResponse) )
                                {
                                switch(userResponse)
                                    {
                                        case EAknSoftkeyOk:
                                            {
                                            //CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                                            LOGSTRING( "CDiagHeadsetCableTestEngine User cancelled the test" )
                                            return;
                                            }
                                            
                                        default:
                                            {
                                            // currently no way distinguish between Cancel and No key
                                            
                                            // user selected "No" to cancel
                                            // do a self-trigger to start again
                                            TRequestStatus* status = &iStatus;
                                            SetActive();
                                            User::RequestComplete( status, KErrNone );
                                            
                                            // this trigger for Cancel key is cancelled by CActive::Cancel() in destructor
                                            }
                                            break;
                                    }
                                }
                            else
                                {
                                // though cancel dialog should not be dismissed internally by the framework
                                // this is just a safeguard to make sure we dont do anything
                                LOGSTRING( "CDiagHeadsetCableTestEngine Cancel dialog was dismissed internally" )
                                return;
                                }
                        }
                    else
                        {
                            ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
                            return;
                        }
                }
                break;
            
            // User select CANCEL (RED End KEY) for "Connect Headset and Press OK" dialog
            default:
                {
                LOGSTRING( "CDiagHeadsetCableTestEngine Connect Headset and press OK dialog was cancelled by end key." )
                }
                break;
        };

    }
    
// ----------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::CheckIfHeadsetDetectedL
//
// This function reports the result to plugin
// ----------------------------------------------------------------------------
TBool CDiagHeadsetCableTestEngine::CheckIfHeadsetDetectedL()
    {
    TBool retval = EFalse;
    TBool isConnected = EFalse;
    CHeadsetStatus* aHeadsetStatus = CHeadsetStatus::NewL();

    CleanupStack::PushL( aHeadsetStatus );
    TInt err( aHeadsetStatus->HeadsetConnectedL( isConnected ) ); 
    CleanupStack::PopAndDestroy( aHeadsetStatus );
    
    if(err == KErrNone)
        {        
        if( isConnected )
            {
            LOGSTRING( "CDiagHeadsetCableTestEngine Headset cable is connected" )
            retval = ETrue; // headset is connected
            }
        else
            {
            LOGSTRING( "CDiagHeadsetCableTestEngine Headset cable is not connected" )
            retval = EFalse; // headset is not connected
            }
        }
    else
        {
        LOGSTRING( "CDiagHeadsetCableTestEngine some error occurred in detecting Headset cable" )
        retval = EFalse; // some error occurred
        }    
    
    return retval;            
    }   
    
    

// ----------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::ReportResultToPluginL
//
// This function reports the result to plugin
// ----------------------------------------------------------------------------       
void CDiagHeadsetCableTestEngine::ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult)
    {
    iInterface.ReportTestResultL( aResult );
    }
    
// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::ConnectAndPressOkL
// This function shows the query dialog "Connect your Headset charger. Press OK when ready."
// ---------------------------------------------------------------------------------------    
TBool CDiagHeadsetCableTestEngine::ConnectAndPressOkL( TInt& aUserResponse )
    {
    ASSERT( iWaitingDialogWrapper == NULL );

    CAknQueryDialog* dialog = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
    CleanupStack::PushL( dialog );
    dialog->PrepareLC( R_DIAG_HEADSET_CABLE_TEST_CONNECT_AND_PRESS_OK );
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
// CDiagHeadsetCableTestEngine::RemoveAllCables
// This function shows the query dialog : Please remove all cables
// --------------------------------------------------------------------------------------- 
TBool CDiagHeadsetCableTestEngine::RemoveAllCablesL( TInt& aUserResponse )
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
    
// ----------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::AskIfRetryL
// This function shows the query dialog "No cable detected. Press OK to continue 
// tests or press Back to retry this test." 
// ----------------------------------------------------------------------------
TBool CDiagHeadsetCableTestEngine::AskIfRetryL( TInt& aUserResponse )
    {
    return DisplayQueryDialogWithHeaderL( R_DIAG_HEADSET_CABLE_TEST_NO_CABLE_DETECTED,
                                aUserResponse );
    }   
    
// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::DisplayQueryDialogWithHeaderL
// This function shows the query dialog with header. It returns ETrue, 
// if the dialog is dismissed by user. If the dialog was dismissed because of time-out, 
// it returns EFalse. The user response (softkey) is returned in the reference 
// argument aUserResponse
// --------------------------------------------------------------------------------------- 
TBool CDiagHeadsetCableTestEngine::DisplayQueryDialogWithHeaderL( 
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
void CDiagHeadsetCableTestEngine::RunL()
    {
    if(iStatus == KErrNone)
        {
        RunHeadsetCableTestL();
        }
    }

// ---------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagHeadsetCableTestEngine::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagHeadsetCableTestEngine::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------------------
// CDiagHeadsetCableTestEngine::DoCancel
// Handles cancel request
// ---------------------------------------------------------------------------------------    
void CDiagHeadsetCableTestEngine::DoCancel()
    {
    // no implementation needed
    }
    
// End of File
