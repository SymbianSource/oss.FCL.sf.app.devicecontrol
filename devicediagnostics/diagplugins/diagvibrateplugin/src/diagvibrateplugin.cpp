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
* Description:  Vibrate plugin interface implementation
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include <AknQueryDialog.h>
#include <aknmessagequerydialog.h>
#include <DiagFrameworkDebug.h>         
#include <DiagTestExecParam.h>      
#include <DiagTestObserver.h>       
#include <devdiagvibratepluginrsc.rsg>
#include <hal.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <MProEngEngine.h>
#include <MProEngProfile.h>
#include <MProEngToneSettings.h>
#include <ProEngFactory.h> 
#include <DiagEngineCommon.h>    
#include <DiagCommonDialog.h>    
#include <StringLoader.h>           
#include <centralrepository.h>

#include "diagvibrateplugin.h"
#include "diagvibrateplugin.hrh"
#include "diagvibratepluginengine.h"
#include "diagvibratepluginprivatecrkeys.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUid KDiagVibratePluginUid = { DIAG_VIBRATE_PLUGIN_UID };
static const TInt KProgressDialogIncrementSize = 10;
static const TInt KProgressDialogFinalValue = 100;
static const TInt KHundredthsInSecond = 100;

static const TInt KVibratePluginTotalStep = 1;

// MACROS
_LIT( KDiagVibratePluginResourceFileName, "z:DevDiagVibratePluginRsc.rsc" );

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
    enum TVibratePluginPanic
        {
        EPanicUnknownVibraState= 900,
        EPanicInitParamsNull,
        EPanicStateRepeated
        };

    _LIT( KVibratePluginErrorUnknownVibraStateText, "VibraStatusChanged: Unknown vibra state" );
    _LIT( KVibratePluginErrorInitParamsNullText, "NewL: InitParams NULL" );
    _LIT( KVibratePluginErrorStateRepeatedText, "State change not effective" );
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CDiagVibratePlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagVibratePlugin::NewL( TAny* aInitParams )
    {
    __ASSERT_DEBUG( aInitParams, User::Panic ( KVibratePluginErrorInitParamsNullText, 
        EPanicInitParamsNull ));

    CDiagPluginConstructionParam* param = NULL;
    param = static_cast<CDiagPluginConstructionParam*>( aInitParams );    

    CleanupStack::PushL( param );    
    CDiagVibratePlugin* self = new( ELeave ) CDiagVibratePlugin ( param );
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop( self ); // self
    return self;
    }


