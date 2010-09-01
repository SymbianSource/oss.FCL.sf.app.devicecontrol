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
* Description:  Plug-in view
*
*/


// INCLUDE FILES

#include "devdiagapp.hrh"
#include "devdiagplugincontainer.h"
#include "devdiagengine.h"
#include "devdiagpluginview.h"
#include "devdiagdef.h"
#include "devdiag.pan"
#include "devdiagexecutionresults.h" 

#include <DiagPlugin.h> 
#include <devdiagapp.rsg> //resources
#include <AknQueryDialog.h>
#include <aknradiobuttonsettingpage.h>
#include <aknViewAppUi.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <bautils.h>
#include <coeaui.h>
#include <eikfrlbd.h>
#include <gulicon.h>
#include <hlplch.h>             // For HlpLauncher
#include <akntitle.h> 
#include <aknmessagequerydialog.h> 
#include <aknnotewrappers.h> 
#include <DiagFrameworkDebug.h>         // Debug Logger
#include <DiagResultsDatabaseItem.h> 

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KLinefeed, "\n");
_LIT(KTabfeed, "\t");
const TInt KFailedTestsMaxLength(1024);

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDevDiagPluginView::CDevDiagPluginView()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDevDiagPluginView::CDevDiagPluginView( CDevDiagEngine& aEngine, TUid aParentSuiteUid ):
    iContainer(NULL), iEngine(aEngine), iParentSuiteUid( aParentSuiteUid )
    {
    }


