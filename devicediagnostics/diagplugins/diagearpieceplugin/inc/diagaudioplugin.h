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
* Description:  View for Data call settings.
*
*/


#ifndef DIAGAUDIOPLUGIN_H
#define DIAGAUDIOPLUGIN_H

#include <AknProgressDialog.h>               // MProgressDialogCallback
#include <DiagTestPluginBase.h>              // CDiagTestPluginBase
#include <DiagResultsDatabaseItem.h>         // CDiagResultsDatabaseItem::TResult
#include <mdaaudiosampleplayer.h>            // MMdaAudioPlayerCallback
#include <AudioOutput.h>                     // TAudioOutputPreference
#include <MTelephonyAudioRoutingObserver.h>  // MTelephonyAudioRoutingObserver

// FORWARD DECLARATIONS
class CPeriodic;
class CDiagResultsDbItemBuilder;
class CEikDialog;
class CAknNoteDialog;
class CAknProgressDialog;
class CEikProgressInfo;

// CLASS DEFINITION
class CDiagAudioPlugin : public CDiagTestPluginBase,
                         public MProgressDialogCallback,
                         public MMdaObjectStateChangeObserver,
                         public MMdaAudioPlayerCallback,
                         public MTelephonyAudioRoutingObserver
{
public: // Constructors and destructor
    /**
    * Symbian OS two-phased constructor
    *
    * @return Timer Test Plugin
    */
    static MDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor
    */
    virtual ~CDiagAudioPlugin();


private: // private functions
    /**
    * C++ default constructor.
    */
    CDiagAudioPlugin( CDiagPluginConstructionParam* aInitParams  );

    /**
    * Symbian OS default constructor.
    *
    */
    void ConstructL();

    /**
    * Start Audio Recording with Dialogs
    *
    */
    void StartRecordL();

    /**
    * Start Audio Playing with Dialogs
    *
    */
    void StartPlayL();

    /**
    * Audio Recording
    *
    */
    void RecordingL();

    /**
    * Audio Playing
    *
    */
    void PlayingL();

    /**
    * Actions after Recording has finished
    *
    */
    void AfterRecordL();

    /**
    * Actions after Playing has finished
    *
    */
    void AfterPlayL();

    /**
    * Invoke CMdaAudioRecorderUtility::RecordL to start record
    *
    */
    void RecordL();

    /**
    * Invoke CMdaAudioRecorderUtility::PlayL to start play
    *
    */
    void PlayL();

    /**
    * Set Audio to Current Output
    *
    */
    void SaveAudioOutput();

    /**
    * Set Audio to previous Output
    *
    */
    void RestoreAudioOutputL();

    /**
    * Clean up instance of CPeriodic
    *
    */
    void ClosePeriodic();

    /**
    * Check whether Recorded file exist
    */
    TBool CheckRecordFile();

    /**
    * Delete Recorded file
    */
    void DeleteRecordFile();

    /**
    * Check whether there are enough space to record
    *
    * @return ETure when there is enough free space
    */
    TBool EnoughFreeSpace();

    /**
    * Set Dialog has dismissed
    *
    * @param aDismissed - ETrue  for Dialog dismissed
    *                   - EFalse for Dialog has not dismissed
    */
    void SetProgressDismissed( TBool aDismissed );

    /**
    * Check whether Dialog has dismissed
    *
    * @return ETure when Dialog has dismissed
    */
    TBool ProgressDismissed() const;

    /**
    * Ask Cancel all Test
    *
    * @param  aButtonId - reference of button id pressed
    * @return ETure when current state is valid
    */
    TBool AskCancelExecutionL( TInt& aButtonId );

    /**
    * Display Confirmation Query
    *
    * @param aResourceId - Resource ID of Confimation Query
    *
    */
    TBool ShowConfQueryL( TInt aResourceId, TInt &aButtonId );

    /**
    * Display Message Query
    *
    * @param aResourceId - Resource ID of Message Query
    * @param aButtonId   - Reference of button id to exit
    */
    TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId );

    /**
    * Display Progress Dialog
    *
    * @param aResourceId - Resource ID of Progress
    * @param aFinalValue - Final value of Progress Note to display
    */
    void ShowProgressNoteL( TInt aResourceId, TInt aFinalValue );

    /**
    * Callback function to update Progress Dialog
    *
    * @param aThis - Object to receive call back
    * @return 0 on finish
    */
    static TInt CallbackIncrementProgressNoteL( TAny* aThis );

    /**
    * Called from CallbackIncrementProgressNoteL
    *  Actual Function to update Progress Dialog
    *
    * @return 0 on finish
    */
    TInt UpdateProgressNoteL();


