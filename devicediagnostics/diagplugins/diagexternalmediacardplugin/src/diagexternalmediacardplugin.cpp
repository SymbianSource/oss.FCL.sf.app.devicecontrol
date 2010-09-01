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
* Description:  External Media Card Test Plugin
*
*/


// CLASS DECLARATION
#include "diagexternalmediacardplugin.hrh" // ADO & Platformization Changes
#include "diagexternalmediacardplugin.h"

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
#include <devdiagexternalmediacardpluginrsc.rsg>
#include <AknDialog.h> // ADO & Platformization Changes
#include <aknmessagequerydialog.h>  // ADO & Platformization Changes
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll 
#include <DiagEngineCommon.h> 


#include <AknQueryDialog.h>
#include <avkon.hrh>
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS 
const TInt KDiagExternalMediaCardStepsMaxCount = 2;   //total test steps
_LIT( KText1, "PhoneDoctor External Memory Test" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::CDiagExternalMediaCardPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagExternalMediaCardPlugin::CDiagExternalMediaCardPlugin( 
        CDiagPluginConstructionParam* aParam )
    :   CDiagTestPluginBase( aParam )
    {
    }


// ---------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagExternalMediaCardPlugin::NewL( TAny* aInitParams )
    {
    ASSERT( aInitParams );
    LOGSTRING( "CDiagExternalMediaCard::NewL" );
    
    CDiagPluginConstructionParam* param = 
            static_cast<CDiagPluginConstructionParam*>( aInitParams );    

    CleanupStack::PushL( param );    
    CDiagExternalMediaCardPlugin* self = new( ELeave ) CDiagExternalMediaCardPlugin (param);
    CleanupStack::Pop( param );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self    

    return self;
    }


// ---------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::ConstructL()
    {
    LOGSTRING("CDiagExternalMediaCardPlugin::ConstructL: Plugin created.");
    BaseConstructL ( KDiagExternalMediaCardPluginResourceFileName );
    
    FeatureManager::InitializeLibL();
	idialogOn = EFalse;
	idialogDismissed = EFalse;
    }


// ----------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::~CDiagExternalMediaCardPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagExternalMediaCardPlugin::~CDiagExternalMediaCardPlugin()
    {
    FeatureManager::UnInitializeLib();    
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagExternalMediaCardPlugin::IsVisible()
// ---------------------------------------------------------------------------
TBool CDiagExternalMediaCardPlugin::IsVisible() const
    {
    return ( FeatureManager::FeatureSupported( KFeatureIdMmc ) );
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagExternalMediaCardPlugin::IsSupported()
// ---------------------------------------------------------------------------
TBool CDiagExternalMediaCardPlugin::IsSupported() const
    {
    return ( FeatureManager::FeatureSupported( KFeatureIdMmc ) );
    }    

        
// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagExternalMediaCardPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagExternalMediaCardPlugin::RunMode() const
    {
    return EAutomatic;
    }


// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagExternalMediaCardPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagExternalMediaCardPlugin::TotalSteps() const
    {
    return KDiagExternalMediaCardStepsMaxCount;
    }


// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagExternalMediaCardPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagExternalMediaCardPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListSingle:
            return StringLoader::LoadL ( R_DIAG_EXTERNAL_MEDIA_CARD_PLUGIN_NAME );
                        
        case ENameLayoutHeadingPane:
             return StringLoader::LoadL ( R_QTN_DIAG_MSG_TITLE_MMC );
        
        case ENameLayoutPopupInfoPane:
              //return StringLoader::LoadL ( R_QTN_DIAG_MSG_INFO_MMC );
              //Return test name for dispalyin TEST REsults in case of Perform All test
              //Ravi
              return StringLoader::LoadL ( R_DIAG_EXTERNAL_MEDIA_CARD_PLUGIN_NAME );
        
        case ENameLayoutTitlePane:
             return StringLoader::LoadL ( R_QTN_DIAG_TITLE_TEST_EXTERNAL_MEMORY );
        
        case ENameLayoutListSingleGraphic:
             return StringLoader::LoadL ( R_QTN_DIAG_LST_TEST_MMC );            
            

        default:
            LOGSTRING2( "CDiagExternalMediaCardPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_EXTERNAL_MEDIA_CARD_PLUGIN_NAME );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagExternalMediaCardPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagExternalMediaCardPlugin::Uid() const
    {
    return KDiagExternalMediaCardPluginUid;
    }

    
// ---------------------------------------------------------------------------
// From CActive
// CDiagExternalMediaCardPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::RunL()
    {    	
    switch ( iState )
        {
        case EWriting:
            //report test progress     	
            iCounter--;    
            if ( iCounter > 0 )
                {
                ReportTestProgressL( KDiagExternalMediaCardStepsMaxCount - iCounter );
                }
    		
    		if (idialogDismissed)
	    		{
	    		ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
	    		LOGSTRING("CDiagExternalMediaCardPlugin::RunL dialogDismissed=EWriting");
	    		return;
	    		}
            //check if write is okay
            if ( iStatus == KErrNone )
                {
                LOGSTRING( "CDiagExternalMediaCard::RunL, write ok" );
                iFile.Close();
					
                // read test			
                TInt errorCodeOpen = iFile2.Open( iFs, iFileName, EFileRead );				
                if ( errorCodeOpen!=KErrNone )
                    {
                    LOGSTRING( "CDiagExternalMediaCard::RunL,iFile2 open fail" );				
                    ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
                    return;
                    }			    
                LOGSTRING( "CDiagExternalMediaCard::RunL, iFile2 open okay" );
				
                // Start read test
                SetState( EReading ); 
                iBufr.Zero();
                iFile2.Read( iBufr, iStatus );
         				
                SetActive();                                
                }
            else   //write fail
                {
                LOGSTRING( "CDiagExternalMediaCard::RunL, write fail" );	
                //set test result fail
                ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );       			
                }
            break;
    			
        case EReading:
        
        	if (idialogDismissed)
	    		{
	    		ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
	    		LOGSTRING("CDiagExternalMediaCardPlugin::RunL dialogDismissed=EReading");
	    		return;
	    		}
            //check if read is okay
            if( iStatus == KErrNone )
                {
                LOGSTRING( "CDiagExternalMediaCard::RunL, read okay" );
								
                //compare data and set test result				
                CDiagResultsDatabaseItem::TResult result = ( iBufr == iBufw )? 
                                                            CDiagResultsDatabaseItem::ESuccess
				                                            : CDiagResultsDatabaseItem::EFailed;
                ReportResultToPluginL( result );
                LOGSTRING2( "CDiagExternalMediaCard::RunL,test result %d", result );
                }
            else  //read fail
                {
                LOGSTRING( "CDiagExternalMediaCard::RunL, read fail" );  		
                //set result fail
                ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );
                }
            break;
    		
        default:
            LOGSTRING( "CDiagExternalMediaCard::RunL, default case" );
            //set result fail
            ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );       			
            break;
        }
    }    

// ---------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagExternalMediaCardPlugin::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagExternalMediaCardPlugin::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// From CActive
// CDiagExternalMediaCardPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::DoCancel()
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
    // RFile.Write() does not support cancel. 
    // See Symbian FAQ 1409 at
    // http://www3.symbian.com/faq.nsf/0/C3248753E9B33398802571C3006037CA?OpenDocument
    // So no cancel is necessary for EWriting
    
    }    


// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagExternalMediaCardPlugin::ShowTitleMessageL()
// ---------------------------------------------------------------------------
TBool CDiagExternalMediaCardPlugin::ShowTitleMessageL()
    {
    TInt aButtonId;
    ShowMessageQueryL(R_MESSAGEQUERY_TITLE_EXTERNALMEMORY,aButtonId);
    
    if(aButtonId == ECBACmdCancel)
        {
        ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
        return EFalse;
        }
    
    if(aButtonId == ECBACmdSkip)
        {
        TInt confirmResult = 0;
            
        CAknDialog* dlg = ExecutionParam().Engine().
        CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
    
        if ( !RunWaitingDialogL( dlg, confirmResult ) )
            {
            return EFalse;
            }
        
        if ( confirmResult)
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagExternalMediaCardPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::DoRunTestL()
    {
    //TInt aButtonId; // ADO & Platformization Changes
    iCounter = KDiagExternalMediaCardStepsMaxCount;
    // ADO & Platformization Changes
    if(!ShowTitleMessageL())
        return;
    // Changes Ends
    
    //Start my test
    ShowProgressNoteL();
        
    SetState( EIdle );
    
    //test should not be performed if external media card is not supported
    ASSERT( FeatureManager::FeatureSupported( KFeatureIdMmc ) );
    
    TInt errorCode( KErrNotFound );
    
        //check the drive number for external card
    TInt mmcDriveNum( 0 );
    
        // If path length is zero, driveLetterBuf will be empty.
    TBuf<1> driveLetterBuf;
    
    
    

        
 	driveLetterBuf = PathInfo::MemoryCardRootPath().Left( 1 );

    if ( driveLetterBuf.Length() > 0 )
        {
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, driveLetterBuf ok" );
        TChar driveLetter = driveLetterBuf[0];
        errorCode = RFs::CharToDrive( driveLetter, mmcDriveNum );
        }
    else
        {
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, driveLetterBuf empty" );
        }
            
    if ( errorCode != KErrNone )
        {
        //no such drive exists,act as feature not supported
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, no drive" );               	
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );
        return;       			
       	}
       	
       	
    TVolumeInfo volumeinfo;
    TDriveInfo driveInfo;
    TInt errorCode2;
        	
    do
    {
    User::LeaveIfError( iFs.Connect() );  // Connects a client process to the fileserver
    LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, iFs connect" );
    
    errorCode = iFs.Volume( volumeinfo, mmcDriveNum );
    LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, Volume" );
    
    errorCode2 = iFs.Drive( driveInfo, mmcDriveNum );
    LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, Drive" );
    
    if ( errorCode != KErrNone || errorCode2 != KErrNone )
        {
        //no such drive exists,act as feature not supported
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, no drive" );               		
	        TInt  buttonId;
		    TBool result = EFalse;
		    result = ShowMessageQueryL( R_MESSAGEQUERY_NO_EXTERNALMEMORY, buttonId );

		    // Check session is valid after dialog dismissed
		    if ( !result )
		        {
		        LOGSTRING( "CDiagExternalMediaCardPlugin::AfterPlayL - !iSessionValid 1.2" )
		        return;
		        }

		    // Report Success on Yes
		    if ( (buttonId == EAknSoftkeyYes) ||  (buttonId == EAknSoftkeyOk))
		        {
		        LOGSTRING( "CDiagExternalMediaCardPlugin::EAknSoftkeyOk has pressed - return ESuccess" )
		        }
		    else if ( buttonId == ECBACmdSkip )
		        {
       			ReportResultToPluginL( CDiagResultsDatabaseItem::ESkipped );
		        return;
		        }
	    	else if ( (buttonId == EAknSoftkeyNo) || (buttonId == ECBACmdNo) )
		        {
		        		if(SinglePluginExecution())
		        		{	
		        		ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed);
		        		return;
		        		}
		        		else
		        		{
		            TInt confirmResult = 0;
                
                    CAknDialog* dlg = ExecutionParam().Engine().
                    CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );
                
                    if ( !RunWaitingDialogL( dlg, confirmResult ) )
                        {
                        return;
                        }
                    
                    if ( confirmResult)
                        {
                        //CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	
                        return;
                        }
                 }

		        }		   
        }
    
      } while(errorCode != KErrNone || errorCode2 != KErrNone);
        
        	                  
    //check status of external media card
        
    if ( ( driveInfo.iMediaAtt & KMediaAttLocked ) ||
         ( driveInfo.iMediaAtt & KMediaAttWriteProtected ) )
        {
        //card is locked or read only
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, card locked or read only" );		     		
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed ); 
        return;       		       		    			
        }
            		       		
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, iBufw.MaxLength(), mmcDriveNum ) )
        {
        //card is out of memory
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, out of memory" );
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
        return;	
        }
    	 
    //Try to create a temp file for test	    		
    TFileName FilePath;
    TDriveUnit unit( mmcDriveNum );
    errorCode = iFs.CreatePrivatePath( unit );
		
    if ( ( errorCode != KErrNone ) && ( errorCode != KErrAlreadyExists ) )
        {
        LOGSTRING( "CDiagExternalMediaCard::DoRunTestL, CreatePrivatePath fail" );
        ReportResultToPluginL( CDiagResultsDatabaseItem::EFailed );	
        return;
        }
			
    iFs.PrivatePath( FilePath );
    FilePath.Insert( 0, unit.Name() );
								    		
    errorCode = iFile.Temp( iFs, FilePath, iFileName, EFileWrite );
    LOGSTRING2( "CDiagExternalMediaCard::DoRunTestL, Temp err = %d", errorCode );
											
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
    }