// ---------------------------------------------------------------------------
// CDevDiagPluginView::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
CDevDiagPluginView* CDevDiagPluginView::NewL( CDevDiagEngine& aEngine, 
                                              TUid aParentSuiteUid )
    {
    LOGSTRING("CDevDiagPluginView* CDevDiagPluginView::NewL");
    CDevDiagPluginView* self = new( ELeave ) 
                     CDevDiagPluginView ( aEngine, aParentSuiteUid );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// CDevDiagPluginView::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDevDiagPluginView::ConstructL()
    {
    if ( iParentSuiteUid == KDDHardwareSuiteUid )
        {
        BaseConstructL( R_DD_HW_PLUGIN_VIEW );
        ChangeTitleL( R_DIAG_TITLE_HARDWARE ); 
        }
    else if ( iParentSuiteUid == KDDConnectivitySuiteUid )   
        {
        BaseConstructL( R_DD_CONNECTIVITY_PLUGIN_VIEW );
        ChangeTitleL( R_DIAG_TITLE_CONNECTIVITY );
        }
     else if ( iParentSuiteUid == KDDServicesSuiteUid )   
        {
        BaseConstructL( R_DD_SERVICE_PLUGIN_VIEW );
        ChangeTitleL( R_DIAG_TITLE_SERVICE );
        }
     else if ( iParentSuiteUid == KDDCoverageSuiteUid)   
        {
        BaseConstructL( R_DD_COVERAGE_PLUGIN_VIEW );
        ChangeTitleL( R_DIAG_TITLE_COVERAGE );
        }
     else
        {
        Panic( EDDApplicationUnknownSuite );
        }   
         
    }


// ----------------------------------------------------------------------------
// CDevDiagPluginView::~CDevDiagPluginView
//
// Destructor
// ----------------------------------------------------------------------------
CDevDiagPluginView::~CDevDiagPluginView()
    {

    if ( iContainer )
        {
        delete iContainer;
        }
      
    }


// ---------------------------------------------------------------------------
// TUid CDevDiagPluginView::Id()
//
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CDevDiagPluginView::Id() const
    {
    return KDDPluginViewId;
    }


// ---------------------------------------------------------------------------
// Set context menu bar according to the suite.
// ---------------------------------------------------------------------------
void CDevDiagPluginView::SetContextMenu() 
    {
    if ( iParentSuiteUid == KDDHardwareSuiteUid )
        {
   
        MenuBar()->SetContextMenuTitleResourceId( R_DD_HW_CONTEXT_MENUBAR );
        }
    else if ( iParentSuiteUid == KDDConnectivitySuiteUid )   
        {
        MenuBar()->SetContextMenuTitleResourceId( R_DD_CONNECTIVITY_CONTEXT_MENUBAR );
        }
    else if ( iParentSuiteUid == KDDServicesSuiteUid )   
        {
        MenuBar()->SetContextMenuTitleResourceId( R_DD_SERVICE_CONTEXT_MENUBAR );
        }
    else if ( iParentSuiteUid == KDDCoverageSuiteUid)   
        {
        MenuBar()->SetContextMenuTitleResourceId( R_DD_COVERAGE_CONTEXT_MENUBAR );
        }
    else
        {
        Panic( EDDApplicationUnknownSuite );
        }   
    }
// ---------------------------------------------------------------------------
// CDevDiagPluginView::HandleCommandL(TInt aCommand)
//
// Handles commands directed to this class.
// ---------------------------------------------------------------------------
void CDevDiagPluginView::HandleCommandL( TInt aCommand )
    {
    LOGSTRING("CDevDiagPluginView::HandleCommandL");
    switch ( aCommand )
        {

        case EDDCmdPerformSingle:
            {
            // State == EStateReady then run tests
            if (iEngine.GetState()== 2)   
	            {        
					LOGSTRING("iState == EStateReady");
		            iContainer->DisplayStartTestingL();
		            
		            MDiagPlugin* plugin = NULL;
		            iContainer->SelectedPlugin(plugin);
		       
		            iEngine.ExecuteTestL( plugin->Uid(), *AppUi() );
	            }
	        else
                {
                LOGSTRING2("PluginView::iEngine::iState = %d ", iEngine.GetState());
                LOGSTRING ("Trying to Execute Plugin @ Invalid State");
                }
            break;
            }

        case EAknSoftkeyBack:
            {
            if (iEngine.GetState()== 2)
              {
            LOGSTRING("Before Activate Local View for Back");
            
            AppUi()->ActivateLocalViewL( KDDMainViewId );
            
            LOGSTRING("After Activate Local View for Back");
	            
              }
            
			else
			  {
				LOGSTRING2("PluginView::iEngine::iState = %d ", iEngine.GetState());
				LOGSTRING ("Trying to issue Back @ Invalid State");
			  }
            break;
            }
            
        case EDDMenuCmdMainHelp:
        	{
        		MessageQueryDialogLaunchL( 
                                R_MESSAGE_QUERY_HELP_NOTE );
        		break;
        	}
            
        case EAknCmdHelp:
            {
                                    
      
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                AppUi()->AppHelpContextL() );
                }
                
            break;
            }
            
        case EDDCmdPerformAllHwTests:
            {                        
            //ask confirmation from the user
            TBool ok = MessageQueryDialogLaunchL( 
                                R_MESSAGE_QUERY_PERFORM_ALL_HW_TESTS );                                            
            if ( ok )
                {
                iContainer->ResetL();
                iContainer->DisplayStartTestingL();
                //Engine will populate the suite 
                iEngine.ExecuteTestL( KDDHardwareSuiteUid,  *AppUi() );                                                            
                }
           
            break;
            }
            
        case EDDCmdPerformAllCoverageTests:
            {
            
            //ask confirmation from the user
            TBool ok = MessageQueryDialogLaunchL( 
                                R_MESSAGE_QUERY_PERFORM_ALL_COVERAGE_TESTS );                                            
           if ( ok )
                {
                //Engine will populate the suite 
                iEngine.ExecuteTestL( KDDCoverageSuiteUid,  *AppUi() );                                                            
                }           
                                            
            break;
            }
            
        //These have only one test, so performing all is not needed yet    
        case EDDCmdPerformAllServiceTests:
            {
            break;            
            }
            
        case EDDCmdPerformAllConnTests:
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
// CDevDiagPluginView::DoActivateL
//
// First method called by the Avkon framwork to invoke a view.
// ----------------------------------------------------------------------------
//
void CDevDiagPluginView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                     TUid               /*aCustomMessageId*/ ,
                                     const TDesC8&      /*aCustomMessage*/ )
    {    
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );

        delete iContainer;
        iContainer = NULL;
        }

    iContainer = CDevDiagPluginContainer::NewL( this, iEngine, ClientRect() );
    iContainer->SetMopParent( this );
    AppUi()->AddToStackL( iContainer );
    
    iEngine.SetObserver( this );
    
    //Construct the model
    iContainer->UpdateModelL( iParentSuiteUid );
    
    SetContextMenu();
    
    //Retrieve last test results. HandleEngineCommandL with
    //EDevDiagEngineCommandGetLastResults is called after this.
    iEngine.GetLastResultsL( iParentSuiteUid );
    }

