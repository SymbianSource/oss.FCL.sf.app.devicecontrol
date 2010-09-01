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
* Description:  Internal Memory Test Plugin
*
*/


// CLASS DECLARATION
#include "diaginternalmemoryplugin.h"

// SYSTME INCLUDE FILES
#include <featmgr.h>                    // feature manager for MMC support checking
#include <features.hrh>
//#ifndef RD_MULTIPLE_DRIVE
#include <pathinfo.h>                   //PathInfo
//#endif // RD_MULTIPLE_DRIVE
#include <sysutil.h>                    //SysUtil
#include <StringLoader.h>               // StringLoader

#include <DiagTestObserver.h>           // MDiagTestObserver
#include <DiagTestExecParam.h>          // TDiagTestExecParam
#include <DiagResultsDbItemBuilder.h>   // CDiagResultsDbItemBuilder
#include <DiagResultDetailBasic.h>      // CDiagResultDetailBasic
#include <DiagFrameworkDebug.h>         // LOGSTRING
#include <devdiaginternalmemorypluginrsc.rsg>

#include "diaginternalmemoryplugin.hrh" // ADO & Platformization Changes

#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll 
#include <DiagEngineCommon.h> 
 


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS 
const TInt KDiagInternalMemoryStepsMaxCount = 3;   //total test steps
_LIT( KText1, "PhoneDoctor Internal Memory Test" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::CDiagInternalMemoryPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagInternalMemoryPlugin::CDiagInternalMemoryPlugin( CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam )
    {
    }


// ---------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagInternalMemoryPlugin::NewL( TAny* aInitParams )
    {
    ASSERT( aInitParams );
    LOGSTRING( "CDiagInternalMemory::NewL" );
    

    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>( aInitParams );    
    CleanupStack::PushL( param );    
    CDiagInternalMemoryPlugin* self = new( ELeave ) CDiagInternalMemoryPlugin (param);
    CleanupStack::Pop( param );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self    
    return self;
    }



// ---------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::ConstructL()
    {
    LOGSTRING("CDiagInternalMemoryPlugin::ConstructL: Plugin created.");
    idialogOn = EFalse;
	idialogDismissed = EFalse;
    BaseConstructL ( KDiagInternalMemoryPluginResourceFileName );            
    }


// ----------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::~CDiagInternalMemoryPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagInternalMemoryPlugin::~CDiagInternalMemoryPlugin()
    {        
    
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagInternalMemoryPlugin::IsVisible()
// ---------------------------------------------------------------------------
TBool CDiagInternalMemoryPlugin::IsVisible() const
    {
    return ETrue;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagInternalMemoryPlugin::IsSupported()
// ---------------------------------------------------------------------------
TBool CDiagInternalMemoryPlugin::IsSupported() const
    {
    return ETrue;
    }    

        
// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagInternalMemoryPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagInternalMemoryPlugin::RunMode() const
    {
    return EAutomatic;
    }




// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagInternalMemoryPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagInternalMemoryPlugin::TotalSteps() const
    {
    return KDiagInternalMemoryStepsMaxCount;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagInternalMemoryPlugin::GetPluginName
// ---------------------------------------------------------------------------
HBufC* CDiagInternalMemoryPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    
    switch ( aLayoutType )
        {
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_LST_INTERNAL_MEMORY );
            
        case ENameLayoutHeadingPane:
             return StringLoader::LoadL ( R_QTN_DIAG_MSG_TITLE_INTERNAL_MEMORY );
        
        case ENameLayoutPopupInfoPane:
              return StringLoader::LoadL ( R_QTN_DIAG_MSG_INFO_INTERNAL_MEMORY );
        
        case ENameLayoutTitlePane:
             return StringLoader::LoadL ( R_QTN_DIAG_TITLE_TEST_INTERNAL_MEMORY );
        
        case ENameLayoutListSingleGraphic:
             return StringLoader::LoadL ( R_QTN_DIAG_LST_TEST_INTERNAL_MEMORY );            

        default:
            LOGSTRING2( "CDiagInternalMemoryPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_LST_INTERNAL_MEMORY );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagInternalMemoryPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagInternalMemoryPlugin::Uid() const
    {
    return KDiagInternalMemoryPluginUid;
    }

    
// ---------------------------------------------------------------------------
// From CActive
// CDiagInternalMemoryPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::RunL()
    {
    
    //report test progress     	    
    if ( iCounter > 0 )
        {
        iCounter--;    
        ReportTestProgressL( KDiagInternalMemoryStepsMaxCount - iCounter );
        }     
    	
    // handle read or write action    	
    switch ( iState )
        {
        case EWriting:                            
    		if (idialogDismissed)
	    		{
	    		ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
	    		LOGSTRING("CDiagExternalMediaCardPlugin::RunL dialogDismissed=EWriting");
	    		//reset the var
	    		idialogDismissed = EFalse;
	    		return;
	    		}
            //check if write is okay
            if ( iStatus == KErrNone )
                {
                LOGSTRING( "CDiagInternalMemory::RunL, write ok" );
                iFile.Close();
					
                // read test			
                TInt errorCodeOpen = iFile2.Open( iFs, iFileName, EFileRead );				
                if ( errorCodeOpen!=KErrNone )
                    {
                    LOGSTRING( "CDiagInternalMemory::RunL,iFile2 open fail" );				
                    ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
                    return;
                    }			    
                LOGSTRING( "CDiagInternalMemory::RunL, iFile2 open okay" );
				
                // Start read test
                SetState( EReading ); 
                iBufr.Zero();
                iFile2.Read( iBufr, iStatus );
         				
                SetActive();                
                
                }
            else   //write fail
                {
                LOGSTRING( "CDiagInternalMemory::RunL, write fail" );	
                //set test result fail
                ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );       			
                }
            break;
    			
        case EReading:        
    		if (idialogDismissed)
	    		{
	    		ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
	    		LOGSTRING("CDiagExternalMediaCardPlugin::RunL dialogDismissed=EReading");
	    		//reset the var
	    		idialogDismissed = EFalse;	    		
	    		return;
	    		}
            //check if read is okay
            if( iStatus == KErrNone )
                {
                LOGSTRING( "CDiagInternalMemory::RunL, read okay" );
								
                //compare data and set test result				
                CDiagResultsDatabaseItem::TResult result = ( iBufr == iBufw )? 
                                                            CDiagResultsDatabaseItem::ESuccess
				                                            : CDiagResultsDatabaseItem::EFailed;
                ReportResultToPluginL( result );
                LOGSTRING2( "CDiagInternalMemory::RunL,test result %d", result );
                }
            else  //read fail
                {
                LOGSTRING( "CDiagInternalMemory::RunL, read fail" );  		
                //set result fail
                ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );
                }
            break;
        default:
            LOGSTRING( "CDiagInternalMemory::RunL, default case" );
            //set result fail
            ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );       			
            break;
        }           
    }    

// ---------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagInternalMemoryPlugin::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagInternalMemoryPlugin::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// From CActive
// CDiagInternalMemoryPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::DoCancel()
    {     
    switch ( iState )
        {
        case EReading:
            iFile.ReadCancel(iStatus);
            break;
            
        default:
            // No action needed
            break;
        }    
    }    


// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagInternalMemoryPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::DoRunTestL()
    {
    TInt aButtonId; // ADO & Platformization Changes
    iCounter = KDiagInternalMemoryStepsMaxCount;
    // ADO & Platformization Changes
    
    ShowMessageQueryL(R_MESSAGEQUERY_TITLE_INTERNALMEMORY,aButtonId);
    
    if(aButtonId == ECBACmdCancel)
    	{
    	ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
        return;
    	}
    
    if(aButtonId == ECBACmdSkip)
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
    
    
    //Start my test
    
    ShowProgressNoteL();
    
    SetState( EIdle );
        	            	  
    //check the drive number for internal memory
    TInt phoneMemoryDriveNum( 0 );
    // If path length is zero, driveLetterBuf will be empty.
    TBuf<1> driveLetterBuf = PathInfo::PhoneMemoryRootPath ().Left( 1 );

    TInt errorCode( KErrNotFound );
    if ( driveLetterBuf.Length() > 0 )
        {
        LOGSTRING( "CDiagInternalMemory::DoRunTestL, driveLetterBuf ok" );
        TChar driveLetter = driveLetterBuf[0];
        errorCode = RFs::CharToDrive( driveLetter, phoneMemoryDriveNum );
        }
    else
        {
        LOGSTRING( "CDiagInternalMemory::DoRunTestL, driveLetterBuf empty" );
        }
            
    if ( errorCode != KErrNone )
        {        
        LOGSTRING( "CDiagInternalMemory::DoRunTestL, no drive" );               	
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );
        return;       			
       	}
        	                  
 
    User::LeaveIfError( iFs.Connect() );  // Connects a client process to the fileserver
    LOGSTRING( "CDiagInternalMemory::DoRunTestL, iFs connect" );
   // check disk space            		       		
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, iBufw.MaxLength(), phoneMemoryDriveNum ) )
        {
        //disk is out of memory
        LOGSTRING( "CDiagInternalMemory::DoRunTestL, out of memory" );
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
        return;	
        }
    	 
    //Try to create a temp file for test	    		
    TFileName filePath;
    TDriveUnit unit( phoneMemoryDriveNum );
    errorCode = iFs.CreatePrivatePath( unit );
		
    if ( ( errorCode != KErrNone ) && ( errorCode != KErrAlreadyExists ) )
        {
        LOGSTRING( "CDiagInternalMemory::DoRunTestL, CreatePrivatePath fail" );
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
        return;
        }
			
    iFs.PrivatePath( filePath );
    filePath.Insert( 0, unit.Name() );
								    		
    errorCode = iFile.Temp( iFs, filePath, iFileName, EFileWrite );
    LOGSTRING2( "CDiagInternalMemory::DoRunTestL, Temp err = %d", errorCode );
											
    if ( errorCode != KErrNone )
        {
        //Temp file creation failed
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );
        return;
        }
    		
    // Temp file created, start the write test					
							
    iBufw.Zero();
    iBufw.Copy( KText1 );
			
    SetState( EWriting ); 
		
    iFile.Write( iBufw, iStatus );
    SetActive();        
    
    //report test progress     	    
    if ( iCounter > 0 )
        {
        iCounter--;    
        ReportTestProgressL( KDiagInternalMemoryStepsMaxCount - iCounter );        
        }  
    }