// ----------------------------------------------------------------------------
// CDiagVibratePlugin::~CDiagVibratePlugin
//
// Destructor
// ----------------------------------------------------------------------------
//
CDiagVibratePlugin::~CDiagVibratePlugin()
    {
    Cancel();
    delete iProgressDialog;
    delete iEngine;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagVibratePlugin::IsVisible()
// ---------------------------------------------------------------------------
//
TBool CDiagVibratePlugin::IsVisible() const
    {
    return ETrue;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagVibratePlugin::RunMode()
// ---------------------------------------------------------------------------
//
MDiagTestPlugin::TRunMode CDiagVibratePlugin::RunMode() const
    {
    return EInteractiveDialog;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagVibratePlugin::TotalSteps()
// ---------------------------------------------------------------------------
//
TUint CDiagVibratePlugin::TotalSteps() const
    {
    return KVibratePluginTotalStep;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagVibratePlugin::GetPluginNameL
// ---------------------------------------------------------------------------
//
HBufC* CDiagVibratePlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_MSG_TITLE_VIBRATE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL ( R_DIAG_MSG_INFO_VIBRATE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_TITLE_TEST_VIBRATE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_LST_TEST_VIBRATE );

        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_LST_VIBRATE );

        default:
            LOGSTRING2( "CDiagVibratePlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL( R_DIAG_LST_VIBRATE );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagVibratePlugin::Uid
// ---------------------------------------------------------------------------
//
TUid CDiagVibratePlugin::Uid() const
    {
    return KDiagVibratePluginUid;
    }


// ---------------------------------------------------------------------------
// From MDiagVibratePluginObserver
// MDiagVibratePluginObserver::VibraStatusChanged
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::VibraStatusChanged( TVibrateStatus aStatus )
    {
    CDiagVibratePlugin::TState newState = EStateUnknown; 
    
    switch ( aStatus )
        {
        case EVibrateStarted:
            //The vibra can be started only from the idle state
            newState = ( iState == EStateIdle || iState == EStateRestart) ? EStateVibrating : EStateUnknown;
            break;
             
        case EVibrateEnded:
            //Vibra should not be stopped before it is started
            newState = ( iState == EStateVibrating ) ? EStateUserQuery : EStateUnknown;
            break;
            
        case EVibrateFailed:
            //check if already failed, cancelled or skipped
            newState = ( iState != EStateFailed && iState != EStateCancelled &&
                         iState != EStateSkipped ) ? EStateFailed : EStateUnknown;
            break;        
                
        case EVibrateBlocked:
			newState = ( iState == EStateVibrating ) ? EStateBlocked : EStateUnknown;
			break;
        default: //should never happen..
            __ASSERT_DEBUG( 0, User::Panic( KVibratePluginErrorUnknownVibraStateText,
                EPanicUnknownVibraState ) );
            return;
        }
        
    if ( newState != EStateUnknown) // Check if new state is available
        {
        SetState( newState );
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        }
        
    }

// ----------------------------------------------------------------------------
// From MProgressDialogCallback
// Progress dialog has been dismissed
// ----------------------------------------------------------------------------
//
void CDiagVibratePlugin::DialogDismissedL( TInt aButtonId )
    {
    //Processing the user input only if the state is still vibrating
    if ( iState == EStateVibrating )
        {
        iEngine->StopVibra();

        CDiagVibratePlugin::TState newState = EStateUnknown; 
        
        switch ( aButtonId )
            {
            case ECBACmdSkip:
                newState = EStateSkipped;
                break;

            case ECBACmdCancel:
                newState = EStateCancelled;
                break;
                
            default:
                break;
            }
        
        if ( newState != EStateUnknown )
            {
            SetState( newState );
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrNone);
            }
        }
    else if(aButtonId == EAknSoftkeyOk)
        {
        CDiagVibratePlugin::TState newState = EStateUserQuery;
        SetState( newState );
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        }
     
    }

// ----------------------------------------------------------------------------
// CDiagVibratePlugin::CDiagVibratePlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagVibratePlugin::CDiagVibratePlugin( CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam ),
        iState( EStateCancelled )
    {
    }


// ---------------------------------------------------------------------------
// CDiagVibratePlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::ConstructL()
    {
    BaseConstructL ( KDiagVibratePluginResourceFileName );
    ReadVibrationTimeL();
    }


// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagVibratePlugin::DoRunTestL()
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::DoRunTestL()
    {
    SetState( EStateIdle );

    //Let's confirm from the user that the test should be run
    FOREVER
        {
        CAknMessageQueryDialog* dialog = new ( ELeave ) CAknMessageQueryDialog();
        dialog->PrepareLC( R_VIBRATEPLUGIN_STARTQUERY );
        
        // If the test is run as a single test removing skip button
        if ( SinglePluginExecution() )
            {
            CEikButtonGroupContainer& cba = dialog->ButtonGroupContainer();
            cba.SetCommandSetL( R_VIBRATE_SOFTKEYS_OK__CANCEL );
            }

        TInt queryResult;
        
        if ( !RunWaitingDialogL( dialog, queryResult ) )
            {
            // Dialog is dismissed by deletion. Exit immediately without
            // accessing member variable, to prevent crash
            return;
            }

        //Let's check the user answer
        switch ( queryResult )
            {
            
            // ADO & Platformization Changes
            //case ECBACmdSkip:
            case ECBACmdCancel:
                CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
                return;

			case ECBACmdSkip:
			
            //case ECBACmdCancel:
                {
                //if ( SinglePluginExecution() )
                  //  {
                   // CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                   // return;
                   // }
               // else 
                 //   {
                      // See if the user really wants to cancel tests.
                    TInt confirmResult = 0;
                
                    CAknDialog* dlg = ExecutionParam().Engine().
                    CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
                
                    if ( !RunWaitingDialogL( dlg, confirmResult ) )
                        {
                        return;
                        }
                    /*
                    if ( confirmResult != EAknSoftkeyNo )
                        {
                       CompleteTestL( CDiagResultsDatabaseItem::ECancelled ); 
                        return;
                        }
                    */
                    if (confirmResult)
                           {
                           //CompleteTestL( CDiagResultsDatabaseItem::ECancelled ); 
                           return;
                           }
                    
                 //   }
              
                // Break and display the dialog again.
                }
                break;
                // Changes ends
                
            case EAknSoftkeyOk:
                {
                //Let's check that the charger is not connected
                
                if ( IsChargerConnected() && !DisplayDisconnectChargerQueryL() )
                    {
                    return;
                    }
                //Let's start the test by activating active object
                SetActive();
                TRequestStatus* status = &iStatus;
                User::RequestComplete(status, KErrNone);
                return;
                }
            default:
                return;
            }
        }
    }


// ---------------------------------------------------------------------------
// From CActive
// CDiagVibratePlugin::RunL
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::RunL()
    {

	// ravi - badwarning #546-D    
    TRequestStatus* status = &iStatus;
    
    switch (iState) 
        {
        case EStateRestart:
        	{
        	iEngine->StopVibra();
        	LOGSTRING( "In RunL, EStateRestart");
            ShowVibratingNoteL();
            iEngine = CDiagVibratePluginEngine::NewL( *this );    
            TRAPD(Err,iEngine->StartVibraL( iVibrationTime ));
            if (Err == KErrNone)
            LOGSTRING("Leave code for restart = Kerrnone");
            LOGSTRING2( "Leave Code for in Restart StartVibra in Hex( 0x%x )", Err );
            break;
        	}
        case EStateIdle:
            if(!SetVibrateSettingOnL())
            	break;
            ShowVibratingNoteL();
            iEngine = CDiagVibratePluginEngine::NewL( *this );    
            iEngine->StartVibraL( iVibrationTime );
            break;
            
        case EStateVibrating:
            //Vibration started -> no action needed
            break;
            
        case EStateUserQuery: 
            {
            delete iEngine;
            iEngine = NULL;
            
            if ( iProgressDialog )
                {
                iProgressDialog->ProcessFinishedL();
                }
 
            SetVibrateSettingToOriginalValueL();
            //displaying user query: did the phone vibrate

            CAknQueryDialog* dialog = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
            dialog->PrepareLC( R_VIBRATEPLUGIN_CONFQUERY_DLG );

            TInt queryResult;
            if ( RunWaitingDialogL( dialog, queryResult ) )
                {
                //Let's check the user answer    
                switch ( queryResult )
                    {
                    case EAknSoftkeyYes:
                        LOGSTRING("CDiagVibratePlugin-EAknSoftkeyYes")
                        CompleteTestL( CDiagResultsDatabaseItem::ESuccess );
                        break;
                        
                    case ECBACmdNo:
                        LOGSTRING("CDiagVibratePlugin-EAknSoftkeyNo")
                        CompleteTestL( CDiagResultsDatabaseItem::EFailed );
                        break;
                    
                    default:
                        LOGSTRING("CDiagVibratePlugin-Default")
                        break;
                    }
                }
            }
            break;
            
        case EStateCancelled:
            {
             if ( SinglePluginExecution() )
                {
                CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                return;
                }
             else 
                {                                                    
                TInt confirmResult = 0;
                CAknDialog* dlg = ExecutionParam().Engine().
                CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );
                
                if ( !RunWaitingDialogL( dlg, confirmResult ) )
                    {
                    //the dialog got deleted, returning right away
                    return;
                    }                    
                    
                if ( confirmResult == EAknSoftkeyYes )
                    {
                    //test is cancelled
                    CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                    }
                else //EAknSoftKeyNo
                    {
                    //let's rerun the test
                    DoStopAndCleanupL();
                    DoRunTestL();
                    }
                }
            }
            break; 
        
        case EStateSkipped:
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            break;
       
        case EStateFailed:
            CompleteTestL( CDiagResultsDatabaseItem::EFailed );
            break;
        
        case EStateBlocked:
        	LOGSTRING( "In RunL, EStateBlocked");
        	if ( iProgressDialog )
        		{
        		iProgressDialog->ProcessFinishedL();       
        		}
        	if (!DisplayDisconnectChargerQueryL() )
               {
                return;
               }
                //Let's start the test by activating active object
            SetState( EStateRestart );
            SetActive();
            User::RequestComplete(status, KErrNone);

        	break;
            
        default:
            __ASSERT_DEBUG( 0, User::Panic( KVibratePluginErrorUnknownVibraStateText,
                EPanicUnknownVibraState ) );
            break;    
        }
    }    

// ---------------------------------------------------------------------------
// CDiagVibratePlugin::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagVibratePlugin::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagVibratePlugin::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// From CActive
// CDiagVibratePlugin::DoCancel
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::DoCancel()
    {
    }    


// ---------------------------------------------------------------------------
// Cleanup the created resources
// CDiagVibratePlugin::DoStopAndCleanupL
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::DoStopAndCleanupL()
    {
    delete iEngine;    
    iEngine = NULL;
    if ( iProgressDialog )
        {
        iProgressDialog->ProcessFinishedL();       
        }
    SetVibrateSettingToOriginalValueL();
    iVibrateSettingUpdated = EFalse;
    if ( iState != EStateCancelled )
        {
        SetState( EStateCancelled );
        }
    }

    
// ---------------------------------------------------------------------------
// Makes sure vibrate is enabled in the profile
// CDiagVibratePlugin::SetVibrateSettingOnL
// ---------------------------------------------------------------------------
//
TInt CDiagVibratePlugin::SetVibrateSettingOnL()
    {
    MProEngEngine* profileEng = ProEngFactory::NewEngineL();
    CleanupDeletePushL(profileEng);

    MProEngProfile* profile = profileEng->ActiveProfileL();
    MProEngToneSettings& profileToneSettings = profile->ToneSettings();
    iOrigVibrateSetting = profileToneSettings.VibratingAlert();
   
   //set the vibration setting on if it is not on already
    if (!iOrigVibrateSetting)
        {
        if(!ShowActivateConfNoteL())
        {
        		profile->Release();    
    			CleanupStack::PopAndDestroy(profileEng);
    			return EFalse;
    	}
        profileToneSettings.SetVibratingAlert(ETrue);
        profile->CommitChangeL();
        iVibrateSettingUpdated = ETrue;
        }
    
    profile->Release();    
    CleanupStack::PopAndDestroy(profileEng);
    return ETrue;
    }
    
    
TInt CDiagVibratePlugin::ShowActivateConfNoteL()
{
	CAknQueryDialog * dialog = new (ELeave) CAknQueryDialog();
    dialog->PrepareLC( R_VIBRATEPLUGIN_VIBRATE_ACTIVATE_DLG );
    
    
    TInt queryResult;
        
    if ( !RunWaitingDialogL( dialog, queryResult ) )
         {
         // Dialog is dismissed by deletion. Exit immediately without
         // accessing member variable, to prevent crash
         return EFalse;
         }

        //Let's check the user answer
        switch ( queryResult )
            {
            
            case ECBACmdNo:
                CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
                return EFalse;

            case EAknSoftkeyYes:
            	return ETrue;

            default:
                return EFalse;
            }	
	
}

    
// ---------------------------------------------------------------------------
// Sets the vibration setting to its original value
// CDiagVibratePlugin::SetVibrateSettingToOriginalValueL
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::SetVibrateSettingToOriginalValueL()
    {
    if ( !iVibrateSettingUpdated )
        {
        return;
        }
        
    MProEngEngine* profileEng = ProEngFactory::NewEngineL();
    MProEngProfile* profile = profileEng->ActiveProfileL();
    MProEngToneSettings& profileToneSettings = profile->ToneSettings();
    
    //Check if vibrate setting need to be turned off again
    if ( !iOrigVibrateSetting )
        {
        profileToneSettings.SetVibratingAlert( EFalse );
        profile->CommitChangeL();
        }
        
    profile->Release();
    profileEng->Release();                

    iVibrateSettingUpdated = EFalse;
    }    


// ---------------------------------------------------------------------------
// Checks if the charger is connected if yes the user is asked to removed it.
// CDiagVibratePlugin::IsChargerConnected
// ---------------------------------------------------------------------------
//
TBool CDiagVibratePlugin::IsChargerConnected() const
    {
    TInt charger;
    TBool result = EFalse;
    
    if( RProperty::Get( KPSUidHWRMPowerState, KHWRMChargingStatus, charger ) == KErrNone )
        {
        switch( charger )
            {
            case EChargingStatusCharging:
            case EChargingStatusNotCharging:
            case EChargingStatusChargingComplete:
            case EChargingStatusAlmostComplete:
            case EChargingStatusChargingContinued:
                result = ETrue;
                break;
                
            case EChargingStatusNotConnected:
            default:
                break;
            }
        }
        
    return( result );
    }


// ---------------------------------------------------------------------------
// Sets the internal state
// CDiagVibratePlugin::SetState
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::SetState( CDiagVibratePlugin::TState aState )
    {
    __ASSERT_DEBUG( iState != aState, User::Panic( KVibratePluginErrorStateRepeatedText,
                EPanicStateRepeated ) );

    iState = aState;
    }

// ---------------------------------------------------------------------------
// Displays disconnect charger query
// CDiagVibratePlugin::DisplayDisconnectChargerQuery
// ---------------------------------------------------------------------------
//
TBool CDiagVibratePlugin::DisplayDisconnectChargerQueryL()
    {
    FOREVER
        {
        CAknQueryDialog* dialog = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );
        dialog->PrepareLC( R_VIBRATEPLUGIN_DISCONNECT_CHARGER_DLG );


        TInt queryResult;
        
        if ( !RunWaitingDialogL( dialog, queryResult ) )
            {
            // Dialog is dismissed by deletion. Exit immediately without
            // accessing member variable, to prevent crash
            return EFalse;
            }

        //Let's check the user answer
        switch ( queryResult )
            {
            
            case ECBACmdCancel:
                {
                if ( SinglePluginExecution() )
                    {
                    CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
                    return EFalse;
                    }
                else
                    {
                    // See if the user really wants to cancel tests.
                    TInt confirmResult = 0;
                    
                    CAknDialog* dlg = ExecutionParam().Engine().
                        CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );
                    
                    if ( !RunWaitingDialogL( dlg, confirmResult ) )
                        {
                        return EFalse;
                        }
                        
                    if ( confirmResult)
                        {
                        return EFalse;
                        }
                    else
                        {
                        continue;
                        }
                    }

                // Break and display the dialog again.
                }
                
            case EAknSoftkeyOk:
                if ( !IsChargerConnected() )
                    {
                    return ETrue;   
                    }
                break;
            default:
                return EFalse;
            }
        }
    }


