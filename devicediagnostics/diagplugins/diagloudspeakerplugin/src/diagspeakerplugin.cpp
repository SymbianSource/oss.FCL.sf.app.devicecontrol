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
* Description:  Class Definition of CDiagSpeakerPlugin
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

// For Central Repository
#include <centralrepository.h>
#include "diagspeakertestprivatecrkeys.h"

// Speaker Plugin Headerfile
#include "diagspeakerplugin.h"       // CDiagSpeakerPlugin
#include "diagspeakerplugin.hrh"     // Button Id defined
#include <devdiagspeakerpluginrsc.rsg>  // Resource Definitions
#include <avkon.hrh>


// Local Macro
#define ASSERT_ALWAYS(c) __ASSERT_ALWAYS((c), User::Invariant())

// Local Data Types
enum TSpeakerSteps
    {
    EStepTestTitle,
    EStepAskRecord,
    EStepProgressRecord,
    EStepAskPlay,
    EStepProgressPlay,
    EStepAskHeard,
    EStepAskRepeat,
    ESpeakerPluginTotalSteps
    };

// To Exit from Query it should be EAknSoftkeyOk
// can't include in .hrh because it requires <eikon.rh>
// leads compilation fail
enum TSpeakerExitCBA
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
const TUid KDiagSpeakerPluginUid  = { DIAG_SPEAKER_PLUGIN_UID };

_LIT( KDiagSpeakerPluginResourceFileName, "z:DevDiagSpeakerPluginRsc.rsc" );
_LIT( KDiagSpeakerPluginCategory,"DevDiagSpeakerPlugin" );

const CAudioOutput::TAudioOutputPreference KTargetOutput = CAudioOutput::EPublic;


// ========================= MEMBER FUNCTIONS ================================
// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
MDiagPlugin* CDiagSpeakerPlugin::NewL( TAny* aInitParams )
    {
    CDiagSpeakerPlugin* self;
    CDiagPluginConstructionParam* param;

    ASSERT_ALWAYS( aInitParams );

    param = static_cast<CDiagPluginConstructionParam*>( aInitParams );
    CleanupStack::PushL( param );
    self  = new( ELeave ) CDiagSpeakerPlugin ( param );
    CleanupStack::Pop( param );


    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self

    return self;
    }

// ----------------------------------------------------------------------------
// CDiagSpeakerPlugin::~CDiagSpeakerPlugin
//
// Destructor
// ----------------------------------------------------------------------------
CDiagSpeakerPlugin::~CDiagSpeakerPlugin()
    {
    // Delete resources allocated during construction.
    // Nothing to do since no new resource is allocated in constructor.
    }

