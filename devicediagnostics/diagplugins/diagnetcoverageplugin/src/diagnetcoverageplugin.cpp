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
* Description:  Diagnostics Framework Plug-in to test network coverage on
                  the currently locked channel.
*
*/


// CLASS DECLARATION
#include "diagnetcoverageplugin.h"

// SYSTEM INCLUDE FILES
#include <centralrepository.h>
#include <StringLoader.h>               // StringLoader

#include <DiagTestObserver.h>           // MDiagTestObserver
#include <DiagTestExecParam.h>          // TDiagTestExecParam
#include <DiagResultsDbItemBuilder.h>   // CDiagResultsDbItemBuilder
#include <DiagResultDetailBasic.h>      // CDiagResultDetailBasic
#include <DiagFrameworkDebug.h>         // LOGSTRING
#include <devdiagnetcoveragepluginrsc.rsg>

// USER INCLUDE FILES
#include "diagnetcoveragepluginprivatecrkeys.h"


// ADO & Platformization Changes
#include <DiagFrameworkDebug.h>                     // debug macros
#include <DiagEngineCommon.h>                       // MDiagEngineCommon
#include <DiagCommonDialog.h> 
#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagPluginExecPlan.h>
#include <DiagExecPlanEntry.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

static const TInt KTotalTestSteps = 2;
_LIT( KDiagNetCoveragePluginResourceFileName, "z:DevDiagNetCoveragePluginRsc.rsc" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDiagNetCoveragePlugin::CDiagNetCoveragePlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagNetCoveragePlugin::CDiagNetCoveragePlugin( CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam ),
        iNetworkInfoV1(),
        iSignalStrengthV1(),
        iNetworkInfoV1Pckg( iNetworkInfoV1 ),
        iSignalStrengthV1Pckg( iSignalStrengthV1 )
    {
    }


// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagNetCoveragePlugin::NewL( TAny* aInitParams )
    {
    
    LOGSTRING( "CDiagNetCoveragePlugin::NewL" )

    ASSERT( aInitParams != NULL );
            
    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>( aInitParams );

    CleanupStack::PushL( param );
    CDiagNetCoveragePlugin* self = new( ELeave ) CDiagNetCoveragePlugin( param );
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }



// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagNetCoveragePlugin::ConstructL()
    {
    LOGSTRING("CDiagNetCoveragePlugin::ConstructL");        
    
    BaseConstructL( KDiagNetCoveragePluginResourceFileName );
    
    ReadThresholdValuesL();                
    idialogOn = EFalse;              
    }


// ----------------------------------------------------------------------------
// CDiagNetCoveragePlugin::~CDiagNetCoveragePlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagNetCoveragePlugin::~CDiagNetCoveragePlugin()
    {
    // Base class will call StopAndCleanup(), which callso
    //  Cancel() -> DoCancel()
    //  DoStopAndCleanupL()
    if ( iWaitDialog )
    delete iWaitDialog;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagNetCoveragePlugin::IsVisible()
// ---------------------------------------------------------------------------
TBool CDiagNetCoveragePlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagNetCoveragePlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagNetCoveragePlugin::RunMode() const
    {
    return EAutomatic;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagNetCoveragePlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagNetCoveragePlugin::TotalSteps() const
    {
    return KTotalTestSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagNetCoveragePlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagNetCoveragePlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_NET_COVERAGE_PLUGIN_NAME );       
        
        case ENameLayoutHeadingPane:
             return StringLoader::LoadL ( R_QTN_DIAG_MSG_TITLE_VOICE_NETWORK );
        
        case ENameLayoutPopupInfoPane:
              return StringLoader::LoadL ( R_QTN_DIAG_MSG_INFO_VOICE_NETWORK );
        
        case ENameLayoutTitlePane:
             return StringLoader::LoadL ( R_QTN_DIAG_TITLE_TEST_VOICE_NETWORK );
        
        case ENameLayoutListSingleGraphic:
             return StringLoader::LoadL ( R_QTN_DIAG_LST_TEST_VOICE_NETWORK );

        default:
            LOGSTRING2( "CDiagNetCoveragePlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_NET_COVERAGE_PLUGIN_NAME );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagNetCoveragePlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagNetCoveragePlugin::Uid() const
    {
    return KDiagNetCoveragePluginUid;
    }
    

// ---------------------------------------------------------------------------
// From CActive
// CDiagNetCoveragePlugin::RunL
// ---------------------------------------------------------------------------
void CDiagNetCoveragePlugin::RunL()
    {
    ReportTestProgressL( iProgressCounter++ );
    
    switch (iState)
        {
        case EStateWaitForBandType:                    
            CheckNetworkAccessL();
            break;
            
        case EStateWaitForSignalStrength:
            CheckSignalStrengthL();                      
            break;
		
		case EStateTestCancel:
			 CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
			 break;
        default:
            // Unsupported state
            ASSERT(0);
            break;
        }
        
    }  

// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagNetCoveragePlugin::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagNetCoveragePlugin::RunError( %d )", aError )    
		return KErrNone;
	}
	    
// ---------------------------------------------------------------------------
// From CActive
// CDiagNetCoveragePlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagNetCoveragePlugin::DoCancel()
    {   
    LOGSTRING("CDiagNetCoveragePlugin::DoCancel");
            
    TInt result;
    
    // Cancel request to CTelephony
    switch ( iState )    
        {
        case EStateWaitForBandType:                                
            result = iTelephony->CancelAsync( CTelephony::EGetCurrentNetworkInfoCancel );
            ASSERT( result == KErrNone );
            break;
            
        case EStateWaitForSignalStrength:
            result = iTelephony->CancelAsync( CTelephony::EGetSignalStrengthCancel );
            ASSERT( result == KErrNone );
            break;
            
        default:
            // No cancel needed
            if (result == KErrNone)
            LOGSTRING("docancel() result");
            break;
        }                             
            
    }    

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagNetCoveragePlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagNetCoveragePlugin::DoRunTestL()
    {
    LOGSTRING("CDiagNetCoveragePlugin::DoRunTestL");
    
     // ADO & Platformization Changes
    if(!(ExecutionParam().Engine().ExecutionPlanL().CurrentExecutionItem().AsDependency()))
    {
    	
     
    TInt aButtonId;
    
    ShowMessageQueryL(R_DIAG_MESSAGEQUERY_TITLE_NETCOVERAGE,aButtonId);
      
    if(aButtonId == ENetcoverageTestCancel)
    	{
    	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == ENetcoverageTestSkip)
    	{
		TInt confirmResult = 0;
            
        CAknDialog* dlg = ExecutionParam().Engine().
        CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
    
        if ( !RunWaitingDialogL(dlg, confirmResult ) )
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
    
    iProgressCounter = 1;
    iNetworkType = ETypeUnknown;
    iNetworkBand = EBandUnknown;
    
    // ones to use because they are less strict
    iNetworkBand = EBand1800_1900_2100;
    ObtainBandTypeAsyncL();
    // Wait note display only if itz not a dependency execution.
    if(!(ExecutionParam().Engine().ExecutionPlanL().CurrentExecutionItem().AsDependency()))
    ShowProgressNoteL();
    }     
    
// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::DoStopAndCleanupL()
// ---------------------------------------------------------------------------    
void CDiagNetCoveragePlugin::DoStopAndCleanupL()
    {    
    LOGSTRING("CDiagNetCoveragePlugin::DoStopAndCleanupL");
    
    delete iTelephony;
    iTelephony = NULL;
    idialogOn = EFalse;   
    iState = EStateTestCompleted;            
    }
    
// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::ObtainBandTypeAsyncL()
// ---------------------------------------------------------------------------     
void CDiagNetCoveragePlugin::ObtainBandTypeAsyncL()
    {    
    LOGSTRING("CDiagNetCoveragePlugin::ObtainBandTypeAsyncL");
    
    ASSERT( iTelephony == NULL );
    
    iTelephony = CTelephony::NewL();    
    iTelephony->GetCurrentNetworkInfo( iStatus, iNetworkInfoV1Pckg );
    iState = EStateWaitForBandType;
        
    SetActive();    
    }

// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::ReadThresholdValuesL()
// ---------------------------------------------------------------------------  
void CDiagNetCoveragePlugin::ReadThresholdValuesL()
    {
    LOGSTRING("CDiagNetCoveragePlugin::ReadThresholdValuesL");
    
    CRepository* repository = CRepository::NewLC( KCRUidNetworkCoverage );       

    LOGSTRING("CDiagNetCoveragePlugin::ReadThresholdValuesL:");
    User::LeaveIfError( repository->Get(KPhoneDoctorGSM850SigStrThreshold, 
                            iThresholdValues[ETypeGSM][EBand900_850]) );
    LOGSTRING2("\tKPhoneDoctorGSM850SigStrThreshold=%.2f",iThresholdValues[ETypeGSM][EBand900_850]);
    User::LeaveIfError( repository->Get(KPhoneDoctorGSM1900SigStrThreshold, 
                            iThresholdValues[ETypeGSM][EBand1800_1900_2100]) );
    LOGSTRING2("\tKPhoneDoctorGSM1900SigStrThreshold=%.2f",iThresholdValues[ETypeGSM][EBand1800_1900_2100]);
    User::LeaveIfError( repository->Get(KPhoneDoctorUMTS850SigStrThreshold, 
                            iThresholdValues[ETypeUMTS][EBand900_850]) );
    LOGSTRING2("\tKPhoneDoctorUMTS850SigStrThreshold=%.2f",iThresholdValues[ETypeUMTS][EBand900_850]);
    User::LeaveIfError( repository->Get(KPhoneDoctorUMTS1900SigStrThreshold, 
                            iThresholdValues[ETypeUMTS][EBand1800_1900_2100]) );
    LOGSTRING2("\tKPhoneDoctorUMTS1900SigStrThreshold=%.2f",iThresholdValues[ETypeUMTS][EBand1800_1900_2100]);
    
    CleanupStack::PopAndDestroy(repository);
    }

    
// ---------------------------------------------------------------------------
// From CActive
// CDiagNetCoveragePlugin::CheckNetworkAccessL
// ---------------------------------------------------------------------------    
void CDiagNetCoveragePlugin::CheckNetworkAccessL()
    {        
    if ( iStatus != KErrNone)
        {                                                 
        LOGSTRING2("CDiagNetCoveragePlugin::CheckNetworkAccessL: "
            L"Error obtaining network access type %d", iStatus.Int() );
        if (idialogOn)
        iWaitDialog->ProcessFinishedL();
        CompleteTestL( CDiagResultsDatabaseItem::EFailed );
        }
    else
        {
        LOGSTRING("CDiagNetCoveragePlugin::CheckNetworkAccessL: Got Network Access Type");                
        // Set network band
        switch (iNetworkInfoV1.iAccess)
            {
            // This is used when there is no network activity and therefore no RAT active.
            case CTelephony::ENetworkAccessUnknown:
                LOGSTRING("  ENetworkAccessUnknown"); 
            
#ifdef __WINS__
                // Fake network type for testing purposes
                iNetworkType = ETypeGSM;
#else               
                if (idialogOn)
                iWaitDialog->ProcessFinishedL();
                CompleteTestL( CDiagResultsDatabaseItem::EFailed );
#endif
                break;

            // The access technology is GSM.
            case CTelephony::ENetworkAccessGsm: 
                // FALL THROUGH
                
            // The access technology is GSM COMPACT. However GSM COMPACT systems 
            // which use GSM frequency bands but with the CBPCCH broadcast channel
            // are considered as a separate access technology from GSM.
            case CTelephony::ENetworkAccessGsmCompact:
            
                LOGSTRING("  ENetworkAccessGsm");                 
                iNetworkType = ETypeGSM;
                break;
            
            // The access technology is UTRAN (UMTS Network).
            case CTelephony::ENetworkAccessUtran:
            
                LOGSTRING("  ENetworkAccessUtran");                 
                iNetworkType = ETypeUMTS;
                break;

            default: // Unexpected
                ASSERT(0);
                break;

            }
        
        // If test did not fail, obtain signal strength
        if ( iState == EStateWaitForBandType )
            {                
            iTelephony->GetSignalStrength(iStatus,iSignalStrengthV1Pckg);
            iState = EStateWaitForSignalStrength;
            SetActive();            
            }
        }                                                         
    }
    
// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::CheckSignalStrengthL
// ---------------------------------------------------------------------------     
void CDiagNetCoveragePlugin::CheckSignalStrengthL()
    {
    CDiagResultsDatabaseItem::TResult result = CDiagResultsDatabaseItem::EFailed;
    
    LOGSTRING2("CDiagNetCoveragePlugin::CheckSignalStrengthL Status %d",iStatus.Int());   
             
    if (iStatus.Int() == KErrNone)  
        {
        
        TReal sigStrength = -iSignalStrengthV1.iSignalStrength;           
                                            
        LOGSTRING3("CDiagNetCoveragePlugin::CheckSignalStrengthL: Got Signal Strength=%d bars, %.2f DBm",
                (TInt)iSignalStrengthV1.iBar,
                sigStrength);
        LOGSTRING3("CDiagNetCoveragePlugin::CheckSignalStrengthL: Network type = %d, band = %d",iNetworkType,iNetworkBand);                            
                
        TReal threshold = iThresholdValues[iNetworkType][iNetworkBand];
        LOGSTRING2("CDiagNetCoveragePlugin::CheckSignalStrengthL: Threshold is %.2f",threshold);
                                      
        // Use band/type to index into threshold array and check
        // signal strength.  If there is no signal, Dbm will be 0, which is actually
        // quite high, so check bars to cover this scenario.
        if ( ( sigStrength >= threshold ) && ( iSignalStrengthV1.iBar ) )
            {                        
            result = CDiagResultsDatabaseItem::ESuccess;
            }          
        }
        else
        {            
            LOGSTRING2("CDiagNetCoveragePlugin::CheckSignalStrengthL: Signal strength error %d",iStatus.Int());
        }      

    
    if (idialogOn)                       
    iWaitDialog->ProcessFinishedL();
    
    CompleteTestL( result );
   
    // End of Test
    }      
 
// ---------------------------------------------------------------------------
// CDiagNetCoveragePlugin::Panic
// ---------------------------------------------------------------------------     
void CDiagNetCoveragePlugin::Panic(TInt aPanicCode)
    {
    _LIT(KDiagNetCoverageCategory,"Diag-NetCoverage");
    User::Panic(KDiagNetCoverageCategory, aPanicCode);
    }
    

// ----------------------------------------------------------------------------
// CDiagNetCoveragePlugin::ShowProgressNoteL
//
// wait Note
// ----------------------------------------------------------------------------

void CDiagNetCoveragePlugin::ShowProgressNoteL()
    {
	if ( iWaitDialog )
	{
	delete iWaitDialog;
	iWaitDialog = NULL;
	}
	                
	// WAIT NOTE DIALOG
	iWaitDialog = new (ELeave) CAknWaitDialog(
							(REINTERPRET_CAST(CEikDialog**, &iWaitDialog)), 
							ETrue);
	iWaitDialog->PrepareLC(R_NETWORK_PLUGIN_WAIT_NOTE);
	iWaitDialog->SetCallback(this);
	iWaitDialog->RunLD();
	idialogOn = ETrue;
	LOGSTRING("CDiagNetCoveragePlugin::ShowProgressNoteL()");
    }
    
void CDiagNetCoveragePlugin::DialogDismissedL( TInt aButtonId )
	{
		 LOGSTRING2("CDiagNetCoveragePlugin::DialogDismissedL() = %d",aButtonId);
		 
		 if (aButtonId == ENetcoverageTestCancel && idialogOn) 
		 {
		 // cancel the tests based on which Async Call the dialog is dismissed
		 
		 LOGSTRING2("CDiagNetCoveragePlugin::DialogDismissed @ iState= %d",iState);
		 if (iState == EStateWaitForBandType)
	     iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
		 else if (iState == EStateWaitForSignalStrength)
		 iTelephony->CancelAsync(CTelephony::EGetSignalStrengthCancel);
		 iState = EStateTestCancel;	 
	  	 idialogOn = EFalse; 
	     iWaitDialog = NULL;
	     
		 }
	} 
	
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
        
TBool CDiagNetCoveragePlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
    {
    LOGSTRING( "CDiagNetCoveragePlugin::ShowMessageQueryL IN" )
    CAknMessageQueryDialog* dlg    = NULL;
    TBool                   result = EFalse;

    // Create CAknMessageQueryDialog instance
    dlg = new ( ELeave ) CAknMessageQueryDialog();

    dlg->PrepareLC( aResourceId );
     
    CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();

        switch ( aResourceId )
            {
            case R_DIAG_MESSAGEQUERY_TITLE_NETCOVERAGE: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_CBA_NETCOVERAGE_SINGLE_EXECUTION );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_CBA_NETCOVERAGE_GROUP_EXECUTION );		
                	}
        		}
                break;

            default:
                break;
            }
        	
    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagNetCoveragePlugin::ShowMessageQueryL() OUT aButtonId=%d result=%d", aButtonId, result );
    return result;
    }

    
// End of File

