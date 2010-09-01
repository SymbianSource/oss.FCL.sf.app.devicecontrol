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
* Description:  This is the class module for the Diagnostics Data Network Test 
*                 plug-in class.
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagdatanetworktestplugin.h"      // CDiagDataNetworkTestPlugin

// System includes
#include <centralrepository.h> 
#include <StringLoader.h>                   // StringLoader
#include <devdiagdatanetworktestpluginrsc.rsg> // Resources
#include <DiagFrameworkDebug.h>             // LOGSTRING
#include "diagdatanetworkprivatecrkeys.h"

// User includes
#include "diagdatanetworktestplugin.hrh"    // UIDs
#include "diagdatanetworktestpdpengine.h"   // CDiagDataNetworkTestPDPEngine
// ADO & Platformization Changes
#include <DiagFrameworkDebug.h>                     // debug macros
#include <DiagEngineCommon.h>                       // MDiagEngineCommon
#include <DiagCommonDialog.h> 
#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagTestExecParam.h>          // TDiagTestExecParam
#include <DiagPluginExecPlan.h>
#include <DiagExecPlanEntry.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUid KDiagDataNetworkTestPluginUid = { _IMPLEMENTATION_UID };
_LIT( KDiagDataNetworkTestPluginResourceFileName, "z:DevDiagDataNetworkTestPluginRsc.rsc" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::CDiagDataNetworkTestPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagDataNetworkTestPlugin::CDiagDataNetworkTestPlugin( CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam )
    {
    LOGSTRING("CDiagDataNetworkTestPlugin::CDiagDataNetworkTestPlugin()")
    }


// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagDataNetworkTestPlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING("CDiagDataNetworkTestPlugin::NewL()")
    
    /*CRepository  *cRep;
    TInt val;
    cRep = CRepository::NewL( KCRUidDiagDataNetworkTestPlugin );    
    CleanupStack::PushL( cRep );  
    TInt err = cRep->Get( KDiagDataNetworkTestPluginPrimaryAPN,val );
    CleanupStack::PopAndDestroy(cRep);
    

    return NULL;*/
    
    if ( !aInitParams )
        {
        User::Leave ( KErrArgument );
        }

    CleanupStack::PushL( aInitParams );
    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>(aInitParams);
                
    CDiagDataNetworkTestPlugin* self = new( ELeave ) CDiagDataNetworkTestPlugin (param);
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop(self);
    CleanupStack::Pop( aInitParams );
    return self;
    }



// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagDataNetworkTestPlugin::ConstructL()
    {
    LOGSTRING("CDiagDataNetworkTestPlugin::ConstructL()")
    BaseConstructL ( KDiagDataNetworkTestPluginResourceFileName );
    }


// ----------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::~CDiagDataNetworkTestPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagDataNetworkTestPlugin::~CDiagDataNetworkTestPlugin()
    {
    LOGSTRING("CDiagDataNetworkTestPlugin::~CDiagDataNetworkTestPlugin()")
    
    // StopAndCleanupL() called by base class destructor, whcih calls
    //  Cancel()    -> DoCancel()
    //  DoStopAndCleanupL()
    // Not much to do here.
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagDataNetworkTestPlugin::IsVisible()
// ---------------------------------------------------------------------------

TBool CDiagDataNetworkTestPlugin::IsVisible() const
    {
    return ETrue;
    }
   

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagDataNetworkTestPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagDataNetworkTestPlugin::RunMode() const
    {
    return EAutomatic;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagDataNetworkTestPlugin::DoStopAndCleanupL()
// ---------------------------------------------------------------------------
void CDiagDataNetworkTestPlugin::DoStopAndCleanupL() 
    {
    delete iPDPEngine;
    iPDPEngine = NULL;        
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagDataNetworkTestPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagDataNetworkTestPlugin::TotalSteps() const
    {
    return (CDiagDataNetworkTestPDPEngine::TotalSteps());
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagDataNetworkTestPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagDataNetworkTestPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_DATA_NETWORK_TEST_HEADING_PANE );
            
        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_DATA_NETWORK_TEST_INFO_PANE );            

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_DATA_NETWORK_TEST_TITLE_PANE );
            
        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_DATA_NETWORK_TEST_SINGLE_GRAPHIC  );  
                    
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_DATA_NETWORK_TEST_SINGLE );

        default:
            LOGSTRING2( "CDiagDataNetworkTestPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_DATA_NETWORK_TEST_SINGLE );
        }
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagDataNetworkTestPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagDataNetworkTestPlugin::Uid() const
    {
    return KDiagDataNetworkTestPluginUid;
    }
    
   
// ---------------------------------------------------------------------------
// From CActive
// CDiagDataNetworkTestPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagDataNetworkTestPlugin::RunL()
    {
    // nothing to implement here
    }    

// ---------------------------------------------------------------------------
// From CActive
// CDiagDataNetworkTestPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagDataNetworkTestPlugin::DoCancel()
    {
    // nothing to implement here   
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagDataNetworkTestPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagDataNetworkTestPlugin::DoRunTestL()
    {
    ASSERT( iPDPEngine == NULL );

    delete iPDPEngine;
    iPDPEngine = NULL;
    
     // ADO & Platformization Changes
     
    if(!(ExecutionParam().Engine().ExecutionPlanL().CurrentExecutionItem().AsDependency()))
    {
     
    TInt aButtonId;
    
    ShowMessageQueryL(R_DIAG_MESSAGEQUERY_TITLE_DATANETWORK,aButtonId);
      
    if(aButtonId == EDataNetworkTestCancel)
    	{
    	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == EDataNetworkTestSkip)
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
    	
    	}
    // Changes Ends
    iPDPEngine = CDiagDataNetworkTestPDPEngine::NewL(*this);
       
    // Run the Data Network Test
    iPDPEngine->RunDataNetworkTestL();
    }
    

TBool CDiagDataNetworkTestPlugin::DisplayDialog(TInt& aUserResponse)
{

	// set softkey for single execution
	if ( !SinglePluginExecution() )
	{
		// Create common dialog by invoking Engine
		CAknDialog* dlg = ExecutionParam().Engine().
		CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );

		return RunWaitingDialogL( dlg, aUserResponse );
	}	
	else 
	{
		//CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	 //commented for NTEI-7EZ96S
		return EFalse;
	}

}
// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::ReportTestResultL()
// ---------------------------------------------------------------------------    
void CDiagDataNetworkTestPlugin::ReportTestResultL( CDiagResultsDatabaseItem::TResult aResult )
    {
    // before returning the test result to the Diagnostics interface,
    // delete the engine
    delete iPDPEngine;
    iPDPEngine = NULL;        
                   
    CompleteTestL( aResult );
    }
    
// ---------------------------------------------------------------------------
// CDiagDataNetworkTestPlugin::ReportTestProgL()
// ---------------------------------------------------------------------------      
void CDiagDataNetworkTestPlugin::ReportTestProgL(TInt aStep)
    {
    ReportTestProgressL( aStep );
    }
    // ADO & Platformization Changes
TBool CDiagDataNetworkTestPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_DIAG_MESSAGEQUERY_TITLE_DATANETWORK: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_CBA_DATANETWORK_SINGLE_EXECUTION );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_CBA_DATANETWORK_GROUP_EXECUTION );		
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