// ----------------------------------------------------------------------------
// CDiagSpeakerPlugin::CDiagSpeakerPlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CDiagSpeakerPlugin::CDiagSpeakerPlugin( CDiagPluginConstructionParam* aParam )
    :    CDiagTestPluginBase( aParam )
    {
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::ConstructL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::ConstructL: Plugin created. IN" )

    BaseConstructL ( KDiagSpeakerPluginResourceFileName );

    TInt recording_time;

    // Open Central Repository
    CRepository* audioRecorderRepository = CRepository::NewL( KCRUidDiagSpeakerTestPlugin );
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

    LOGSTRING( "CDiagSpeakerPlugin::ConstructL: Plugin created. OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::StartRecordL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::StartRecordL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::DoRunTest() IN" )
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
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL - !iSessionValid 1" )
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
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL - !iSessionValid 2" )
            return;
            }

        // 1.1 Case for press Skip
        if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL: Skip has pressed" );
            // ADO & Platformization Changes
            TInt   cancelButtonId;

            //cancelResult = AskCancelExecutionL( cancelButtonId );
			CAknDialog* dlg = ExecutionParam().Engine().
							  CreateCommonDialogLC( EDiagCommonDialogConfirmSkipAll, NULL );
							  
            if ( !RunWaitingDialogL( dlg, cancelButtonId ) )
                {
                LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 3" )
                return;
                }

			if(cancelButtonId)
			return;
			else
			goNext = EFalse;          
            

			// ADO & Platformization Changes
            //CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            //return;
            }

        // 1.2 case for press Cancel
        else if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL: Cancel has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
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
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL - !iSessionValid 4" )
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
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL - !iSessionValid 5" )
            return;
            }

        // 2.2 Case for press Skip
        if ( buttonId == ECBACmdStart )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL: Start has pressed" )
            RecordL();
            return;
            }
        else if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL: Skip has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
            }

        // 2.3 case for press Cancel
        else if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartRecordL: Cancel has pressed" )

            TInt  cancelButtonId;
            TBool cancelResult = EFalse;

            cancelResult = AskCancelExecutionL( cancelButtonId );

            if ( !cancelResult )
                {
                LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 6" )
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
                	CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	 //Added for NTEI-7EZ96S
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
// CDiagSpeakerPlugin::StartPlayL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::StartPlayL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::StartPlayL() IN" )
    TBool goNext;


    // Check Record File
    if ( CheckRecordFile() == EFalse )
        {
        LOGSTRING( "CDiagSpeakerPlugin::StartPlayL() CheckRecordFile == EFalse" )
        CompleteTestL( CDiagResultsDatabaseItem::ENotPerformed );
        return;
        }

    do
        {
        goNext = ETrue;

        // Check session before launch before dialog starts
        if ( !iSessionValid )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 1" )
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
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 2" )
            return;
            }

        // 1.1 Case for press Skip
        if ( buttonId == ECBACmdSkip )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL: Skip has pressed" )
            CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
            return;
            }

        // 1.2 case for press Cancel
        else if ( buttonId == ECBACmdCancel )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL: Cancel has pressed" )
            TInt  cancelButtonId;
            TBool cancelResult = EFalse;

            cancelResult = AskCancelExecutionL( cancelButtonId );

            if ( !cancelResult )
                {
                LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 3" )
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
                    iState = EStateCancelled;
                    SetActive();
            		TRequestStatus* status = &iStatus;
            		User::RequestComplete(status, KErrNone);
                	//CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	 //Commented by Ganesh
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
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL: Play has pressed" )
            iState = EStateRecorded;
            PlayL();
            }
        } while ( !goNext );
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::RecordingL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::RecordingL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::RecordingL IN" )
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
    LOGSTRING( "CDiagSpeakerPlugin::RecordingL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::PlayingL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::PlayingL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::PlayingL IN" )

    TInt64  interval   = 0;
    TInt    finalValue = 0;

    interval             = iMaxDurationMicroSec.Int64() / KMicroSecond;
    finalValue           = ((I64INT(interval)));
    iFinalValue          = finalValue;

    // Save Audio Output
    SaveAudioOutput();

    ReportTestProgressL( EStepProgressPlay );
    ShowProgressNoteL( R_PROGRESS_NOTE_PLAYING, iFinalValue );

    iAudioPlayer->SetPosition( iPositionMicroSec );
    iAudioPlayer->Play();

    LOGSTRING( "CDiagSpeakerPlugin::PlayingL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::AfterRecordL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::AfterRecordL()
    {
    StartPlayL();
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::AfterPlayL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::AfterPlayL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL IN" )


    // Check session before launch before dialog starts
    if ( !iSessionValid )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - !iSessionValid 1.1" )
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
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - !iSessionValid 1.2" )
        return;
        }

    // Report Success on Yes
    if ( buttonId == EAknSoftkeyYes )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - Yes has pressed - return ESuccess" )
        CompleteTestL( CDiagResultsDatabaseItem::ESuccess );
        return;
        }
    else if ( buttonId != ECBACmdCustomNo )
        {
        //return;  // ADO & Platformization Changes
        }

    // Check session before launch before dialog starts
    if ( !iSessionValid )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - !iSessionValid 2.1" )
        return;
        }

    // Report current Progress to Engine
    ReportTestProgressL( EStepAskRepeat );

    // Show ConfQuery to ask Heard
    result = ShowConfQueryL( R_CONFQUERY_ASK_REPEAT, buttonId );

   // Check session is valid after dialog dismissed
    if ( !result )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - !iSessionValid 2.2" )
        return;
        }

    // Report Success on Yes
    if ( buttonId == EAknSoftkeyYes )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - Yes has pressed - Start Record Again" )

        StartRecordL();
        return;
        }
    else if ( buttonId == ECBACmdCustomNo || buttonId == EAknSoftkeyNo )
        {
        LOGSTRING( "CDiagSpeakerPlugin::AfterPlayL - No has pressed - Start Record Again" )
        CompleteTestL( CDiagResultsDatabaseItem::EFailed );
        }
    else
        {
        CompleteTestL( CDiagResultsDatabaseItem::EFailed );
        return;
        }
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::RecordL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::RecordL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::RecordL IN" )

    iAudioState = CMdaAudioRecorderUtility::ERecording;
    
    iState = EStateUnknown;

    if ( iRecorder )
        {
        ResetWatchdog( KAudioServerRequestTimeoutValue, CDiagResultsDatabaseItem::EFailed );  
        iRecorder->OpenFileL( iRecordFilePath ); // MoscoStateChangeEvent() will be called as return
        }

    LOGSTRING( "CDiagSpeakerPlugin::RecordL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::PlayL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::PlayL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::PlayL IN" )
    iAudioState = CMdaAudioRecorderUtility::EPlaying;

    // Save Current Audio Output
    LOGSTRING( "CDiagSpeakerPlugin::PlayL:SaveAudioOutput" )

    // Invoke Play
    if ( iAudioPlayer )
        {
        LOGSTRING( "CDiagSpeakerPlugin::PlayL:OpenFileL(iRecordFilePath);" )
        ResetWatchdog( KAudioServerRequestTimeoutValue, CDiagResultsDatabaseItem::EFailed ); 
        iAudioPlayer->OpenFileL( iRecordFilePath ); // MapcInitComplete() will be called as return
        }

    LOGSTRING( "CDiagSpeakerPlugin::PlayL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::SaveAudioOutput()
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::SaveAudioOutput()
    {
    LOGSTRING( "CDiagSpeakerPlugin::SaveAudioOutput IN" )

    if ( iAudioOutput )
        {
        iPrevAudioOutput = iAudioOutput->AudioOutput();
        }

    LOGSTRING( "CDiagSpeakerPlugin::SaveAudioOutput OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::RestoreAudioOutputL()
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::RestoreAudioOutputL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::RestoreAudioOutputL IN" )

    if ( iAudioOutput )
        {
        iAudioOutput->SetAudioOutputL( iPrevAudioOutput );
        }

    LOGSTRING( "CDiagSpeakerPlugin::RestoreAudioOutputL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ClosePeriodic
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::ClosePeriodic()
    {
    LOGSTRING( "CDiagSpeakerPlugin::ClosePeriodic IN" )

    if ( iPeriodic )
        {
        delete iPeriodic;
        iPeriodic = NULL;
        }

    LOGSTRING( "CDiagSpeakerPlugin::ClosePeriodic OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::CheckRecordFile
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::CheckRecordFile()
    {
    LOGSTRING( "CDiagSpeakerPlugin::CheckRecordFile IN" )

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

    LOGSTRING2( "CDiagSpeakerPlugin::CheckRecordFile OUT ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiagAuioPlugin::DeleteRecordFile
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::DeleteRecordFile()
    {
    LOGSTRING( "CDiagSpeakerPlugin::DeleteRecordFile IN" )

    CoeEnv().FsSession().Delete( iRecordFilePath );

    LOGSTRING( "CDiagSpeakerPlugin::DeleteRecordFile OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::EnoughFreeSpace
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::EnoughFreeSpace()
    {
    LOGSTRING( "CDiagSpeakerPlugin::EnoughFreeSpace IN" )

    TInt64      RecordFileSize64;
    TVolumeInfo vol_info;
    TBool       ret;

    RecordFileSize64 = iRecordFileSize;
    CoeEnv().FsSession().Volume( vol_info, EDriveC );
    ret = vol_info.iFree >= RecordFileSize64;

    LOGSTRING2( "CDiagSpeakerPlugin::EnoughFreeSpace OUT ret=%d", ret )
    return ret;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::SetProgressDismissed
// ---------------------------------------------------------------------------
//
void  CDiagSpeakerPlugin::SetProgressDismissed( TBool aDismissed )
    {
    LOGSTRING2( "CDiagSpeakerPlugin::SetProgressDismissed aDismissed=%d", aDismissed )
    iProgressDismissed = aDismissed;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ProgressDismissed
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::ProgressDismissed() const
    {
    LOGSTRING2( "CDiagSpeakerPlugin::ProgressDismissed iProgressDismissed=%d", iProgressDismissed )

    return iProgressDismissed;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::AskCancelExecutionL
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::AskCancelExecutionL( TInt& aButtonId )
    {
    LOGSTRING( "CDiagSpeakerPlugin::AskCancelExecutionL() IN" )

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
       //CompleteTestL( CDiagResultsDatabaseItem::ECancelled );	 //commented for NTEI-7EZ96S
       aButtonId = EAknSoftkeyYes;
       return ETrue;
       }
    
    LOGSTRING3( "CDiagSpeakerPlugin::AskCancelExecutionL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ShowConfQueryL
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::ShowConfQueryL( TInt aResourceId, TInt &aButtonId )
    {
    LOGSTRING( "CDiagSpeakerPlugin::ShowConfQueryL IN" )
    CAknQueryDialog* dlg    = NULL;
    TBool            result = EFalse;

    // Create CAknQueryDialog instance
    dlg = new ( ELeave ) CAknQueryDialog( CAknQueryDialog::ENoTone );

    dlg->PrepareLC( aResourceId );

    // set softkey for single execution
    /*
    if ( SinglePluginExecution() )
        { */  // ADO & Platformization Changes
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();

        switch ( aResourceId )
            {
            case R_CONFQUERY_ASK_PLAY: 
                cba.SetCommandSetL( R_DIAG_SPEAKER_SOFTKEYS_PLAY_CANCEL );
                break;
            default:
                break;
            }
      /*  } */

    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagSpeakerPlugin::ShowConfQueryL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ShowMessageQueryL
// ---------------------------------------------------------------------------
//
TBool CDiagSpeakerPlugin::ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  )
    {
    LOGSTRING( "CDiagSpeakerPlugin::ShowMessageQueryL IN" )
    CAknMessageQueryDialog* dlg    = NULL;
    TBool                   result = EFalse;

    // Create CAknMessageQueryDialog instance
    dlg = new ( ELeave ) CAknMessageQueryDialog();

    dlg->PrepareLC( aResourceId );

    // set softkey for single execution
    
    
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();

        switch ( aResourceId )
            {
            case R_MESSAGEQUERY_TITLE: 
        	if ( SinglePluginExecution() )
        		{
                cba.SetCommandSetL( R_DIAG_SPEAKER_SOFTKEYS_OK_CANCEL );
        		}
        	else
        		{
        		cba.SetCommandSetL( R_DIAG_SPEAKER_SOFTKEYS_OK_SKIP );	
        		}
                break;
            case R_MESSAGEQUERY_ASK_RECORD:
                cba.SetCommandSetL( R_DIAG_SPEAKER_SOFTKEYS_START_CANCEL );
                break;
            default:
                break;
            }
            
    	

    result = RunWaitingDialogL( dlg, aButtonId );

    LOGSTRING3( "CDiagSpeakerPlugin::ShowMessageQueryL() OUT aButtonId=%d result=%d", aButtonId, result )
    return result;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::ShowProgressNoteL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::ShowProgressNoteL( TInt aResourceId, TInt aFinalValue )
    {
    LOGSTRING( "CDiagSpeakerPlugin::ShowProgressNoteL IN" )

    CAknProgressDialog* dlg;

    if ( iProgressDialog )
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    SetProgressDismissed( EFalse );

    iProgressDialog = dlg = new ( ELeave ) CAknProgressDialog(
        reinterpret_cast <CEikDialog**> (&iProgressDialog),
        ETrue
        );

    dlg->SetCallback( this );
    dlg->PrepareLC( aResourceId );

    // set softkey for single execution
  /*  if ( SinglePluginExecution() ) 
        { */ // ADO & Platformization Changes
        CEikButtonGroupContainer& cba = dlg->ButtonGroupContainer();
        cba.SetCommandSetL( R_DIAG_SPEAKER_SOFTKEYS_STOP_CANCEL );
      /*  } */ // ADO & Platformization Changes

    iProgressInfo = dlg->GetProgressInfoL();
    iProgressInfo->SetFinalValue( aFinalValue * KFive );

    dlg->RunLD();

    // Set up Periodic
    ClosePeriodic();

    iPeriodic = CPeriodic::NewL( EPriorityNormal );
    TCallBack callback( CallbackIncrementProgressNoteL, this );
    iPeriodic->Start( 0, KProgressDelay, callback );

    LOGSTRING( "CDiagSpeakerPlugin::ShowProgressNoteL OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::CallbackIncrementProgressNoteL
// ---------------------------------------------------------------------------
//
TInt CDiagSpeakerPlugin::CallbackIncrementProgressNoteL( TAny* aThis )
    {
    TInt err = KErrNone;
    CDiagSpeakerPlugin* myThis  = (CDiagSpeakerPlugin *)aThis;

    TRAP( err, myThis->UpdateProgressNoteL() );
    if ( err != KErrNone )
        {
        LOGSTRING( "CDiagSpeakerPlugin::CallbackIncrementProgressNoteL:User::Panic() on UpdateProgressNoteL()" )
        User::Panic( KDiagSpeakerPluginCategory, err );
        }

    return KFinished;
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::UpdateProgressNoteL
// ---------------------------------------------------------------------------
//
TInt CDiagSpeakerPlugin::UpdateProgressNoteL()
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
// CDiagSpeakerPlugin::IsVisible()
// ---------------------------------------------------------------------------
TBool CDiagSpeakerPlugin::IsVisible() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagSpeakerPlugin::RunMode()
// ---------------------------------------------------------------------------
MDiagTestPlugin::TRunMode CDiagSpeakerPlugin::RunMode() const
    {
    return EInteractiveDialog;
    }

// ---------------------------------------------------------------------------
// From MDiagTestPlugin
// CDiagSpeakerPlugin::TotalSteps()
// ---------------------------------------------------------------------------
TUint CDiagSpeakerPlugin::TotalSteps() const
    {
    return ESpeakerPluginTotalSteps;
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSpeakerPlugin::GetPluginNameL
// ---------------------------------------------------------------------------
HBufC* CDiagSpeakerPlugin::GetPluginNameL( TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_SPEAKER_PLUGIN_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_SPEAKER_PLUGIN_POPUP_INFO_LIST_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_SPEAKER_PLUGIN_TITLE_PANE );

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_SPEAKER_PLUGIN_LIST_SINGLE_GRAPHIC_PANE );

        case ENameLayoutListSingle:           
            return StringLoader::LoadL( R_DIAG_SPEAKER_PLUGIN_LIST_SINGLE_GRAPHIC_HEADING_PANE );

        default:
            LOGSTRING2( "CDiagSpeakerPlugin::GetPluginNameL: "
                L"ERROR: Unsupported layout type %d", aLayoutType )
            __ASSERT_DEBUG( 0, User::Invariant() );
            return StringLoader::LoadL ( R_DIAG_SPEAKER_PLUGIN_LIST_SINGLE_GRAPHIC_HEADING_PANE );
        }
    }

// ---------------------------------------------------------------------------
// From MDiagPlugin
// CDiagSpeakerPlugin::Uid
// ---------------------------------------------------------------------------
TUid CDiagSpeakerPlugin::Uid() const
    {
    return KDiagSpeakerPluginUid;
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagSpeakerPlugin::InitializeL()
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::TestSessionBeginL(
    MDiagEngineCommon& /*aEngine*/,
    TBool              /*aSkipDependencyCheck*/,
    TAny*              /*aCustomParams*/)
    {
    LOGSTRING( "CDiagSpeakerPlugin::TestSessionBeginL:: Deleting record file..." )
    DeleteRecordFile();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagSpeakerPlugin::CleanupL()
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::TestSessionEndL(
    MDiagEngineCommon& /*aEngine*/,
    TBool              /*aSkipDependencyCheck*/,
    TAny*              /*aCustomParams*/)
    {
    LOGSTRING( "CDiagSpeakerPlugin::TestSessionEndL:: Deleting record file..." )
    DeleteRecordFile();
    }

// ---------------------------------------------------------------------------
// From CDiagTestPluginBase
// CDiagSpeakerPlugin::DoRunTestL()
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::DoRunTestL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::DoRunTest() IN" )

    // Framework should never call DoRunTestL() before previous test
    ASSERT_ALWAYS( iRecorder == NULL );
    ASSERT_ALWAYS( iAudioPlayer == NULL );
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

    // Check it has been recorded
    if ( CheckRecordFile() == EFalse )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DoRunTest() Record file does not exist. StartRecordL()" )
        StartRecordL();
        }
     else
        {
        LOGSTRING( "CDiagSpeakerPlugin::DoRunTest() Record file exists. StartPlayL()" )
        StartPlayL();
        }

    LOGSTRING( "CDiagSpeakerPlugin::DoRunTest() OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::DoStopAndCleanupL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::DoStopAndCleanupL()
    {
    LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL IN" )
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
        LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL: Stop & Close AudioPlayer" )
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
    }

    // Restore AudioOutput
   RestoreAudioOutputL();

    if ( ResultsDbItemBuilder().TestResult() == CDiagResultsDatabaseItem::ESuccess )
        {
        // Test was successful. Do not delete the temporary file.
        LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL: Keep file for next test." )
        }
    else
        {
        // test was not successful. Delete recorded file.
        LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL:DeleteRecordFile()" )
        DeleteRecordFile();
        }

    if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL:RestoreAudioOutputL" )
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

    delete iAudioOutput;
    iAudioOutput = NULL;

    // Cleanup Progress Dialog
    LOGSTRING( "Cleanup Progress Dialog" )
    if ( iProgressDialog )
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        }

    SetProgressDismissed( ETrue );

    LOGSTRING( "CDiagSpeakerPlugin::DoStopAndCleanupL OUT" )
    }

// ---------------------------------------------------------------------------
// From CActive
// CDiagSpeakerPlugin::RunL
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::RunL()
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
// CDiagSpeakerPlugin::RunError
// ---------------------------------------------------------------------------
//
TInt CDiagSpeakerPlugin::RunError( TInt aError )
	{
		LOGSTRING2( "CDiagSpeakerPlugin::RunError( %d )", aError )    
		return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// From CActive
// CDiagSpeakerPlugin::DoCancel
// ---------------------------------------------------------------------------
void CDiagSpeakerPlugin::DoCancel()
    {
    // Stop active request. This class does not use Async request using iStatus.
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::DialogDismissedL( TInt aButtonId )
    {
    LOGSTRING2( "CDiagSpeakerPlugin::DialogDismissedL: aButtonId = %d", aButtonId )

    TInt        prevState;

    SetProgressDismissed( ETrue );

    // Restore Audio Output in case of Playing
    if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL: Calling RestoreAudioOutputL" )
        if ( iAudioOutput )
        {
            delete iAudioOutput;
            iAudioOutput = NULL;
        }

        //RestoreAudioOutputL();
        }

    // Stop Recording / Playing
    if ( iAudioState == CMdaAudioRecorderUtility::ERecording )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Stop. Record" )
        if ( iRecorder )
            {
            iRecorder->Stop();
            iRecorder->Close();
            }
        }
    else if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Stop. Play" )
        if ( iAudioPlayer )
            {
            iAudioPlayer->Stop();
            iAudioPlayer->Close();
            }
        }

    // If button has not pressed
    if ( ( aButtonId != ECBACmdSkip )   &&
         ( aButtonId != EEikBidCancel ) &&   // ADO & Platformization Changes
         ( aButtonId != EAknSoftkeyOk ) &&
         ( aButtonId != EAknSoftkeyDone) &&
         ( aButtonId != ECBACmdCancel )
       )
        {
        LOGSTRING2( "CDiagAudioPlugin::DialogDismissedL: Button not pressed. aButtonId = %d", aButtonId )
        return;
        }

    // Skip
    if ( aButtonId == ECBACmdSkip )
        {
        LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: aButtonId == ECBACmdSkip" )
        CompleteTestL( CDiagResultsDatabaseItem::ESkipped );
        return;
        }

    // Cancel
    else if ( aButtonId == EEikBidCancel ) // ADO & Platformization Changes
        {
        LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: aButtonId == ECBACmdCancel" )

        prevState = iAudioState;
        iAudioState    = CMdaAudioRecorderUtility::EOpen;

        TInt   cancelButtonId;
        TBool  cancelResult = EFalse;

        cancelResult = AskCancelExecutionL( cancelButtonId );
        
        if ( !cancelResult )
            {
            LOGSTRING( "CDiagSpeakerPlugin::StartPlayL - !iSessionValid 3" )
            return;
            }

        switch ( cancelButtonId )
            {
            case EAknSoftkeyYes:
                SetAudioEvent(EStateCancelled);
                LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: EAknSoftkeyYes return NOW!!" )
                return;

            default:
                if ( prevState == CMdaAudioRecorderUtility::ERecording )
                    {
                    LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Start RecordL" )
                    RecordL();
                    }
                else if ( prevState == CMdaAudioRecorderUtility::EPlaying )
                    {
                    LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Start PlayL" )
                    PlayL();
                    }
                break;
            }
        return;
        }

    // Stop
    else
        {
        LOGSTRING2( "CDiagSpeakerPlugin::DialogDismissedL: State = %d", iAudioState )
        if ( iAudioState == CMdaAudioRecorderUtility::ERecording )
            {
            LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Record" )
            if ( iRecorder &&  iState == EStateUnknown)
                {
                LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Call AfterRecordL()" )
                SetAudioEvent(EStateRecorded);
                }
            /*
            LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Stop. Call AfterRecordL()" )
            // Stop playback/recording
            AfterRecordL(); */
            }
        else if ( iAudioState == CMdaAudioRecorderUtility::EPlaying )
            {  
            LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Play" )
            if ( iAudioPlayer && iState == EStateRecorded)
                {
                LOGSTRING( "CDiagAudioPlugin::DialogDismissedL:: Stop. Call AfterPlayL()" )
                SetAudioEvent(EStatePlayed);
                }
            /*
            LOGSTRING( "CDiagSpeakerPlugin::DialogDismissedL:: Stop. Call AfterPlayL()" )
            // Stop playback/recording
            AfterPlayL(); */
            }
        }
    }


void CDiagSpeakerPlugin::SetAudioEvent(TState aState)
    {
    iState = aState;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::MoscoStateChangeEvent
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::MoscoStateChangeEvent(
    CBase* /*aObject*/,
    TInt   aPreviousState,
    TInt   aCurrentState,
    TInt   aErrorCode
)
    {
    TInt err = KErrNone;

    LOGSTRING4( "CDiagSpeakerPlugin::MoscoStateChangeEvent:IN (%d -> %d, %d )", aPreviousState, aCurrentState, aErrorCode )

    // 1. Record reaches to the end of file
    if ( aErrorCode == KErrEof )
        {
        iState = EStateRecorded;
        iAudioState = CMdaAudioRecorderUtility::EOpen;
        TRAP( err, AfterRecordL() );
        }

    // 2. Start - Recording
    else if ( ( aPreviousState == CMdaAudioRecorderUtility::ENotReady)  &&
              ( aCurrentState  == CMdaAudioRecorderUtility::EOpen    ) )
        {
        LOGSTRING( "CDiagSpeakerPlugin::MoscoStateChangeEvent: Leave on RecordingL()" )
        TRAP( err, RecordingL() );
        LOGSTRING2( "CDiagSpeakerPlugin::MoscoStateChangeEvent: RecordingL() err = %d", err )
        }

    // 3. Stopped - Recording
    else if ( ( aPreviousState == CMdaAudioRecorderUtility::ERecording ) &&
              ( aCurrentState  == CMdaAudioRecorderUtility::EOpen )
            )
        {
        iState = EStateRecorded;
        LOGSTRING( "CDiagSpeakerPlugin::MoscoStateChangeEvent: Leave on AfterRecordL()" )
        TRAP( err, AfterRecordL() );
        LOGSTRING2( "CDiagSpeakerPlugin::MoscoStateChangeEvent: AfterRecordL() err = %d", err )
        }

    // Handle for Trap
    if (err != KErrNone)
        {
        LOGSTRING2( "CDiagSpeakerPlugin::MoscoStateChangeEvent:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagSpeakerPlugin::MoscoStateChangeEvent:User::Panic()" )
            User::Panic( KDiagSpeakerPluginCategory, err );
            }
        return; // Fail Test
        }
    LOGSTRING( "CDiagSpeakerPlugin::MoscoStateChangeEvent OUT" )
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::MapcInitComplete
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
    {
    LOGSTRING2( "CDiagSpeakerPlugin::MapcInitComplete: aError=%d", aError )
    TInt err = KErrNone;

    // Set Duration
    iPositionMicroSec    = 0;
    iMaxDurationMicroSec = aDuration;

    // Set Priority of AudioPlayer
    iAudioPlayer->SetPriority(
        KAudioPriorityAlarm + 1,
        TMdaPriorityPreference( KAudioPrefVoiceRec )
        );

    // Set Audio Output
    if ( iAudioOutput )
    {
        delete iAudioOutput;
        iAudioOutput = NULL;
    }

    // Create AudioOutput
    TRAP(err,iAudioOutput = CAudioOutput::NewL( *iAudioPlayer ));
    if ( err != KErrNone ) // Added for removing code scanner warning
        {
        LOGSTRING2( "CDiagSpeakerPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );

        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagSpeakerPlugin::MapcInitComplete:User::Panic()" )
            User::Panic( KDiagSpeakerPluginCategory, err );
            }

        return; // Fail Test
        } 
    
    TRAP(err,iAudioOutput->SetAudioOutputL( iOutput ));
    
    if ( err != KErrNone ) // Added for removing code scanner warning
        {
        LOGSTRING2( "CDiagSpeakerPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );

        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagSpeakerPlugin::MapcInitComplete:User::Panic()" )
            User::Panic( KDiagSpeakerPluginCategory, err );
            }

        return; // Fail Test
        } 

    // Start Playing
    TRAP( err, PlayingL() );

    // Handle for Trap
    if ( err != KErrNone )
        {
        LOGSTRING2( "CDiagSpeakerPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );

        if ( err != KErrNone )
            {
            LOGSTRING( "CDiagSpeakerPlugin::MapcInitComplete:User::Panic()" )
            User::Panic( KDiagSpeakerPluginCategory, err );
            }

        return; // Fail Test
        }
    }

// ---------------------------------------------------------------------------
// CDiagSpeakerPlugin::MapcPlayComplete
// ---------------------------------------------------------------------------
//
void CDiagSpeakerPlugin::MapcPlayComplete(TInt aError)
    {
    LOGSTRING2( "CDiagSpeakerPlugin::MapcPlayComplete: aError=%d", aError )

    if ( aError == KErrNone)
        {
        LOGSTRING( "CDiagSpeakerPlugin::MapcPlayComplete KErrNone" )
        
        if ( iAudioOutput )
        {
            delete iAudioOutput;
            iAudioOutput = NULL;
        }

       // RestoreAudioOutputL();

        
        if(iState == EStateRecorded)
            {
        // Stop and Close Audio Player
            if ( iAudioPlayer )
                {
                iAudioPlayer->Stop();
                iAudioPlayer->Close();
                iState = EStatePlayed;
                }
    
            TRAPD(err,AfterPlayL());
            
            // Handle for Trap
				    if ( err != KErrNone )
				        {
				        LOGSTRING2( "CDiagSpeakerPlugin::MapcInitComplete:Call CompleteTestL with error = %d", err )
				        TRAP( err, CompleteTestL(CDiagResultsDatabaseItem::EFailed) );
				
				        if ( err != KErrNone )
				            {
				            LOGSTRING( "CDiagSpeakerPlugin::MapcInitComplete:User::Panic()" )
				            User::Panic( KDiagSpeakerPluginCategory, err );
				            }
				
				        return; // Fail Test
				        }
            }
        return;
        }
    }
// End of File

