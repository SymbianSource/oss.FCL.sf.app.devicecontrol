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
* Description:  Main view implementation. 
*
*/



#include "devdiagsuitecontainer.h"
#include "devdiagengine.h"
#include "devdiagdef.h"
#include "devdiagpluginview.h"
#include "devdiagexecutionresults.h"
#include "devdiag.pan"

#include <DiagPlugin.h> 
#include <akntitle.h> 
#include <coemain.h>
#include <devdiagapp.rsg>
#include <aknViewAppUi.h> 
#include <eikbtgpc.h> 			//Button group container
#include <aknmessagequerydialog.h> 
#include <DiagFrameworkDebug.h>         // Debug Logger
#include <aknnotewrappers.h> 
#include <StringLoader.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DiagPluginPool.h> 

#include <DiagPlugin.h> 
#include <AknQueryDialog.h>
#include <aknViewAppUi.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <bautils.h>
#include <coeaui.h>
#include <akntitle.h> 
#include <aknmessagequerydialog.h> 
#include <aknnotewrappers.h> 
#include <DiagResultsDatabaseItem.h>   

#ifdef __SERIES60_HELP
#include <hlplch.h>
#endif

#include "devdiagsuiteview.h"

const TInt KRootSuite = 0x2000E540; //top level suite

_LIT(KLinefeed, "\n");
_LIT(KTabfeed, "\t");
const TInt KFailedTestsMaxLength(1024);