// ----------------------------------------------------------------------------
// CDevDiagPluginView::DoDeactivate
//
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CDevDiagPluginView::DoDeactivate()
    {
   if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );        
        delete iContainer;
        iContainer = NULL;
        }    
    }


// ----------------------------------------------------------------------------
// Handle CDevDiagEngine events. 
//
// EDevDiagEngineCommandTestExecutionDone is called when single or group 
// execution finishes.
//
// MDevDiagEngineObserver::EDevDiagEngineCommandResultsDataUpdated is called
// when a plug-in is executed.
//
// EDevDiagEngineCommandGetLastResults is called after 
// iEngine.GetLastResultsL( iParentSuiteUid ) is called. It retrieves the 
// last results.
// ----------------------------------------------------------------------------
//    
void CDevDiagPluginView::HandleEngineCommandL( TAppEngineCommand aCommand,
                                       TInt aError,
                                       TAny* aCustom )
    {
    LOGSTRING("CDevDiagPluginView::HandleEngineCommandL");
    switch ( aCommand )
        {
        case MDevDiagEngineObserver::EDevDiagEngineCommandGroupExecutionDone:
            {
            if ( aError != KErrNone )
                {
                LOGSTRING2("MDevDiagEngineObserver::Exec Done Error: %d: ", aError);
                return;
                }
            
            iEngine.GetLastResultsL( iParentSuiteUid );
                                    
            if ( iEngine.HasExecutionResults() )
                {
                TInt failedCount = iEngine.ExecutionResults().FailedResultsCount();
                
                TInt passedCount = iEngine.ExecutionResults().PassedResultsCount();
                
                if ( failedCount == 0 && passedCount > 0)
                    {
                    DisplayConfirmationDialogL(R_DIAG_CONF_TEST_PASSED);
                    }
                else if(failedCount > 0)
                    {
                    DisplayMessageQueryDialogFailedResultsL();
                    }                
                }
                
            break;
            }        
            
        case MDevDiagEngineObserver::EDevDiagEngineCommandGroupExecutionProgress:
            {     
            if ( aError != KErrNone )
                {
                LOGSTRING2("MDevDiagEngineObserver::ExecutionProgress Error: %d: ", aError);
                return;
                }
                      
            iEngine.GetLastResultsL( iParentSuiteUid ); 
            
            iContainer->MoveStartTestingDownL();
                 
            break;   
            }
        case MDevDiagEngineObserver::EDevDiagEngineCommandGetLastResults:
            {            
            if ( aError != KErrNone )
                {
                LOGSTRING2("MDevDiagEngineObserver::GetLastResults Error: %d: ", aError);
                return;
                }
            
            RPointerArray<CDiagResultsDatabaseItem>* array = 
                static_cast<RPointerArray<CDiagResultsDatabaseItem>*> (aCustom);
            iContainer->DisplayChildrenWithResultsL( *array );
           
            
            if ( iEngine.HasExecutionResults() && !iEngine.ExecutionResults().LastTestFinishedL())
                {
                iContainer->MoveStartTestingDownL();
                }            
           
                                          
            break;
            }       
        case MDevDiagEngineObserver::EDevDiagEngineCommandSinglePluginExecutionDone:
            {
			
			if(!iEngine.GetPluginDependencyL())
				iEngine.GetLastResultsL( iParentSuiteUid );
            
            if ( aError != KErrNone )
                {
                LOGSTRING2("MDevDiagEngineObserver::SinglePluginExecutionDone Error: %d: ", aError);
                return;
                }
            
            //iEngine.GetLastResultsL( iParentSuiteUid );  // ADO & Platformization Changes
            
            //  ADO & Platformization Changes
            
            ///@@@KSR: changes for Codescanner error val = High
            //if(iEngine.ExecutionResults().SinglePluginExecutionL() && !iEngine.GetPluginDependency())
            if(iEngine.ExecutionResults().SinglePluginExecutionL() && !iEngine.GetPluginDependencyL())
	            {
	        
	            const CDiagResultsDatabaseItem* aResult;
	            aResult = iEngine.ExecutionResults().GetSinglepluginExecutionResult();
	            
	            
	            if(aResult->TestResult() == CDiagResultsDatabaseItem::ESuccess)
	            	{
	            	DisplayConfirmationDialogL(R_DIAG_CONF_TEST_PASSED_1);
	            	LOGSTRING("The test has passed");
	            	}
	            else if(aResult->TestResult() == CDiagResultsDatabaseItem::EFailed)
	            	{
	            	DisplayConfirmationDialogL(R_DIAG_CONF_TEST_FAILED_1);	
	            	LOGSTRING("The test has failed");
	            	}
	            }
            
	            
	          // Changes ends
                                                                                                     
            break;
            }
            
        case MDevDiagEngineObserver::EDevDiagEngineCommandTestExecutionCancelled:
            {                                    
            //Update container.
            iEngine.GetLastResultsL( iParentSuiteUid );
            
            if ( iEngine.HasExecutionResults() )
                {
                TInt failedCount = iEngine.ExecutionResults().FailedResultsCount();
                
                TInt passedCount = iEngine.ExecutionResults().PassedResultsCount();
                
                if ( failedCount == 0 && passedCount > 0)
                    {
                    DisplayConfirmationDialogL(R_DIAG_CONF_TEST_PASSED);
                    }
                else if(failedCount > 0)
                    {
                    DisplayMessageQueryDialogFailedResultsL();
                    }                
                }                
                            
            break;
            }
        }
    }