// ----------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::ShowProgressNoteL
//
// Progress Note
// ----------------------------------------------------------------------------

void CDiagExternalMediaCardPlugin::ShowProgressNoteL()
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

		iWaitDialog->PrepareLC(R_EXTMEM_PLUGIN_WAIT_NOTE);
		iWaitDialog->SetCallback(this);
		iWaitDialog->RunLD();
		idialogOn = ETrue;
		LOGSTRING("CDiagExternalMediaCardPlugin::ShowProgressNoteL()");

    }

//*********************************************************
// CallBack from AKnWaitDialog when Cancel is pressed
//*********************************************************

void CDiagExternalMediaCardPlugin::DialogDismissedL( TInt aButtonId )
	{
		 LOGSTRING2("CDiagExternalMediaCardPlugin::DialogDismissedL() = %d",aButtonId);
		 
		 if (aButtonId == ECBACmdCancel && idialogOn) 
		 {
		 // cancel the tests based on which point the dialog is dismissed
		 LOGSTRING2("CDiagExternalMediaCardPlugin::DialogDismissed @ iState= %d",iState);
		 if (iState == EReading)
	     iFile.ReadCancel(iStatus);
		
	     iWaitDialog = NULL;
	     idialogOn = EFalse;
	     idialogDismissed = ETrue;      
		 }
	}    

void CDiagExternalMediaCardPlugin::ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult)
	{
		// clear the dialogue only if dialog is running.    
		if(idialogOn)    
		iWaitDialog->ProcessFinishedL();

		CompleteTestL( aResult );
	}
	
//---------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::DoStopAndCleanupL
//---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::DoStopAndCleanupL()
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

    
//---------------------------------------------------------------------------
// CDiagExternalMediaCardPlugin::SetState()
//---------------------------------------------------------------------------
void CDiagExternalMediaCardPlugin::SetState( TStates aState )
    {
    iState = aState;
    }
    
// ADO & Platformization Changes
TBool CDiagExternalMediaCardPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_MESSAGEQUERY_TITLE_EXTERNALMEMORY: 
         	   {
            	if ( SinglePluginExecution() )
        			{
        			cba.SetCommandSetL( R_EXT_MEMORY_SOFTKEYS_OK_CANCEL );		
        			}
        	   else 
        	   		{	
                	cba.SetCommandSetL( R_EXT_MEMORY_SOFTKEYS_OK_SKIP );		
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