// ----------------------------------------------------------------------------
// Standard symbian c++ constructor
// ----------------------------------------------------------------------------
CDevDiagSuiteView* CDevDiagSuiteView::NewL( CDevDiagEngine& aEngine )
    {
    //LOGSTRING("CDevDiagSuiteView* CDevDiagSuiteView::NewL");
    CDevDiagSuiteView* self = CDevDiagSuiteView::NewLC( aEngine );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Standard symbian c++ constructor
// ----------------------------------------------------------------------------
// 
CDevDiagSuiteView* CDevDiagSuiteView::NewLC( CDevDiagEngine& aEngine )
    {
    //LOGSTRING("CDevDiagSuiteView* CDevDiagSuiteView::NewLC");
    CDevDiagSuiteView* self = new (ELeave) CDevDiagSuiteView(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL.
// ----------------------------------------------------------------------------
// 
void CDevDiagSuiteView::ConstructL()
    {
    BaseConstructL( R_GS_DD_VIEW );
    ChangeTitleL ( R_GS_DIAGNOSTICS_VIEW_CAPTION );        
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
// 
CDevDiagSuiteView::CDevDiagSuiteView(CDevDiagEngine& aEngine): 
        iContainer ( NULL ),
        iEngine ( aEngine ),
        iParentPlugin( TUid::Uid( KRootSuite ))      
    {
    aEngine.SetObserver(this);
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
// 
CDevDiagSuiteView::~CDevDiagSuiteView()
   {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        } 
    }

// ----------------------------------------------------------------------------
// Engine observer.
// ----------------------------------------------------------------------------
// 
void CDevDiagSuiteView::HandleEngineCommandL( TAppEngineCommand aCommand,
                                       TInt aError,
                                       TAny* aCustom )
    {
    switch ( aCommand )
        {
        case MDevDiagEngineObserver::EDevDiagEngineCommandPluginLoadComplete:
            {                        
            
            if ( iWaitDialog )
                {                
                DismissWaitDialogPreparingL();
                }
            
            CheckCrashedTestL();    
                
            break;
            }
        case MDevDiagEngineObserver::EDevDiagEngineCommandPluginLoadProgress:
            {
            MDiagPlugin* plugin = static_cast<MDiagPlugin*>(aCustom);
            
            if ( SuiteUid( plugin->Uid() ))
                {
                iContainer->DisplayPluginL( plugin );    
                }
            
            break;
            }
            
        case MDevDiagEngineObserver::EDevDiagEngineCommandGroupExecutionDone:
            {
            if ( aError != KErrNone )
                {
                LOGSTRING2("MDevDiagEngineObserver::Exec Done Error: %d: ", aError);
                return;
                }
                                                            
            if ( iEngine.HasExecutionResults() )
                {
                TInt failedCount = iEngine.ExecutionResults().FailedResultsCount();
                
                if ( failedCount == 0 )
                    {
                    DisplayConfirmationDialogL();
                    }
                else 
                    {
                    DisplayMessageQueryDialogFailedResultsL();
                    }                        
                }
                
            break;   
            }
        }
    }
    
// ----------------------------------------------------------------------------
// Application ID.
// ----------------------------------------------------------------------------
//     
TUid CDevDiagSuiteView::Id() const
    {
    return KDDMainViewId;
    }


// ----------------------------------------------------------------------------
// Change the title of the application.
// ----------------------------------------------------------------------------
//     
void CDevDiagSuiteView::ChangeTitleL(TInt aResourceId) 
	{
	CAknTitlePane* titlePane = 
	    (CAknTitlePane*)(AppUi()->StatusPane())->ControlL( 
	    TUid::Uid( EEikStatusPaneUidTitle ) );
	    
	HBufC* titleText = iEikonEnv->AllocReadResourceLC( aResourceId );
	titlePane->SetTextL( titleText->Des() );
	CleanupStack::PopAndDestroy(titleText);
	}    

// ----------------------------------------------------------------------------
// Handle commands.
// ----------------------------------------------------------------------------
// 
void CDevDiagSuiteView::HandleCommandL( TInt aCommand )
    {
    LOGSTRING2("CDevDiagSuiteView::HandleCommandL %d",aCommand);
    switch ( aCommand )
        {
        case EDDCmdOpen:
            {	
            	//Selected suite plug-in
            	MDiagPlugin* plugin;
            	
            	iContainer->SelectedPlugin( plugin );
                        
            	if ( iPluginView )
                   {
                   AppUi()->RemoveView( KDDPluginViewId );                	
                    }
            
                iPluginView = CDevDiagPluginView::NewL( iEngine, plugin->Uid() );
            
                AppUi()->AddViewL( iPluginView );
            
                AppUi()->ActivateLocalViewL( KDDPluginViewId );
            
            }
            
            break;
            
        case EDDCmdPerformAllTests:
            {
            	
            
            	TBool ok = MessageQueryDialogLaunchL( R_MESSAGE_QUERY_PERFORM_ALL_TESTS );
            
            	if ( ok )
                	{                
                	//Engine will populate the suite.
                	//because the plug-in is root suite, all tests will be executed.
                	iEngine.ExecuteTestL( iParentPlugin,  *AppUi() );     
                	}
           
            break;
            }
            
        case EDDMenuCmdMainHelp:
        	{
        	/*	MessageQueryDialogLaunchL( 
                                R_MESSAGE_QUERY_HELP_NOTE );*/
        	
			HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
			                         AppUi()->AppHelpContextL() );                                
            break;	
        	}
 
        case EAknCmdHelp:
            {            
            break;
            }
	case EAknCmdTaskSwapper:
            {
    	/*
    	* MSK: Active Applications
    	* This command is actually handled by Avkon FW
    	* Applications should not handle this
    	*/
    	    break;
    	    }
        default:
            AppUi()->HandleCommandL( aCommand );
            break;
        }
    }


// ----------------------------------------------------------------------------
// Identify is it a suite or not.
// ----------------------------------------------------------------------------
// 
TBool CDevDiagSuiteView::SuiteUid( TUid aUid )
    {
    
    if ( aUid == KDDHardwareSuiteUid )
        {
        return ETrue;
        }
    else if ( aUid == KDDConnectivitySuiteUid )   
        {
        return ETrue;
        }
    else if ( aUid == KDDServicesSuiteUid )   
        {
        return ETrue;
        }
    else if ( aUid == KDDCoverageSuiteUid )   
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }   
    }

// ----------------------------------------------------------------------------
// First method called by the Avkon framwork to invoke a view.
// ----------------------------------------------------------------------------
//
void CDevDiagSuiteView::DoActivateL( const TVwsViewId& aPrevViewId,
                                     TUid /*aCustomMessageId*/,
                                     const TDesC8& /*aCustomMessage*/ )
    {
    LOGSTRING( "CDevDiagSuiteView::DoActivateL");
    ChangeTitleL ( R_GS_DIAGNOSTICS_VIEW_CAPTION );
    
    iPrevViewId = aPrevViewId;
    
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );

        delete iContainer;
        iContainer = NULL;
        }
        
    iContainer = CDevDiagSuiteContainer::NewL( this, iEngine, ClientRect() );
    iContainer->SetMopParent( this );
    AppUi()->AddToStackL( iContainer );
    
    iEngine.SetObserver(this);
     
    if ( !iEngine.ArePluginsLoaded() ) 
        {
        DisplayWaitDialogPreparingL();    
        }
    else 
        {
        //display children of the top level suite.
        iContainer->DisplayChildrenL( iParentPlugin );     
        }                
    
    if ( iEikonEnv->StartedAsServerApp() )
        {
        //UpdateCbaL( R_PROFILE_MAIN_VIEW_CBA_2 );    
        }
    
    LOGSTRING( "CDevDiagSuiteView::DoActivateL end");
    }

