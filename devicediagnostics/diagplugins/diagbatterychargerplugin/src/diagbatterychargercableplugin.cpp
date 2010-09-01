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
* Description:  This is the implementation module for the Diagnostics Battery Charger 
*                 Cable Test plugin
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagbatterychargercableplugin.h"

// System includes
#include <StringLoader.h>                               // StringLoader
#include <avkon.hrh>                                    // command definitions
#include <DiagFrameworkDebug.h>                         // debug macros
#include <DiagTestExecParam.h>                          // TDiagTestExecParam
#include <DiagEngineCommon.h>                           // MDiagEngineCommon
#include <devdiagbatterychargercabletestpluginrsc.rsg>     // resource defintions

// User includes
#include "diagbatterychargercableengine.h"              // CDiagBatteryChargerCableTestEngine
#include "diagbatterychargercabletestplugin.hrh"        // UIDs and Command Ids


#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll 
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint KTotalNumOfSteps = 2;
const TUid KDiagBatteryChargerCableTestPluginUid = { _IMPLEMENTATION_UID };
_LIT( KDiagBatteryChargerCableTestPluginResourceFileName, "z:DevDiagBatteryChargerCableTestPluginRsc.rsc" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// Standard C++ Constructor
// ----------------------------------------------------------------------------
//
CDiagBatteryChargerCableTestPlugin::CDiagBatteryChargerCableTestPlugin
                                  ( CDiagPluginConstructionParam* aParam )
    : CDiagTestPluginBase( aParam )
    {
    // nothing to do here
    }


// ---------------------------------------------------------------------------
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagBatteryChargerCableTestPlugin::NewL( TAny* aInitParams )
    {
    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>(aInitParams);

    CleanupStack::PushL( param );
    CDiagBatteryChargerCableTestPlugin* self = 
            new( ELeave ) CDiagBatteryChargerCableTestPlugin (param);
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }



// ---------------------------------------------------------------------------
// Symbian OS second phased constructor
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::ConstructL()
    {
    BaseConstructL ( KDiagBatteryChargerCableTestPluginResourceFileName );
    }


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CDiagBatteryChargerCableTestPlugin::~CDiagBatteryChargerCableTestPlugin()
    {
    // StopAndCleanupL() called by base class, whcih calls
    //  Cancel()    -> DoCancel()
    //  DoStopAndCleanupL()
    // Not much to do here.
    }


// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------

TBool CDiagBatteryChargerCableTestPlugin::IsVisible() const
    {
    return ETrue;
    }
   

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the plugin's level of interactivity.
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagBatteryChargerCableTestPlugin::RunMode() const
    {
    return EInteractiveDialog;
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the total number of execution steps for the plugin.
// ---------------------------------------------------------------------------
TUint CDiagBatteryChargerCableTestPlugin::TotalSteps() const
    {
    return KTotalNumOfSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagBatteryChargerCableTestPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagBatteryChargerCableTestPlugin::GetPluginNameL( 
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {        
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_BATTERY_CHARGER_CABLE_TEST_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_BATTERY_CHARGER_CABLE_TEST_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_BATTERY_CHARGER_CABLE_TEST_TITLE_PANE );
                                        
        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_BATTERY_CHARGER_CABLE_TEST_SINGLE_GRAPHIC  );        
        
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_BATTERY_CHARGER_CABLE_TEST_SINGLE ); 
			
        default:
            {
            LOGSTRING2( "CDiagBatteryChargerCableTestPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            
            return StringLoader::LoadL ( R_DIAG_BATTERY_CHARGER_CABLE_TEST_SINGLE );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagBatteryChargerCableTestPlugin::Uid() const
    {
    return KDiagBatteryChargerCableTestPluginUid;
    }
    
   
// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Called from the base class to begin test execution.
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagBatteryChargerCableTestPlugin Test execution begins" )
    
    // Run Battery Charger Cable Test
    ASSERT( iEngine == NULL );

    iEngine = CDiagBatteryChargerCableTestEngine::NewL(*this, 
                                                SinglePluginExecution() );
                                                
                                                
        // ADO & Platformization Changes
    TInt aButtonId;
    
    ShowMessageQueryL(R_DIAG_MESSAGEQUERY_TITLE_BATTERY_CHARGER,aButtonId);
      
    if(aButtonId == EBatteryTestConnectBatteryChargerCancel)
    	{
    	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == EBatteryTestConnectBatteryChargerSkip)
    	{
		TInt confirmResult = 0;
            
        CAknDialog* dlg = ExecutionParam().Engine().
        CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
    
        if ( !RunWaitingDialogL( dlg, confirmResult ) )
            {
            return;
            }
        
        if (confirmResult)
            {
            return;
            }
    	}
    // Changes Ends
    iEngine->RunBatteryChargerTestL();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagBatteryChargerCableTestPlugin::DoStopAndCleanupL()
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::DoStopAndCleanupL()
    {
    delete iEngine;
    iEngine = NULL;
    }

// ---------------------------------------------------------------------------
// The engine class uses this interface to inform the plug-in class about the 
// test result.
// ---------------------------------------------------------------------------
void CDiagBatteryChargerCableTestPlugin::ReportTestResultL(
                                    CDiagResultsDatabaseItem::TResult aResult)
    {
    CompleteTestL( aResult );
    }  
    

// ---------------------------------------------------------------------------
// The engine class uses this callback on the plugin class to ask the user if 
// the entire test execution needs to be cancelled.
// ---------------------------------------------------------------------------
TBool CDiagBatteryChargerCableTestPlugin::AskCancelExecutionL(TInt& aUserResponse)
    {
    CAknDialog* cancelDialog = ExecutionParam().Engine().
             CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );
             
    return RunWaitingDialogL( cancelDialog, aUserResponse );
    }
// ADO & Platformization Changes
TBool CDiagBatteryChargerCableTestPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
    {
    LOGSTRING( "CDiagAudioPlugin::ShowMessageQueryL IN" )
    CAknMessageQueryDialog* dlg    = NULL;
    TBool                   result = EFalse;

    // Create CAknMessageQueryDialog instance
    dlg = new ( ELeave ) CAknMessageQueryDialog();

    dlg->PrepareLC( aResourceId );
     
    CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();

        switch ( aResourceId )
            {
            case R_DIAG_MESSAGEQUERY_TITLE_BATTERY_CHARGER: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_CBA_CONNECT_AND_PRESS_OK_SINGLE_EXECUTION );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_CBA_CONNECT_AND_PRESS_OK_SKIP_GROUP_EXECUTION );		
                	}
        		}
                break;

            default:
                break;
            }
        	
    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagAudioPlugin::ShowMessageQueryL() OUT aButtonId=%d result=%d", aButtonId, result );
    return result;
    }
// End of File

