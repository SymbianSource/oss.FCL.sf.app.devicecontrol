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
* Description:  This is the implementation of the Browser Test Plugin, which
*                tests HTTP functionality.
*
*/


// System Include Files
#include <StringLoader.h>                   // StringLoader
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <devdiagbrowserpluginrsc.rsg>         // Resource Definitions

// User Include Files
#include "diagbrowserplugin.h"              // CDiagBrowserPlugin
#include "diagbrowserpluginmodel.h"         // CDiagBrowserPluginModel
#include "diagbrowserplugin.hrh"            // UID definition
#include "diagbrowserplugin.pan"            // Panic

#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll

// ADO & Platformization Changes
#include <DiagFrameworkDebug.h>                     // debug macros
#include <DiagTestExecParam.h>                      // TDiagTestExecParam
#include <DiagEngineCommon.h>                       // MDiagEngineCommon

// Local Constants
_LIT( KDiagBrowserPluginResourceFileName, "z:devdiagbrowserpluginrsc.rsc" );
const TUid KDiagBrowserPluginUid = { _IMPLEMENTATION_UID };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagBrowserPlugin::NewL( TAny* aInitParams )
    {
    LOGSTRING2( "CDiagBrowserPlugin::NewL( 0x%x )", aInitParams )

    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagBrowserPluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagBrowserPlugin* self = new( ELeave ) CDiagBrowserPlugin( param );
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDiagBrowserPlugin::~CDiagBrowserPlugin()
    {
    LOGSTRING( "CDiagBrowserPlugin::~CDiagBrowserPlugin()" )
    delete iModel;
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagBrowserPlugin::CDiagBrowserPlugin( CDiagPluginConstructionParam* aParam )
:   CDiagTestPluginBase( aParam )
    {
    LOGSTRING2( "CDiagBrowserPlugin::CDiagBrowserPlugin( 0x%x )", aParam )

    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPlugin::ConstructL()
    {
    LOGSTRING( "CDiagBrowserPlugin::ConstructL()" )
    BaseConstructL( KDiagBrowserPluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagBrowserPlugin::IsVisible() const
    {
    LOGSTRING( "CDiagBrowserPlugin::IsVisible()" )
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the plugin's level of interactivity.
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagBrowserPlugin::RunMode() const
    {
    LOGSTRING( "CDiagBrowserPlugin::RunMode()" )
    return EAutomatic;
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the total number of execution steps for the plugin.
// ---------------------------------------------------------------------------
TUint CDiagBrowserPlugin::TotalSteps() const
    {
    LOGSTRING( "CDiagBrowserPlugin::TotalSteps()" )
    return CDiagBrowserPluginModel::TotalSteps();
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagBrowserPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    LOGSTRING2( "CDiagBrowserPlugin::GetPluginNameL( %d )", aLayoutType )

    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_BROWSER_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_BROWSER_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_BROWSER_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_BROWSER_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_BROWSER_LIST_SINGLE );

        default:
            __ASSERT_DEBUG( EFalse, Panic( EDiagBrowserPluginName ) );
            return StringLoader::LoadL( R_DIAG_BROWSER_LIST_SINGLE );
        }
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagBrowserPlugin::Uid() const
    {
    LOGSTRING( "CDiagBrowserPlugin::Uid()" )
    return KDiagBrowserPluginUid;
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Called from the base class to begin test execution.
// ---------------------------------------------------------------------------
void CDiagBrowserPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagBrowserPlugin::DoRunTestL()" )

    __ASSERT_ALWAYS( !iModel, Panic( EDiagBrowserPluginBadState ) );
   // ADO & Platformization Changes
    TInt aButtonId;
    
    ShowMessageQueryL(R_DIAG_MESSAGEQUERY_TITLE_BROWSERTEST,aButtonId);
      
    if(aButtonId == EBrowserTestCancel)
    	{
    	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == EBrowserTestSkip)
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

    iModel = CDiagBrowserPluginModel::NewL( *this );
    iModel->RunBrowserTestL();
    }

// ---------------------------------------------------------------------------
// From class CDiagTestPluginBase.
// Called when the plugin's execution is stopped.
// ---------------------------------------------------------------------------
//
void CDiagBrowserPlugin::DoStopAndCleanupL()
    {
    LOGSTRING( "CDiagBrowserPlugin::DoStopAndCleanupL()" )

    delete iModel;
    iModel = NULL;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagBrowserPlugin::RunL()
    {
    LOGSTRING( "CDiagBrowserPlugin::RunL()" )

    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagBrowserPlugin::DoCancel()
    {
    LOGSTRING( "CDiagBrowserPlugin::DoCancel()" )

    }

// ---------------------------------------------------------------------------
// From class MDiagBrowserPluginModelObserver.
// This function notifies that test execution has completed.
// ---------------------------------------------------------------------------
void CDiagBrowserPlugin::TestEndL( CDiagResultsDatabaseItem::TResult aResult )
    {
    LOGSTRING2( "CDiagBrowserPlugin::TestEndL( %d )", aResult )
    CompleteTestL( aResult );

    delete iModel;
    iModel = NULL;
    }

// ---------------------------------------------------------------------------
// From class MDiagBrowserPluginModelObserver.
// This function notifies of test progress.
// ---------------------------------------------------------------------------
void CDiagBrowserPlugin::TestProgressL( TUint aCurrentStep )
    {
    LOGSTRING2( "CDiagBrowserPlugin::TestProgressL( %d )", aCurrentStep )
    ReportTestProgressL( aCurrentStep );
    }
    
    // ADO & Platformization Changes
TBool CDiagBrowserPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_DIAG_MESSAGEQUERY_TITLE_BROWSERTEST: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_CBA_BROWSER_SINGLE_EXECUTION );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_CBA_BROWSER_GROUP_EXECUTION );		
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