private: // From CDiagTestPluginBase

    /**
    * @see CDiagTestPluginBase::Visible()
    */
    virtual TBool IsVisible() const;

    /**
    * @see CDiagTestPluginBase::RunMode()
    */
    virtual TRunMode RunMode() const;

    /**
    * @see CDiagTestPluginBase::TotalSteps()
    */
    virtual TUint TotalSteps() const;

    /**
    * @see CDiagTestPluginBase::GetPluginNameL
    */
    virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

    /**
    * @see CDiagTestPluginBase::Uid
    */
    virtual TUid Uid() const;

    /**
    * @see CDiagTestPluginBase::TestSessionBeginL
    */
    virtual void TestSessionBeginL( MDiagEngineCommon& aEngine,
                                    TBool aSkipDependencyCheck,
                                    TAny* aCustomParams );

    /**
    * @see CDiagTestPluginBase::TestSessionEndL
    */
    virtual void TestSessionEndL( MDiagEngineCommon& aEngine,
                                  TBool aSkipDependencyCheck,
                                  TAny* aCustomParams );

    /**
    * Handle plug-in execution.
    *   @see CDiagTestPluginBase::DoRunTestL()
    */
    virtual void DoRunTestL();

    /**
    * Handle plug-in execution stop and clean up
    *   @see CDiagTestPluginBase::DoStopAndCleanupL()
    */
    virtual void DoStopAndCleanupL();


private: // from CActive
    /**
    * Handle active object completion event
    */
    virtual void RunL();

    /**
    * Handle cancel
    */
    virtual void DoCancel();
    
    enum TState
    {
    EStateUnknown = 0,
    EStateFailed,
    EStateCancelled,
    EStateSkipped,
    EStateIdle,
    EStateRecorded,
    EStatePlayed,
    EStateUserQuery
    };


private: // from MProgressDialogCallback
    /**
    * Callback method Get's called when a dialog is dismissed
    *
    * @param aButtonId - indicate which button made dialog to dismiss
    */
    virtual void DialogDismissedL( TInt aButtonId );


private: // from MMdaObjectStateChangeObserver
    /**
    * Handling the change of state of an audio data sample object
    *
    * @param aObject        - indicate which button made dialog to dismiss
    * @param aPreviousState - previous state
    * @param aCurrentState  - current state
    * @param aErrorCode     - error code
    */
    virtual void MoscoStateChangeEvent(
        CBase *aObject,
        TInt aPreviousState,
        TInt aCurrentState,
        TInt aErrorCode);


private: // MMdaAudioPlayerCallback
     /**
     * Defines required client behaviour when an attempt to open and initialise an audio sample has completed
     *
     * @param aError     - The status of the audio sample after initialisation
     * @param aDuration  - Duration of the audio sample
     */
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);

    /**
     * Defines required client behaviour when an attempt to playback an audio sample has completed
     *
     * @param aError  - The status of playback
     */
    virtual void MapcPlayComplete(TInt aError);


private: // from MTelephonyAudioRoutingObserver
    /**
     * Callback Available Audio Output has changed
     *
     * @param aTelephonyAudioRouting  - Reference of Telephony AudioRouting
     */
    virtual void AvailableOutputsChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );

    /**
     * Callback when Routing Audio Output has chaged
     *
     * @param aTelephonyAudioRouting  - Reference of Telephony AudioRouting
     */
    virtual void OutputChanged( CTelephonyAudioRouting& aTelephonyAudioRouting );

    /**
     * Callback when Set new Telephony Audio Routing is done
     *
     * @param aTelephonyAudioRouting  - Reference of Telephony AudioRouting
     * @param aError                  - The status of set AudioRouting
     */
    virtual void SetOutputComplete( CTelephonyAudioRouting& aTelephonyAudioRouting, TInt aError );
    
    
    void SetAudioEvent(TState aState);


private: // Data
    /**
     * Tells session is Valid
     */
    TBool iSessionValid;

    /**
     * Indicate Progress has dismissed
     */
    TBool iProgressDismissed;

    /**
     * AudioState for Recording/Playing
     */
    TInt iAudioState;

    /**
     * Recorder for Recording Audio
     * OWN
     */
    CMdaAudioRecorderUtility* iRecorder;

    /**
     * Player for Playing Audio
     * OWN
     */
    CMdaAudioPlayerUtility* iAudioPlayer;

    /**
     * Maxmium Size of Record File
     */
    TInt iRecordFileSize;

    /**
     * Path of Record File
     */
    TFileName iRecordFilePath;

    /**
     * Progress Dialog
     * OWN
     */
    CAknProgressDialog* iProgressDialog;

    /**
     * Info for Progress Dialog
     * OWN
     */
    CEikProgressInfo* iProgressInfo;

    /**
     * Timer for Update Progress
     * OWN
     */
    CPeriodic* iPeriodic;

    /**
     * Duration of Audio in usec
     */
    TTimeIntervalMicroSeconds iMaxDurationMicroSec;

    /**
     * Position of Audio in usec
     */
    TTimeIntervalMicroSeconds iPositionMicroSec;

    /**
     * Maximum time(sec) to record
     */
    TInt iFinalValue;

    /**
     * Previous Audio Output
     */
    CTelephonyAudioRouting::TAudioOutput iPrevAudioOutput;

    /**
     * Current Audio Output
     */
    CTelephonyAudioRouting::TAudioOutput iOutput;

    /**
     * Audio Output to route
     * OWN
     */
    CTelephonyAudioRouting* iRouting;
    
    /*
    * State of the Audio Plugin.
    */
    TState iState;
    };

#endif //DIAGAUDIOPLUGIN_H

// End of File