// ----------------------------------------------------------------------------
// Update command buttons
// ----------------------------------------------------------------------------
// 	
void CDevDiagSuiteView::UpdateCbaL(TInt aResourceId)
    {
    CEikButtonGroupContainer* cba = Cba();
    cba->SetCommandSetL(aResourceId);
    cba->DrawDeferred();
    }

// ----------------------------------------------------------------------------
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CDevDiagSuiteView::DoDeactivate()
    {
   if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
            
    }

// ----------------------------------------------------------------------------
// Display message query dialog. 
//
// ----------------------------------------------------------------------------
// 	
TBool CDevDiagSuiteView::MessageQueryDialogLaunchL( TInt aResourceId )
	{	
    CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog(); 
   
    dlg->PrepareLC( aResourceId ); 

    if (dlg->RunLD())
    	{	    
    	return ETrue;
    	}
    	
	return EFalse;
	}


// ----------------------------------------------------------------------------
// Display a confirmation dialog.
//
// ----------------------------------------------------------------------------
// 
void CDevDiagSuiteView::DisplayConfirmationDialogL( TBool aPassed )
	{	    
    CAknConfirmationNote* note = 
        new (ELeave) CAknConfirmationNote( ETrue ); //Waiting

    HBufC* text = NULL;

    if ( aPassed )
        {
        text = StringLoader::LoadL( R_DIAG_CONF_TEST_PASSED  );
        }
    else 
        {
        text = StringLoader::LoadL( R_DIAG_CONF_TEST_FAILED  );
        }

    CleanupStack::PushL( text );

    note->ExecuteLD( *text ); //Blocks until note dismissed

    CleanupStack::PopAndDestroy();
	}	
	

// ---------------------------------------------------------------------------
// Display a wait dialog to inform the user we are preparing for execution.
// ---------------------------------------------------------------------------
//
void CDevDiagSuiteView::DisplayWaitDialogPreparingL()
    {
    LOGSTRING( "CDevDiagSuiteView::DisplayWaitDialogPreparingL()" );

    __ASSERT_DEBUG( !iWaitDialog, Panic( EDevDiagApplicationGeneral ) );
 
    // Create the wait dialog and run it. The dialog will only display if
    // checking runtime requirements takes more than 1.5 seconds.
    iWaitDialog = new (ELeave) CAknWaitDialog(
        reinterpret_cast< CEikDialog** >( &iWaitDialog ),
        EFalse );
    iWaitDialog->ExecuteLD( R_DEV_DIAG_PREPARING_WAIT_NOTE );

    }
	
// ---------------------------------------------------------------------------
// Display a confirmation dialog to inform the user there are no logged
// results available.
// ---------------------------------------------------------------------------
//
void CDevDiagSuiteView::DismissWaitDialogPreparingL()
    {
    LOGSTRING( "CDevDiagSuiteView::DismissWaitDialogPreparingL()" )

    __ASSERT_DEBUG( iWaitDialog, Panic( EDevDiagApplicationGeneral ) );

    // Stop the wait dialog.
    iWaitDialog->ProcessFinishedL();
    iWaitDialog = NULL;
    
    }

// ---------------------------------------------------------------------------
// Check and display a dialog if there is a crashed test record. Only the last
// crashed test record is counted.
// ---------------------------------------------------------------------------
//
void CDevDiagSuiteView::CheckCrashedTestL()
    {
    LOGSTRING("CDevDiagSuiteView::CheckCrashedTestL");
    TUid pluginUid;
    TBool found = EFalse;
    TInt error = KErrNone;
    found = iEngine.CrashedPluginL( pluginUid );
    LOGSTRING2("CrashedPlugin Found = %d", found);
    if ( found )
        {        
        //Complete the record. 
        ///@@@KSR: changes for Codescanner error val = High 
        //error = iEngine.CompleteCrashedTestRecord();
        error = iEngine.CompleteCrashedTestRecordL();
        
        if ( error != KErrNone )
            {
            LOGSTRING2( "CDevDiagSuiteView::CheckCrashedTestL 1() error: %d", error );
            Panic( EDevDiagCheckCrashedTest );
            }
        
        MDiagPlugin* plugin = NULL;
        error = iEngine.PluginPool().FindPlugin( pluginUid, plugin );
        
        HBufC* name = plugin->GetPluginNameL( MDiagPlugin::ENameLayoutPopupInfoPane );
        CleanupStack::PushL( name );
        
        HBufC* text = StringLoader::LoadLC( R_DIAG_CONF_QUERY_AFTER_CRASH, *name );
    	
        CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog;
        
        dlg->PrepareLC( R_DEVDIAG_EXECUTE_CRASHED_TEST_MESSAGEQUERY_DIALOG ); 
         
        dlg->SetMessageTextL( *text );
        
        HBufC* headerText = plugin->GetPluginNameL( MDiagPlugin::ENameLayoutHeadingPane );
        CleanupStack::PushL( headerText );
        
        dlg->SetHeaderTextL( *headerText );
        
        CleanupStack::PopAndDestroy();

        if (dlg->RunLD())
    	    {	    
    	    //Create a new test run. 
    	    iEngine.ExecuteTestL( pluginUid,  *AppUi() );  
    	    }
    		                                                
        CleanupStack::PopAndDestroy();    
        CleanupStack::PopAndDestroy();
        }    
    }