//---------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::DoStopAndCleanupL
//---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::DoStopAndCleanupL()
    {
    switch ( iState )
        {
        case EWriting: 
            iFile.Close();
            iFs.Delete( iFileName ); // delete the file created by Temp									
            iFs.Close();
            break;
    		
        case EReading:
            iFile2.Close();	
            iFs.Delete( iFileName ); // delete the file created by Temp						
            iFs.Close();   		
            break;
    		 
        case EIdle:
            iFs.Close();
            break;
    	    
        default:
            ASSERT ( EFalse );      //this should never happen
            break;
        }
        
	if ( iWaitDialog )
	delete iWaitDialog;   
    idialogOn = EFalse;
	idialogDismissed = EFalse;  
       
    SetState( EIdle );
    }


// ----------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::ShowProgressNoteL
//
// Progress Note
// ----------------------------------------------------------------------------

void CDiagInternalMemoryPlugin::ShowProgressNoteL()
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

		iWaitDialog->PrepareLC(R_INTMEM_PLUGIN_WAIT_NOTE);
		iWaitDialog->SetCallback(this);
		iWaitDialog->RunLD();
		idialogOn = ETrue;
		LOGSTRING("CDiagInternalMemoryPlugin::ShowProgressNoteL()");

    }

//*********************************************************
// CallBack from AKnWaitDialog when Cancel is pressed
//*********************************************************