// ---------------------------------------------------------------------------
// Displays vibrating progress dialog
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::ShowVibratingNoteL()
    {
    if ( iProgressDialog )
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        }
    
    //Calculate progress bar update interval in hundredths of seconds
    TInt interval = KHundredthsInSecond * iVibrationTime *
                    ( ( TReal ) KProgressDialogIncrementSize / 
                    KProgressDialogFinalValue );
                    
    iProgressDialog = new ( ELeave ) CAknProgressDialog( 
                        KProgressDialogFinalValue, 
                        KProgressDialogIncrementSize,
                        interval,
                        reinterpret_cast <CEikDialog**> ( &iProgressDialog ) );

    iProgressDialog->SetCallback( this );
    iProgressDialog->PrepareLC( R_VIBRATEPLUGIN_PROGRESS_NOTE_VIBRATING );
    
    // If the test is run as a single test removing skip button
    if ( SinglePluginExecution() )
        {
        CEikButtonGroupContainer& cba = iProgressDialog->ButtonGroupContainer();
        cba.SetCommandSetL( R_VIBRATE_SOFTKEYS_STOP__CANCEL );
        }

    iProgressDialog->RunLD();
    }


// ---------------------------------------------------------------------------
// Reads vibration time from central repository
// ---------------------------------------------------------------------------
//
void CDiagVibratePlugin::ReadVibrationTimeL()
    {
    LOGSTRING( "CDiagVibratePlugin::ReadVibrationTimeL begin" );
                
    CRepository* repository = CRepository::NewLC( KCRUidDevDiagVibraTestPlugin );  
         
    User::LeaveIfError( repository->Get( KDevDiagVibraTestVibrateTime, iVibrationTime ) );
    
    CleanupStack::PopAndDestroy( repository );
    
    LOGSTRING( "CDiagVibratePlugin::ReadVibrationTimeL end" );
    }
    

// End of File