// ----------------------------------------------------------------------------
// This is needed after performing a group of tests succesfully.
// ----------------------------------------------------------------------------
// 	
void CDevDiagSuiteView::DisplayConfirmationDialogL( )
	{	    
    CAknConfirmationNote* note = 
        new (ELeave) CAknConfirmationNote( ETrue ); //Waiting

    HBufC* text = NULL;
   
    text = StringLoader::LoadLC( R_DIAG_CONF_TEST_PASSED );
          
    note->ExecuteLD( *text ); //Blocks until note dismissed

    CleanupStack::PopAndDestroy();
	}	
	

// ----------------------------------------------------------------------------
// This is needed after performing a group of tests
//
// Display dialog text according to the test result.
//
// Message query dialog will display a list of failed test cases. 
// ----------------------------------------------------------------------------
// 		
void CDevDiagSuiteView::DisplayMessageQueryDialogFailedResultsL()
	{	
    CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog(); 
   
    dlg->PrepareLC( R_DEVDIAG_FAILED_TESTS_MESSAGEQUERY_DIALOG ); 

    HBufC* failText = StringLoader::LoadL( R_DIAG_CONF_TEST_FAILED  );
    CleanupStack::PushL( failText );
        
    HBufC* failedTests = CreateResultsTextsForFailedTestsLC( iEngine.ExecutionResults() );
        
    TInt newBufLength = failText->Length() + failedTests->Length();
        
    HBufC* text = HBufC::NewLC( newBufLength );        
        
    text->Des().Append( *failText );
    text->Des().Append( *failedTests );
    
    dlg->SetMessageTextL( *text );
        
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();    

    if (dlg->RunLD())
    	{	    
    	
    	}    		
	}
	
	
// ----------------------------------------------------------------------------
// Loads names of the tests that failed.
// ----------------------------------------------------------------------------
// 		
HBufC* CDevDiagSuiteView::CreateResultsTextsForFailedTestsLC( 
                                    const CDevDiagExecResults& aExecResults )
    {
    LOGSTRING("CDevDiagSuiteView::CreateResultsTextsForFailedTestsLC");        
    RBuf resultText;
    CleanupClosePushL( resultText );
    
    resultText.CreateL( KFailedTestsMaxLength );
    
    TBool firstLineFeed = ETrue;
    
    for ( TInt i = 0; i < aExecResults.Count(); ++i )
        {
        const CDevDiagExecResults::CResult& result = aExecResults[i];
        
        //if item == NULL it is a test suite
        const CDiagResultsDatabaseItem* item = result.Result();
        
        if (item && !item->WasDependency())
            {
        
        //List failed test cases
            if ( item && item->TestResult() == CDiagResultsDatabaseItem::EFailed )
                {
                const MDiagPlugin& plugin = result.Plugin();
                
                //Note the correct layout. Name is used in the pop-up info pane.
                HBufC* name = plugin.GetPluginNameL( MDiagPlugin::ENameLayoutPopupInfoPane );
                CleanupStack::PushL( name );
                
                if ( resultText.MaxLength() < resultText.Length() + name->Length() )
                    {
                    HBufC* temp = resultText.AllocLC();
                    resultText.ReAlloc( resultText.Length() + KFailedTestsMaxLength );
                    resultText.Append( *temp );
                    CleanupStack::PopAndDestroy();
                    }
                
                if ( !firstLineFeed )
                    {
                    resultText.Append( KLinefeed );
                    }
                else 
                    {
                    firstLineFeed = EFalse;        
                    }
                                
                resultText.Append( *name );
                resultText.Append(KTabfeed);
                
                CleanupStack::PopAndDestroy();
                }
            }
        }
            
    //contains combined result texts                
    HBufC* result = resultText.AllocL(); 
    
    CleanupStack::PopAndDestroy();
    
    CleanupStack::PushL( result );    
    return result;
    }
//End of file
