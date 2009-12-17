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
* Description:  This is the class module for the Diagnostics USB Cable Test plugin
*
*/


// INCLUDE FILES

// Class Declaration
#include "diagusbcabletestplugin.h"

// System includes
#include <avkon.hrh>                        // command definitions
#include <StringLoader.h>                   // StringLoader
#include <aknmessagequerydialog.h>     		// CAknMessageQueryDialog
#include <DiagFrameworkDebug.h>             // Debug macros
#include <DiagTestExecParam.h>              // TDiagTestExecParam
#include <DiagEngineCommon.h>               // MDiagEngineCommon
#include <devdiagusbcabletestpluginrsc.rsg>    // resources

// User includes
#include "diagusbcabletestengine.h"         // CDiagUSBCableTestEngine
#include "diagusbcabletestplugin.hrh"       // UIDs and command ids

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint KTotalNumOfSteps = 2;
const TUid KDiagUSBCableTestPluginUid = { _IMPLEMENTATION_UID };
_LIT( KDiagUSBCableTestPluginResourceFileName, "z:DevDiagUSBCableTestPluginRsc.rsc" );

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
CDiagUSBCableTestPlugin::CDiagUSBCableTestPlugin
                                  ( CDiagPluginConstructionParam* aParam )
    : CDiagTestPluginBase( aParam )
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::CDiagUSBCableTestPlugin()" )
    }


// ---------------------------------------------------------------------------
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagUSBCableTestPlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::NewL()" )
    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>(aInitParams);

    CleanupStack::PushL( param );
    CDiagUSBCableTestPlugin* self = 
            new( ELeave ) CDiagUSBCableTestPlugin (param);
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }



// ---------------------------------------------------------------------------
// Symbian OS second phase constructor
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::ConstructL()
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::ConstructL()" )
    BaseConstructL ( KDiagUSBCableTestPluginResourceFileName );
    }


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CDiagUSBCableTestPlugin::~CDiagUSBCableTestPlugin()
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::~CDiagUSBCableTestPlugin()" )
    // StopAndCleanupL() called by base class, whcih calls
    //  Cancel()    -> DoCancel()
    //  DoStopAndCleanupL()
    // Not much to do here.
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagUSBCableTestPlugin::IsVisible()
// ---------------------------------------------------------------------------

TBool CDiagUSBCableTestPlugin::IsVisible() const
    {
    return ETrue;
    }
   

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagUSBCableTestPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagUSBCableTestPlugin::RunMode() const
    {
    return EInteractiveDialog;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagUSBCableTestPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagUSBCableTestPlugin::TotalSteps() const
    {
    return KTotalNumOfSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagUSBCableTestPlugin::GetPluginName
// ---------------------------------------------------------------------------
HBufC* CDiagUSBCableTestPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_USB_CABLE_TEST_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_USB_CABLE_TEST_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_USB_CABLE_TEST_TITLE_PANE );
                                                                           
        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_USB_CABLE_TEST_SINGLE_GRAPHIC  );        
        
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_USB_CABLE_TEST_SINGLE ); 

        default:
            LOGSTRING2( "CDiagUSBCableTestPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_USB_CABLE_TEST_SINGLE );
        }
    }
    



// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagUSBCableTestPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagUSBCableTestPlugin::Uid() const
    {
    return KDiagUSBCableTestPluginUid;
    }
    
   
// ---------------------------------------------------------------------------
// From CActive
// CDiagUSBCableTestPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::RunL()
    {
    // nothing to do here
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagUSBCableTestPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::DoCancel()
    {
    // nothing to do here
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagUSBCableTestPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::DoRunTestL() Test execution begins here." )
    // ADO & Platformization Changes
    CAknMessageQueryDialog* dlg    = NULL;
    TInt aButtonId,result;
    // code ends

    // Run USB Cable Test
    ASSERT( iEngine == NULL );
    
    // ADO & Platformization Changes
	dlg = new ( ELeave ) CAknMessageQueryDialog();

    dlg->PrepareLC(R_MESSAGEQUERY_TITLE );
    
    if(SinglePluginExecution())
        {
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL(R_DIAG_USB_SOFTKEYS_CANCEL__OK);
        }

	result = RunWaitingDialogL( dlg, aButtonId );
	
	if (result == KErrNone)
	{
	LOGSTRING("RunWaitingDialogL return = true");
	}
	
	if(aButtonId == EUsbTestConnectUSBCancel )
		{
		CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
		return;	
		}
	
   if(aButtonId == EUsbTestConnectUSBSkip )
        {
        TInt aUserResponse;
        TInt aResponseType;
        TInt aOption = 0;
        aResponseType = AskCancelExecutionL(aUserResponse,aOption);
        
        if(!aResponseType)
            return;
        
        if (aUserResponse)
               { 
               return;
               }
        }
		
	// ADO & Platformization Changes

    iEngine = CDiagUSBCableTestEngine::NewL(*this, SinglePluginExecution() );
    iEngine->RunUSBCableTestL();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagUSBCableTestPlugin::DoStopAndCleanupL()
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::DoStopAndCleanupL()
    {
    LOGSTRING( "CDiagUSBCableTestPlugin::DoStopAndCleanupL()." )
    delete iEngine;
    iEngine = NULL;
    }

// ---------------------------------------------------------------------------
// From MDiagUSBTestReportResultInterface
// CDiagUSBCableTestPlugin::ReportTestResultL()
// ---------------------------------------------------------------------------
void CDiagUSBCableTestPlugin::ReportTestResultL(
                                    CDiagResultsDatabaseItem::TResult aResult)
    {
    CompleteTestL( aResult );
    }

// ---------------------------------------------------------------------------
// The engine class uses this callback on the plugin class to ask the user if 
// the entire test execution needs to be cancelled.
// ---------------------------------------------------------------------------
TBool CDiagUSBCableTestPlugin::AskCancelExecutionL(TInt& aUserResponse, TInt aOption)
    {
    CAknDialog* cancelDialog = NULL;
    if(aOption == 0)
        {
        cancelDialog = ExecutionParam().Engine().
                   CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
        }
    else
        {
        cancelDialog = ExecutionParam().Engine().
                           CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL);
        }
          
    return RunWaitingDialogL( cancelDialog, aUserResponse );
    }    

// End of File