void CDiagInternalMemoryPlugin::DialogDismissedL( TInt aButtonId )
	{
		 LOGSTRING2("CDiagInternalMemoryPlugin::DialogDismissedL() = %d",aButtonId);
		 
		 if (aButtonId == ECBACmdCancel && idialogOn) 
		 {
		 // cancel the tests based on which point the dialog is dismissed
		 LOGSTRING2("CDiagInternalMemoryPlugin::DialogDismissed @ iState= %d",iState);
		 if (iState == EReading)
	     iFile.ReadCancel(iStatus);
		
	     iWaitDialog = NULL;
	     idialogOn = EFalse;
	     idialogDismissed = ETrue;      
		 }
	}    

void CDiagInternalMemoryPlugin::ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult)
	{
		// clear the dialogue only if dialog is running.    
		if(idialogOn)    
		iWaitDialog->ProcessFinishedL();
		LOGSTRING("CDiagInternalMemoryPlugin::ReportResultToPluginL()");
		CompleteTestL( aResult );
	}
	    
//---------------------------------------------------------------------------
// CDiagInternalMemoryPlugin::SetState()
//---------------------------------------------------------------------------
void CDiagInternalMemoryPlugin::SetState( TStates aState )
    {
    iState = aState;
    }
    
    
// ADO & Platformization Changes
TBool CDiagInternalMemoryPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_MESSAGEQUERY_TITLE_INTERNALMEMORY: 
         	   {
            	if ( SinglePluginExecution() )
        		{
        			cba.SetCommandSetL( R_INT_MEMORY_SOFTKEYS_OK_CANCEL );		
        		}
        	   else 
        	   	{	
                	cba.SetCommandSetL( R_INT_MEMORY_SOFTKEYS_OK_SKIP );		
                }
        	}
                break;

            default:
                break;
            }
        	
    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagAudioPlugin::ShowMessageQueryL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }
    
// End of File
