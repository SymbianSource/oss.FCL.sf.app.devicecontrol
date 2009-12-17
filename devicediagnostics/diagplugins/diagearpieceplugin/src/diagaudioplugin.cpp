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
* Description:  Class Definition of CDiagAudioPlugin
*
*/


// INCLUDE FILES

// From System
#include <AknQueryDialog.h>            // CAknQueryDialog
#include <aknmessagequerydialog.h>     // CAknMessageQueryDialog
#include <AknProgressDialog.h>         // CAknProgressDialog
#include <eikprogi.h>                  // CEikProgressInfo
#include <eikbtgpc.h>                  // CEikButtonGroupContainer
#include <AudioPreference.h>           // KAudioPriorityRecording
#include <mdaaudiosampleeditor.h>      // CMdaAudioRecorderUtility
#include <mdaaudiosampleplayer.h>      // CMdaAudioPlayerUtility
#include <StringLoader.h>              // StringLoader
#include <DiagTestObserver.h>          // MDiagTestObserver
#include <DiagTestExecParam.h>         // TDiagTestExecParam
#include <DiagEngineCommon.h>          // MDiagEngine
#include <DiagPluginPool.h>            // CDiagPluginPool
#include <DiagResultsDbItemBuilder.h>  // CDiagResultsDbItemBuilder
#include <DiagResultDetailBasic.h>     // CDiagResultDetailBasic
#include <DiagFrameworkDebug.h>        // LOGSTRING
#include <DiagCommonDialog.h>          // for EDiagCommonDialogConfirmCancelAll
#include <avkon.rsg>                   // AVKON Resource Definitions

// For Central Repository
#include <centralrepository.h>
#include "diagaudiorecorderprivatecrkeys.h"

// Audio Plugin Headerfile
#include "diagaudioplugin.h"       // CDiagAudioPlugin
#include "diagaudioplugin.hrh"     // Button Id defined
#include <devdiagaudiopluginrsc.rsg>  // Resource Definitions


// Local Macro
#define ASSERT_ALWAYS(c) __ASSERT_ALWAYS((c), User::Invariant())

// Local Data Types
enum TAudioSteps
    {
    EStepTestTitle,
    EStepAskRecord,
    EStepProgressRecord,
    EStepAskPlay,
    EStepProgressPlay,
    EStepAskHeard,
    EStepAskRepeat,
    EAudioPluginTotalSteps
    };

// To Exit from Query it should be EAknSoftkeyOk
// can't include in .hrh because it requires <eikon.rh>
// leads compilation fail
enum TAudioExitCBA
    {
    ECBACmdPlay  = EAknSoftkeyOk,
    ECBACmdStop  = EAknSoftkeyOk,
    ECBACmdStart = EAknSoftkeyOk
    };

// Local Constants
const TInt KFinished      = 0;
const TInt KMinFileSize   = 1024;
const TInt KSamplingRate  = 8192;
const TInt KStereo        = 2;
const TInt KFive          = 5;
const TInt KMicroSecond   = 1000*1000;
const TInt KProgressDelay = 200000;
const TInt KAudioServerRequestTimeoutValue = 16 * KMicroSecond;

const TInt KProgressFinished    =  0;
const TInt KProgressNotFinished =  1;
const TUid KDiagAudioPluginUid  = { DIAG_AUDIO_PLUGIN_UID };

_LIT( KDiagAudioPluginResourceFileName, "z:DevDiagAudioPluginRsc.rsc" );
_LIT( KDiagAudioPluginCategory,"DevDiagAudioPlugin" );

#ifdef __WINS__
const CTelephonyAudioRouting::TAudioOutput KTargetOutput = CTelephonyAudioRouting::ELoudspeaker;
#else
const CTelephonyAudioRouting::TAudioOutput KTargetOutput = CTelephonyAudioRouting::EHandset;
#endif


// ========================= MEMBER FUNCTIONS ================================
// ---------------------------------------------------------------------------
// CDiagAudioPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagAudioPlugin::NewL( TAny* aInitParams )
    {
    CDiagAudioPlugin*             self;
    CDiagPluginConstructionParam* param;

    ASSERT_ALWAYS( aInitParams );

    param = static_cast<CDiagPluginConstructionParam*>( aInitParams );
    CleanupStack::PushL( param );
    self  = new( ELeave ) CDiagAudioPlugin ( param );
    CleanupStack::Pop( param );


    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self

    return self;
    }

// ----------------------------------------------------------------------------
// CDiagAudioPlugin::~CDiagAudioPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagAudioPlugin::~CDiagAudioPlugin()
    {
    // Delete resources allocated during construction.
    // Nothing to do since no new resource is allocated in constructor.
    }

