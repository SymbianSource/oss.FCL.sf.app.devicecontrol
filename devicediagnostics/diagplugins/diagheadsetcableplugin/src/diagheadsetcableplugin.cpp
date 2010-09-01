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
* Description:  This is the class module for the Diag Headset Cable Test plugin
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagheadsetcableplugin.h"

// System includes
#include <StringLoader.h>                           // StringLoader
#include <avkon.hrh>                                // command ids
#include <DiagFrameworkDebug.h>                     // debug macros
#include <DiagTestExecParam.h>                      // TDiagTestExecParam
#include <DiagEngineCommon.h>                       // MDiagEngineCommon
#include <devdiagheadsetcabletestpluginrsc.rsg>        // resources

// User includes
#include "diagheadsetcabletestplugin.hrh"           // UIDs
#include "diagheadsetcableengine.h"                 // CDiagHeadsetCableTestEngine


#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint KTotalNumOfSteps = 2;
const TUid KDiagHeadsetCableTestPluginUid = { _IMPLEMENTATION_UID };
_LIT( KDiagHeadsetCableTestPluginResourceFileName, "z:DevDiagHeadsetCableTestPluginRsc.rsc" );


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CDiagHeadsetCableTestPlugin::CDiagHeadsetCableTestPlugin( 
    CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam )
    {
    LOGSTRING( "CDiagHeadsetCableTestPlugin instance created" )
    }


// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagHeadsetCableTestPlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING( "CDiagHeadsetCableTestPlugin::NewL() " )
    ASSERT( aInitParams );

    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>(aInitParams);    

    CleanupStack::PushL( param );
    CDiagHeadsetCableTestPlugin* self = new( ELeave ) CDiagHeadsetCableTestPlugin (param);
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::ConstructL()
    {
    LOGSTRING( "CDiagHeadsetCableTestPlugin::ConstructL() " )
    BaseConstructL ( KDiagHeadsetCableTestPluginResourceFileName );
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CDiagHeadsetCableTestPlugin::~CDiagHeadsetCableTestPlugin()
    {
    LOGSTRING( "CDiagHeadsetCableTestPlugin::~CDiagHeadsetCableTestPlugin()" )
    // StopAndCleanupL() called by base class, whcih calls
    //  Cancel()    -> DoCancel()
    //  DoStopAndCleanupL()
    // Not much to do here.
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagHeadsetCableTestPlugin::IsVisible()
// ---------------------------------------------------------------------------

TBool CDiagHeadsetCableTestPlugin::IsVisible() const
    {
    return ETrue;
    }
   

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagHeadsetCableTestPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagHeadsetCableTestPlugin::RunMode() const
    {
    return EInteractiveDialog;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagHeadsetCableTestPlugin::DoStopAndCleanupL()
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::DoStopAndCleanupL()
    {
    delete iEngine;
    iEngine = NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagHeadsetCableTestPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagHeadsetCableTestPlugin::TotalSteps() const
    {
    return KTotalNumOfSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagHeadsetCableTestPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagHeadsetCableTestPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_HEADSET_CABLE_TEST_HEADING_PANE ); 
            
        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_HEADSET_CABLE_TEST_INFO_PANE );
            
        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_HEADSET_CABLE_TEST_TITLE_PANE );                               
        
        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_HEADSET_CABLE_TEST_SINGLE_GRAPHIC  );             

        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_HEADSET_CABLE_TEST_SINGLE ); 

        default:
            LOGSTRING2( "CDiagHeadsetCableTestPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_HEADSET_CABLE_TEST_SINGLE );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagHeadsetCableTestPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagHeadsetCableTestPlugin::Uid() const
    {
    return KDiagHeadsetCableTestPluginUid;
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagHeadsetCableTestPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagHeadsetCableTestPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagHeadsetCableTestPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagHeadsetCableTestPlugin::DoRunTestL() Test execution begins" )
    ASSERT( iEngine == NULL );

    // Run Battery Charger Cable Test
    delete iEngine;
    iEngine = NULL;

    iEngine = CDiagHeadsetCableTestEngine::NewL( *this, SinglePluginExecution() );
    // ADO & Platformization Changes
    TInt aButtonId;
    
    ShowMessageQueryL(R_DIAG_MESSAGEQUERY_TITLE_HEADSET,aButtonId);
      
    if(aButtonId == EHeadsetTestConnectHeadsetCancel)
    	{
    	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == EHeadsetTestConnectHeadsetSkip)
    	{
		TInt confirmResult = 0;
            
        CAknDialog* dlg = ExecutionParam().Engine().
        CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
    
        if ( !RunWaitingDialogL( dlg, confirmResult ) )
            {
            return;
            }
        
        if ( confirmResult)
            {
            return;
            }
    	}
    // Changes Ends
    iEngine->RunHeadsetCableTestL();        
    }

// ---------------------------------------------------------------------------
// This is a callback from the engine class to report the result of the test
// ---------------------------------------------------------------------------
void CDiagHeadsetCableTestPlugin::ReportTestResultL(
    CDiagResultsDatabaseItem::TResult aResult)
    {
    CompleteTestL( aResult );
    }

// ---------------------------------------------------------------------------
// The engine class uses this callback on the plugin class to ask the user if 
// the entire test execution needs to be cancelled.
// ---------------------------------------------------------------------------
TBool CDiagHeadsetCableTestPlugin::AskCancelExecutionL(TInt& aUserResponse)
    {
    CAknDialog* cancelDialog = ExecutionParam().Engine().
             CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );
             
    return RunWaitingDialogL( cancelDialog, aUserResponse );
    }    
    
    // ADO & Platformization Changes
TBool CDiagHeadsetCableTestPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_DIAG_MESSAGEQUERY_TITLE_HEADSET: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_CBA_CONNECT_AND_PRESS_OK_SINGLE_EXECUTION );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_CBA_CONNECT_AND_PRESS_OK_GROUP_EXECUTION );		
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