// ----------------------------------------------------------------------------
// Change the title of the application. 
//
// ----------------------------------------------------------------------------
//
void CDevDiagPluginView::ChangeTitleL(TInt aResourceId) 
	{
	CAknTitlePane* titlePane = 
	    (CAknTitlePane*)(AppUi()->StatusPane())->ControlL( 
	                                    TUid::Uid( EEikStatusPaneUidTitle ) );
	                                    
	HBufC* titleText = iEikonEnv->AllocReadResourceLC( aResourceId );
	titlePane->SetTextL( titleText->Des() );
	CleanupStack::PopAndDestroy(titleText);
	}  
	

// ----------------------------------------------------------------------------
// Display a message query dialog. 
//
// ----------------------------------------------------------------------------
// 	
TBool CDevDiagPluginView::MessageQueryDialogLaunchL( TInt aResourceId )
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
// This is needed after performing a group of tests succesfully.
// ----------------------------------------------------------------------------
// 	
void CDevDiagPluginView::DisplayConfirmationDialogL(TInt aResourceId)
	{	    
    CAknConfirmationNote* note = 
        new (ELeave) CAknConfirmationNote( ETrue ); //Waiting

    HBufC* text = NULL;
   
    text = StringLoader::LoadLC( aResourceId );
          
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
void CDevDiagPluginView::DisplayMessageQueryDialogFailedResultsL()
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
HBufC* CDevDiagPluginView::CreateResultsTextsForFailedTestsLC( 
                                    const CDevDiagExecResults& aExecResults )
    {        
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
                resultText.Append( KTabfeed );
                
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
    
// End of File