// ----------------------------------------------------------------------------
// CDiagAudioPlugin::CDiagAudioPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagAudioPlugin::CDiagAudioPlugin( CDiagPluginConstructionParam* aParam )
    :    CDiagTestPluginBase( aParam )
    {
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::ConstructL()
    {
    LOGSTRING( "CDiagAudioPlugin::ConstructL: Plugin created. IN" )

    BaseConstructL ( KDiagAudioPluginResourceFileName );

    TInt recording_time;

    // Open Central Repository
    CRepository* audioRecorderRepository;
    audioRecorderRepository = CRepository::NewL( KCRUidDiagVoiceRecorderPlugin );
    CleanupStack::PushL( audioRecorderRepository );

    // Read File Path from CR
    iRecordFilePath.FillZ();
    User::LeaveIfError(
        audioRecorderRepository->Get( KAudioRecorderFilePath, iRecordFilePath) );

    // Read File Size from CR
    User::LeaveIfError(
        audioRecorderRepository->Get( KAudioRecorderRecordingTime , recording_time) );

    iRecordFileSize = (recording_time * KSamplingRate * KStereo);

    CleanupStack::PopAndDestroy( audioRecorderRepository );
    audioRecorderRepository = NULL;

    // Set Audio Output
    iOutput = KTargetOutput;

    LOGSTRING( "CDiagAudioPlugin::ConstructL: Plugin created. OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::StartRecordL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::StartRecordL()
    {
    LOGSTRING( "CDiagAudioPlugin::DoRunTest() IN" )
    TBool       goNext;

    // Delete Record File
    DeleteRecordFile();

    // Check Free Space
    if ( !EnoughFreeSpace() )
        {
        CompleteTestL( CDiagResultsDatabaseItem::ENotPerformed );
        return;
        }

    // Show Init Message Query
    do
        {
        goNext = ETrue;

        // Check session before launch before dialog starts
        if ( !iSessionValid )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL - !iSessionValid 1" )
            return;
            }

        // Report current Progress to Engine
        ReportTestProgressL( EStepTestTitle );

        // Show Message Query Title
        TInt  buttonId;
        TBool result = EFalse;

        result = ShowMessageQueryL( R_MESSAGEQUERY_TITLE, buttonId );

        // Check session is valid after dialog dismissed
        if ( !result )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL - !iSessionValid 2" )
            return;
            }

        // 1.1 Case for press Skip
        
        // ADO & Platformization Changes
        //if ( buttonId == ECBACmdSkip )
        if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL: Skip has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
            }

        // 1.2 case for press Cancel
        //else if ( buttonId == ECBACmdCancel )
        else if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL: Cancel has pressed" )

            TInt   cancelButtonId;

            //cancelResult = AskCancelExecutionL( cancelButtonId );
			CAknDialog* dlg = ExecutionParam().Engine().
							  CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
							  
            if ( !RunWaitingDialogL( dlg, cancelButtonId ) )
                {
                LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 3" )
                return;
                }

			if(cancelButtonId)
			return;
			else
			goNext = EFalse;
			
            
            /*
            switch ( cancelButtonId )
                {
                case EAknSoftkeyYes:
                	CompleteTestL( CDiagResultsDatabaseItem::ECancelled ); // Added for NTEI-7EZ9VQ
                    return;
                default:
                    goNext = EFalse;
                    break;
                }
            */
            }
        }
    while ( goNext == EFalse );


    // Show Record Message Query
    do
        {
        goNext = ETrue;

        // Check session before launch before dialog starts
        if ( !iSessionValid )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL - !iSessionValid 4" )
            return;
            }

        // Report current Progress to Engine
        ReportTestProgressL( EStepAskRecord );

        // Show Message Query Title
        TInt  buttonId;
        TBool result = EFalse;

        result = ShowMessageQueryL( R_MESSAGEQUERY_ASK_RECORD, buttonId );

        // Check session is valid after dialog dismissed
        if ( !result )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL - !iSessionValid 5" )
            return;
            }

        // 2.2 Case for press Skip
        if ( buttonId == ECBACmdStart )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL: Start has pressed" )
            RecordL();
            return;
            }
        else if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL: Skip has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
            }

        // 2.3 case for press Cancel
        else if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagAudioPlugin::StartRecordL: Cancel has pressed" )

            TInt  cancelButtonId;
            TBool cancelResult = EFalse;

            cancelResult = AskCancelExecutionL( cancelButtonId );

            if ( !cancelResult )
                {
                LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 6" )
                return;
                }

            if(cancelButtonId == EAknSoftkeyYes)
                {
                CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                return;
                }
            else if(cancelButtonId)
                {
                return;
                }
            else
                {
                goNext = EFalse;
                }
            
            /*
            switch ( cancelButtonId )
                {
                case EAknSoftkeyYes:
                	CompleteTestL( CDiagResultsDatabaseItem::ECancelled ); // Added for NTEI-7EZ9VQ
                    return;
                default:
                    goNext = EFalse;
                    break;
                }
            */
            }
        }
    while ( goNext == EFalse );

    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::StartPlayL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::StartPlayL()
    {
    LOGSTRING( "CDiagAudioPlugin::StartPlayL() IN" )
    TBool       goNext;


    // Check Record File
    if ( CheckRecordFile() == EFalse )
        {
        LOGSTRING( "CDiagAudioPlugin::StartPlayL() CheckRecordFile == EFalse" )
        CompleteTestL( CDiagResultsDatabaseItem::ENotPerformed );
        return;
        }

    do
        {
        goNext = ETrue;

        // Check session before launch before dialog starts
        if ( !iSessionValid )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 1" )
            return;
            }

        // Report current Progress to Engine
        ReportTestProgressL( EStepAskPlay );

        // Show ConfQuery to ask Play
        TInt  buttonId;
        TBool result = EFalse;

        result = ShowConfQueryL( R_CONFQUERY_ASK_PLAY, buttonId );

        // Check session is valid after dialog dismissed
        if ( !result )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 2" )
            return;
            }

        // 1.1 Case for press Skip
        if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL: Skip has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
            }

        // 1.2 case for press Cancel
        else if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL: Cancel has pressed" )
            TInt  cancelButtonId;
            TBool cancelResult = EFalse;

            cancelResult = AskCancelExecutionL( cancelButtonId );

            if ( !cancelResult )
                {
                LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 3" )
                return;
                }

            if(cancelButtonId == EAknSoftkeyYes)
                {
                CompleteTestL( CDiagResultsDatabaseItem::ECancelled );
                return;
                }
            else if(cancelButtonId)
                {
                return;
                }
            else
                {
                goNext = EFalse;
                }
            /*
            switch ( cancelButtonId )
                {
                case EAknSoftkeyYes:
                	CompleteTestL( CDiagResultsDatabaseItem::ECancelled ); // Added for NTEI-7EZ9VQ
                    return;
                default:
                    goNext = EFalse;
                    break;
                }
               */
            }
        // 1.3 case for press Play
        else if ( buttonId == ECBACmdPlay )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL: Play has pressed" )
            iState = EStateRecorded;
            PlayL();
            }
        } while ( !goNext );
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::RecordingL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::RecordingL()
    {
    LOGSTRING( "CDiagAudioPlugin::RecordingL IN" )
    const TTimeIntervalMicroSeconds     position   = 0;

    iFinalValue = iRecordFileSize / KSamplingRate / KStereo;

    ReportTestProgressL( EStepProgressRecord );
    ShowProgressNoteL( R_PROGRESS_NOTE_RECORDING, iFinalValue );

    iRecorder->SetPriority(
        KAudioPriorityAlarm + 1,
        TMdaPriorityPreference( KAudioPrefVoiceRec )
        );

    iRecorder->SetPosition( position );
    iRecorder->CropL();
    iRecorder->SetMaxWriteLength( iRecordFileSize );

    ResetWatchdog( KAudioServerRequestTimeoutValue, CDiagResultsDatabaseItem::EFailed ); 
    iRecorder->RecordL(); // MoscoStateChangeEvent() will be called as return
    LOGSTRING( "CDiagAudioPlugin::RecordingL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::PlayingL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::PlayingL()
    {
    LOGSTRING( "CDiagAudioPlugin::PlayingL IN" )

    TInt64  interval   = 0;
    TInt    finalValue = 0;

    interval             = iMaxDurationMicroSec.Int64() / KMicroSecond;
    finalValue           = ((I64INT(interval)));
    iFinalValue          = finalValue;

    ReportTestProgressL( EStepProgressPlay );
    ShowProgressNoteL( R_PROGRESS_NOTE_PLAYING, iFinalValue );

    iAudioPlayer->SetPosition( iPositionMicroSec );
    iAudioPlayer->Play();

    LOGSTRING( "CDiagAudioPlugin::PlayingL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::AfterRecordL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::AfterRecordL()
    {
    StartPlayL();
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::AfterPlayL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::AfterPlayL()
    {
    LOGSTRING( "CDiagAudioPlugin::AfterPlayL IN" )


    // Check session before launch before dialog starts
    if ( !iSessionValid )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - !iSessionValid 1.1" )
        return;
        }

    // Report current Progress to Engine
    ReportTestProgressL( EStepAskHeard );

    // Show ConfQuery to ask Heard
    TInt  buttonId;
    TBool result = EFalse;
    result = ShowConfQueryL( R_CONFQUERY_ASK_HEARD, buttonId );

    // Check session is valid after dialog dismissed
    if ( !result )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - !iSessionValid 1.2" )
        return;
        }

    // Report Success on Yes
    if ( buttonId == EAknSoftkeyYes )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - Yes has pressed - return ESuccess" )
        CompleteTestL( CDiagResultsDatabaseItem::ESuccess );
        return;
        }
    else if ( buttonId != ECBACmdCustomNo )
        {
       // return;  // ADO & Platformization Changes
        }

    // Check session before launch before dialog starts
    if ( !iSessionValid )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - !iSessionValid 2.1" )
        return;
        }

    // Report current Progress to Engine
    ReportTestProgressL( EStepAskRepeat );

    // Show ConfQuery to ask Heard
    result = ShowConfQueryL( R_CONFQUERY_ASK_REPEAT, buttonId );

   // Check session is valid after dialog dismissed
    if ( !result )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - !iSessionValid 2.2" )
        return;
        }

    // Report Success on Yes
    if ( buttonId == EAknSoftkeyYes )
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - Yes has pressed - Start Record Again" )
        StartRecordL();
        return;
        }
    else if ( buttonId == ECBACmdCustomNo || buttonId == EAknSoftkeyNo)
        {
        LOGSTRING( "CDiagAudioPlugin::AfterPlayL - No has pressed - Start Record Again" )
        CompleteTestL( CDiagResultsDatabaseItem::EFailed );
        }
    else
        {
        CompleteTestL( CDiagResultsDatabaseItem::EFailed ); // Temp change / As button ID from Ask_Repeat resource is not known
        return;
        }
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::RecordL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::RecordL()
    {
    LOGSTRING( "CDiagAudioPlugin::RecordL IN" )

    iAudioState = CMdaAudioRecorderUtility::ERecording;
    iState = EStateUnknown;

    if ( iRecorder )
        {
        ResetWatchdog( KAudioServerRequestTimeoutValue, CDiagResultsDatabaseItem::EFailed );	
        iRecorder->OpenFileL( iRecordFilePath ); // MoscoStateChangeEvent() will be called as return
        }

    LOGSTRING( "CDiagAudioPlugin::RecordL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::PlayL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::PlayL()
    {
    LOGSTRING( "CDiagAudioPlugin::PlayL IN" )
    iAudioState = CMdaAudioRecorderUtility::EPlaying;

    // Save Current Audio Output
    LOGSTRING( "CDiagAudioPlugin::PlayL:SaveAudioOutput" )
    SaveAudioOutput();

    // Invoke Play
    if ( iAudioPlayer )
        {
        LOGSTRING( "CDiagAudioPlugin::PlayL:OpenFileL(iRecordFilePath);" )
        ResetWatchdog( KAudioServerRequestTimeoutValue, CDiagResultsDatabaseItem::EFailed );
        iAudioPlayer->OpenFileL( iRecordFilePath ); // MapcInitComplete() will be called as return
        }

    LOGSTRING( "CDiagAudioPlugin::PlayL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::SaveAudioOutput()
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::SaveAudioOutput()
    {
    LOGSTRING( "CDiagAudioPlugin::SaveAudioOutput IN" )

    iPrevAudioOutput = iRouting->Output();

    LOGSTRING( "CDiagAudioPlugin::SaveAudioOutput OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::RestoreAudioOutputL()
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::RestoreAudioOutputL()
    {
    LOGSTRING( "CDiagAudioPlugin::RestoreAudioOutputL IN" )

    if ( iRouting )
    {
        iRouting->SetOutputL( iPrevAudioOutput );
    }

    LOGSTRING( "CDiagAudioPlugin::RestoreAudioOutputL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ClosePeriodic
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::ClosePeriodic()
    {
    LOGSTRING( "CDiagAudioPlugin::ClosePeriodic IN" )

    if ( iPeriodic )
        {
        delete iPeriodic;
        iPeriodic = NULL;
        }

    LOGSTRING( "CDiagAudioPlugin::ClosePeriodic OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::CheckRecordFile
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::CheckRecordFile()
    {
    LOGSTRING( "CDiagAudioPlugin::CheckRecordFile IN" )

    TInt        err;
    TEntry      entry;
    TBool       ret = EFalse;

    err  = CoeEnv().FsSession().Entry( iRecordFilePath, entry );

    // Check if there was an error
    if ( err != KErrNone)
        {
        ret = EFalse;
        }

    // Check File Size
    else
        {
        ret = entry.iSize >= KMinFileSize;
        }

    LOGSTRING2( "CDiagAudioPlugin::CheckRecordFile OUT ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiagAuioPlugin::DeleteRecordFile
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::DeleteRecordFile()
    {
    LOGSTRING( "CDiagAudioPlugin::DeleteRecordFile IN" )

    CoeEnv().FsSession().Delete( iRecordFilePath );

    LOGSTRING( "CDiagAudioPlugin::DeleteRecordFile OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::EnoughFreeSpace
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::EnoughFreeSpace()
    {
    LOGSTRING( "CDiagAudioPlugin::EnoughFreeSpace IN" )

    TInt64      RecordFileSize64;
    TVolumeInfo vol_info;
    TBool       ret;

    RecordFileSize64 = iRecordFileSize;
    CoeEnv().FsSession().Volume( vol_info, EDriveC );
    ret = vol_info.iFree >= RecordFileSize64;

    LOGSTRING2( "CDiagAudioPlugin::EnoughFreeSpace OUT ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::SetProgressDismissed
// ---------------------------------------------------------------------------
//
void  CDiagAudioPlugin::SetProgressDismissed( TBool aDismissed )
    {
    LOGSTRING2( "CDiagAudioPlugin::SetProgressDismissed aDismissed=%d", aDismissed )
    iProgressDismissed = aDismissed;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ProgressDismissed
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::ProgressDismissed() const
    {
    LOGSTRING2( "CDiagAudioPlugin::ProgressDismissed iProgressDismissed=%d", iProgressDismissed )

    return iProgressDismissed;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::AskCancelExecutionL
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::AskCancelExecutionL( TInt& aButtonId )
    {
    LOGSTRING( "CDiagAudioPlugin::AskCancelExecutionL() IN" )

    CAknDialog* dialog;
    TBool       result;

	
	 // set softkey for single execution
    if ( !SinglePluginExecution() )
        {
    	// Create common dialog by invoking Engine
    	dialog = ExecutionParam().Engine().
             CreateCommonDialogLC( EDiagCommonDialogConfirmCancelAll, NULL );

    	// Launch dialog and get result from it
    	result = RunWaitingDialogL( dialog, aButtonId );
	}	
    else 
       {
       //CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	 //commented for NTEI-7EZ9VQ
       aButtonId = EAknSoftkeyYes;
       return ETrue;
       }

    LOGSTRING3( "CDiagAudioPlugin::AskCancelExecutionL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ShowConfQueryL
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::ShowConfQueryL( TInt aResourceId, TInt &aButtonId )
    {
    LOGSTRING( "CDiagAudioPlugin::ShowConfQueryL IN" )
    CAknQueryDialog* dlg    = NULL;
    TBool            result = EFalse;

    // Create CAknQueryDialog instance
    dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );

    dlg->PrepareLC( aResourceId );

    // set softkey for single execution
    /*
    if ( SinglePluginExecution() )
        {  */ // ADO & Platformization Changes
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();

        switch ( aResourceId )
            {
            case R_CONFQUERY_ASK_PLAY: 
                cba.SetCommandSetL( R_DIAG_AUDIO_SOFTKEYS_PLAY_CANCEL );
                break;
            default:
                break;
            }
    /*    } */

    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagAudioPlugin::ShowConfQueryL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ShowMessageQueryL
// ---------------------------------------------------------------------------
//
TBool CDiagAudioPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
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
            case R_MESSAGEQUERY_TITLE: 
         	   {
            	   if ( SinglePluginExecution() )
        		{
        		cba.SetCommandSetL( R_DIAG_AUDIO_SOFTKEYS_OK_CANCEL );		
        		}
        	   else 
        	   	{	
                	cba.SetCommandSetL( R_DIAG_AUDIO_SOFTKEYS_OK_SKIP );		
                }
        	}
                break;
            case R_MESSAGEQUERY_ASK_RECORD:
                cba.SetCommandSetL( R_DIAG_AUDIO_SOFTKEYS_START_CANCEL );
                break;
            default:
                break;
            }
        	
    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagAudioPlugin::ShowMessageQueryL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::ShowProgressNoteL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::ShowProgressNoteL( TInt aResourceId, TInt aFinalValue )
    {
    LOGSTRING( "CDiagAudioPlugin::ShowProgressNoteL IN" )

    //CAknProgressDialog* dlg;

    if ( iProgressDialog )
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    SetProgressDismissed( EFalse );

   //G iProgressDialog = dlg = new ( ELeave ) CAknProgressDialog(
   iProgressDialog = new ( ELeave ) CAknProgressDialog(
        reinterpret_cast <CEikDialog**> (&iProgressDialog),
        ETrue
        );

    //G dlg->SetCallback( this );
    iProgressDialog->SetCallback( this );
    //G dlg->PrepareLC( aResourceId );
    iProgressDialog->PrepareLC( aResourceId );

    // set softkey for single execution
/*    if ( SinglePluginExecution() )
        { */ // ADO & Platformization Changes  
        //G CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        CEikButtonGroupContainer& cba = iProgressDialog->ButtonGroupContainer();

        cba.SetCommandSetL( R_DIAG_AUDIO_SOFTKEYS_STOP_CANCEL );
   /*     } */ // ADO & Platformization Changes

    //G iProgressInfo = dlg->GetProgressInfoL();
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo->SetFinalValue( aFinalValue * KFive );

    //G dlg->RunLD();
    iProgressDialog->RunLD();

    // Set up Periodic
    ClosePeriodic();

    iPeriodic = CPeriodic::NewL( EPriorityNormal );
    TCallBack callback( CallbackIncrementProgressNoteL, this );
    iPeriodic->Start( 0, KProgressDelay, callback );

    LOGSTRING( "CDiagAudioPlugin::ShowProgressNoteL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::CallbackIncrementProgressNoteL
// ---------------------------------------------------------------------------
//
TInt CDiagAudioPlugin::CallbackIncrementProgressNoteL( TAny* aThis )
    {
    TInt err = KErrNone;
    CDiagAudioPlugin* myThis  = (CDiagAudioPlugin *)aThis;

    TRAP( err, myThis->UpdateProgressNoteL() );
    if ( err != KErrNone )
        {
        LOGSTRING( "CDiagAudioPlugin::CallbackIncrementProgressNoteL:User::Panic() on UpdateProgressNoteL()" )
        User::Panic( KDiagAudioPluginCategory, err );
        }

    return KFinished;
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::UpdateProgressNoteL
// ---------------------------------------------------------------------------
//
TInt CDiagAudioPlugin::UpdateProgressNoteL()
    {
    TTime              intervalTime;
    TTime              currentTime;

    // Prevent from Screen Saver
    User::ResetInactivityTime();

    if ( iProgressDialog )
        {
        iProgressInfo->IncrementAndDraw(1);

        if ( (iFinalValue * KFive) <= iProgressInfo->CurrentValue() )
            {
            if ( ProgressDismissed() == EFalse )
                {
                iProgressDialog->ProcessFinishedL();
                }

            return KProgressFinished;
            }
        }

    return KProgressNotFinished;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagAudioPlugin::IsVisible()
// ---------------------------------------------------------------------------
TBool CDiagAudioPlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagAudioPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagAudioPlugin::RunMode() const
    {
    return EInteractiveDialog;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagAudioPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagAudioPlugin::TotalSteps() const
    {
    return EAudioPluginTotalSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagAudioPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagAudioPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_AUDIO_PLUGIN_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_AUDIO_PLUGIN_POPUP_INFO_LIST_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_AUDIO_PLUGIN_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_AUDIO_PLUGIN_LIST_SINGLE_GRAPHIC_PANE );

        case ENameLayoutListSingle:           
            return StringLoader::LoadL( R_DIAG_AUDIO_PLUGIN_LIST_SINGLE_GRAPHIC_HEADING_PANE );

        default:
            LOGSTRING2( "CDiagAudioPlugin::GetPluginNameL: "
              L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_AUDIO_PLUGIN_LIST_SINGLE_GRAPHIC_HEADING_PANE );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagAudioPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagAudioPlugin::Uid() const
    {
    return KDiagAudioPluginUid;
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagAudioPlugin::InitializeL()
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::TestSessionBeginL(
    MDiagEngineCommon& /*aEngine*/,
    TBool              /*aSkipDependencyCheck*/,
    TAny*              /*aCustomParams*/)
    {
    LOGSTRING( "CDiagAudioPlugin::TestSessionBeginL:: Deleting record file..." )
    DeleteRecordFile();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagAudioPlugin::CleanupL()
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::TestSessionEndL(
    MDiagEngineCommon& /*aEngine*/,
    TBool              /*aSkipDependencyCheck*/,
    TAny*              /*aCustomParams*/)
    {
    LOGSTRING( "CDiagAudioPlugin::TestSessionEndL:: Deleting record file..." )
    DeleteRecordFile();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagAudioPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagAudioPlugin::DoRunTest() IN" )

    // Framework should never call DoRunTestL() before previous test
    ASSERT_ALWAYS( iRecorder    == NULL );
    ASSERT_ALWAYS( iAudioPlayer == NULL );
    ASSERT_ALWAYS( iRouting     == NULL );

    iSessionValid = ETrue;

    // Create Recorder
    iRecorder = CMdaAudioRecorderUtility::NewL(
        *this,
        NULL,
        KAudioPriorityRecording,
        TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback )
        );

    // Create Audio Player
    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this );

    // Create Audio Routing
    iRouting  = CTelephonyAudioRouting::NewL( *this );

    // Check it has been recorded
    if ( CheckRecordFile() == EFalse )
        {
        LOGSTRING( "CDiagAudioPlugin::DoRunTest() Record file does not exist. StartRecordL()" )
        StartRecordL();
        }
     else
        {
        LOGSTRING( "CDiagAudioPlugin::DoRunTest() Record file exists. StartPlayL()" )
        StartPlayL();
        }

    LOGSTRING( "CDiagAudioPlugin::DoRunTest() OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::DoStopAndCleanupL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::DoStopAndCleanupL()
    {
    LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL IN" )
    // All memory and resource allocated during test execution should be
    // cleaned up here.

    // Set Session
    iSessionValid = EFalse;

    // Close Recorder
    if ( iRecorder )
    {
        LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL: Stop & Close AudioPlayer" )
        iRecorder->Stop();
        iRecorder->Close();
    }

    // Close Audio
    if ( iAudioPlayer )
    {
        LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL: Stop & Close AudioPlayer" )
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
    }

    // Restore AudioOutput
    RestoreAudioOutputL();

    if ( ResultsDbItemBuilder().TestResult() == CDiagResultsDatabaseItem::ESuccess )
        {
        // Test was successful. Do not delete the temporary file.
        LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL: Keep file for next test." )
        }
    else
        {
        // test was not successful. Delete recorded file.
        LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL:DeleteRecordFile()" )
        DeleteRecordFile();
        }

    if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL:RestoreAudioOutputL" )
        TRAP_IGNORE( RestoreAudioOutputL() );
        }

    // Call Engine to Dismiss Any Dialog
    DismissWaitingDialog();

    // Close which can be recovered by running doRunTestL()
    ClosePeriodic();

    // Clean up resources allocated during object creation.
    delete iRecorder;
    iRecorder = NULL;

    delete iAudioPlayer;
    iAudioPlayer = NULL;

    delete iRouting;
    iRouting = NULL;

    // Cleanup Progress Dialog
    LOGSTRING( "Cleanup Progress Dialog" )
    if ( iProgressDialog )
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    SetProgressDismissed( ETrue );

    LOGSTRING( "CDiagAudioPlugin::DoStopAndCleanupL OUT" )
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagAudioPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::RunL()
    {           
    	switch(iState)
    	{
    	case EStateCancelled:
        	CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        	return;
    	case EStateRecorded:
    		AfterRecordL();
    		break;
    	case EStatePlayed:
    		AfterPlayL();
    		break;
    	}
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagAudioPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagAudioPlugin::DoCancel()
    {
    // Stop active request. This class does not use Async request using iStatus.
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::DialogDismissedL( TInt aButtonId )
    {
    LOGSTRING2( "CDiagAudioPlugin::DialogDismissedL: aButtonId = %d", aButtonId )

    TInt        prevState;

    SetProgressDismissed( ETrue );

    // Restore Audio Output in case of Playing
    if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagAudioPlugin::DialogDismissedL: Calling RestoreAudioOutputL" )
        RestoreAudioOutputL();
        }

    // Stop Recording / Playing
    if ( iAudioState == CMdaAudioRecorderUtility::ERecording )
        {
        LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Record" )
        if ( iRecorder )
            {
            iRecorder->Stop();
            iRecorder->Close();
            }
        }
    else if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Play" )
        if ( iAudioPlayer )
            {
            iAudioPlayer->Stop();
            iAudioPlayer->Close();
            }
        }

    // If button has not pressed
    if ( ( aButtonId != ECBACmdSkip )   &&
         ( aButtonId != ECBACmdCancel ) &&
         ( aButtonId != EEikBidCancel ) &&  // for NTEI-7EZ9VQ
         ( aButtonId != EAknSoftkeyOk ) &&
         ( aButtonId != EAknSoftkeyDone) )
        {
        LOGSTRING2( "CDiagAudioPlugin::DialogDismissedL: Button not pressed. aButtonId = %d", aButtonId )
        return;
        }

    // Skip
    if ( aButtonId == ECBACmdSkip )
        {
        LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: aButtonId == ECBACmdSkip" )
        CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
        }

    // Cancel
    else if ( aButtonId == ECBACmdCancel || aButtonId == EEikBidCancel) // for NTEI-7EZ9VQ
        {
        LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: aButtonId == ECBACmdCancel" )

        prevState   = iAudioState;
        iAudioState = CMdaAudioRecorderUtility::EOpen;

        TInt   cancelButtonId;
        TBool  cancelResult = EFalse;

        cancelResult = AskCancelExecutionL( cancelButtonId );
        
        if ( !cancelResult )
            {
            LOGSTRING( "CDiagAudioPlugin::StartPlayL - !iSessionValid 6" )
            return;
            }

        switch ( cancelButtonId )  // for NTEI-7EZ9VQ
             {
            case EAknSoftkeyYes: 
                LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: EAknSoftkeyYes return NOW!!" )
                SetAudioEvent(EStateCancelled);
            	return;

            default:
                if ( prevState == CMdaAudioRecorderUtility::ERecording )
                    {
                    LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Start RecordL" )
                    RecordL();
                    }
                else if ( prevState == CMdaAudioRecorderUtility::EPlaying )
                    {
                    LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Start PlayL" )
                    PlayL();
                    }
                break;
            }
        return;
        }

    // Stop
    else
        {
        LOGSTRING2( "CDiagAudioPlugin::DialogDismissedL: AudioState = %d", iAudioState )
        if ( iAudioState == CMdaAudioRecorderUtility::ERecording )
            {
                LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Record" )
                if ( iRecorder &&  iState == EStateUnknown)
                    {
                    LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Call AfterRecordL()" )
                    SetAudioEvent(EStateRecorded);
                    }
            }
        else if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
            {
                LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Play" )
                if ( iAudioPlayer && iState == EStateRecorded)
                    {
                    LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Call AfterPlayL()" )
                    // Stop playback/recording
                    SetAudioEvent(EStatePlayed);
                    }
            }
        }
    }


void CDiagAudioPlugin::SetAudioEvent(TState aState)
    {
    iState = aState;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::MoscoStateChangeEvent
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::MoscoStateChangeEvent(
    CBase* /*aObject*/,
    TInt   aPreviousState,
    TInt   aCurrentState,
    TInt   aErrorCode
)
    {
    TInt err = KErrNone;

    LOGSTRING4( "CDiagAudioPlugin::MoscoStateChangeEvent:IN (%d -> %d, %d )", aPreviousState, aCurrentState, aErrorCode )

    // 1. Record reaches to the end of file
    if ( aErrorCode == KErrEof )
        {
        iAudioState = CMdaAudioRecorderUtility::EOpen;
        iState = EStateRecorded;
        TRAP( err, AfterRecordL() );
        }

    // 2. Start - Recording
    else if ( ( aPreviousState == CMdaAudioRecorderUtility::ENotReady)  &&
              ( aCurrentState  == CMdaAudioRecorderUtility::EOpen    ) )
        {
        LOGSTRING( "CDiagAudioPlugin::MoscoStateChangeEvent: Leave on RecordingL()" )
        TRAP( err, RecordingL() );
        LOGSTRING2( "CDiagAudioPlugin::MoscoStateChangeEvent: RecordingL() err = %d", err )
        }

    // 3. Stopped - Recording
    else if ( ( aPreviousState == CMdaAudioRecorderUtility::ERecording ) &&
              ( aCurrentState  == CMdaAudioRecorderUtility::EOpen )
            )
        {
        LOGSTRING( "CDiagAudioPlugin::MoscoStateChangeEvent: Leave on AfterRecordL()" )
        iState = EStateRecorded;
        TRAP( err, AfterRecordL() );
        LOGSTRING2( "CDiagAudioPlugin::MoscoStateChangeEvent: AfterRecordL() err = %d", err )
        }

    // Handle for Trap
    if (err != KErrNone)
        {
        LOGSTRING2( "CDiagAudioPlugin::MoscoStateChangeEvent:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagAudioPlugin::MoscoStateChangeEvent:User::Panic()" )
            User::Panic( KDiagAudioPluginCategory, err );
            }
        return; // Fail Test
        }
    LOGSTRING( "CDiagAudioPlugin::MoscoStateChangeEvent OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::MapcInitComplete
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
    {
    LOGSTRING2( "CDiagAudioPlugin::MapcInitComplete: aError=%d", aError )

    // Set Duration
    iPositionMicroSec    = 0;
    iMaxDurationMicroSec = aDuration;

    // Set Priority of AudioPlayer
    iAudioPlayer->SetPriority(
        KAudioPriorityAlarm + 1,
        TMdaPriorityPreference( KAudioPrefVoiceRec )
        );
    
    TInt err = KErrNone;
    // Set Audio Output
    TRAP(err,iRouting->SetOutputL( iOutput ));
    if(err != KErrNone) // Added TRAP for removing code scanner warning
        {
        LOGSTRING2( "CDiagAudioPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
        
        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagAudioPlugin::MapcInitComplete:User::Panic()" )
            User::Panic( KDiagAudioPluginCategory, err );
            }
        return;
        }

    // Start Playing
    TRAP( err, PlayingL() );

    // Handle for Trap
    if ( err != KErrNone )
        {
        LOGSTRING2( "CDiagAudioPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );

        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagAudioPlugin::MapcInitComplete:User::Panic()" )
            User::Panic( KDiagAudioPluginCategory, err );
            }

        return; // Fail Test
        }
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::MapcPlayComplete
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::MapcPlayComplete(TInt aError)
    {
    TInt err = KErrNone;
    LOGSTRING2( "CDiagAudioPlugin::MapcPlayComplete: aError=%d", aError )

    if ( aError == KErrNone)
        {
        LOGSTRING( "CDiagAudioPlugin::MapcPlayComplete KErrNone" )
        TRAP(err,RestoreAudioOutputL());
        
        if ( err != KErrNone )
	        {
	        LOGSTRING2( "CDiagAudioPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
	        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
	
	        if ( err != KErrNone )
	            {
	            LOGSTRING( "CDiagAudioPlugin::MapcInitComplete:User::Panic()" )
	            User::Panic( KDiagAudioPluginCategory, err );
	            }
	
	        return; // Fail Test
	        }
        
        if(iState == EStateRecorded)
            {
        // Stop and Close Audio Player
        if ( iAudioPlayer )
            {
            iAudioPlayer->Stop();
            iAudioPlayer->Close();
                iState = EStatePlayed;
            }

        TRAP(err,AfterPlayL());
        
        if ( err != KErrNone )
	        {
	        LOGSTRING2( "CDiagAudioPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
	        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
	
	        if ( err != KErrNone )
	            {
	            LOGSTRING( "CDiagAudioPlugin::MapcInitComplete:User::Panic()" )
	            User::Panic( KDiagAudioPluginCategory, err );
	            }
	
	        return; // Fail Test
	        }
        
            }
        return;
        }
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::AvailableOutputsChanged
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::AvailableOutputsChanged( CTelephonyAudioRouting& /*aTelephonyAudioRouting */ )
    {
    LOGSTRING( "CDiagAudioPlugin::AvailableOutputsChanged" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::OutputChanged
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::OutputChanged( CTelephonyAudioRouting& /* aTelephonyAudioRouting */)
    {
    LOGSTRING( "CDiagAudioPlugin::OutputChanged" )
    }

// ---------------------------------------------------------------------------
// CDiagAudioPlugin::SetOutputComplete
// ---------------------------------------------------------------------------
//
void CDiagAudioPlugin::SetOutputComplete(
    CTelephonyAudioRouting&    /*aTelephonyAudioRouting */,
    TInt                       aError
    )
    {
    LOGSTRING2( "CDiagAudioPlugin::SetOutputComplete aError=%d", aError )
    }
// End of File
