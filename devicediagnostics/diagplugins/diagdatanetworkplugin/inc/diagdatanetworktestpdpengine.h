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
* Description:  This is the header for the Diagnostics Data Network Test 
*                 PDP-Engine class
*
*/


#ifndef DIAGDATANETWORKTESTPDPENGINE_H 
#define DIAGDATANETWORKTESTPDPENGINE_H

// INCLUDES


#include "diagpdptestengine.h"                      // CDiagPDPTestEngine
#include <DiagNetworkRegStatusObserver.h>           // MDiagNetworkRegStatusObserver
#include <diaglogeventeraserobserver.h>             // MDiagLogEventEraserObserver
#include <AknWaitDialog.h> 
#include "diagdatanetworktestcallbackinterface.h"   // MDiagDataNetworkTestCallBackInterface

//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagNetworkRegStatusWatcher;
class CDiagPluginWaitingDialogWrapper;
class CDiagLogEventEraser;

// CLASS DEFINITION  
class CDiagDataNetworkTestPDPEngine 
    : public CDiagPDPTestEngine,
      public MDiagNetworkRegStatusObserver, 
      public MDiagLogEventEraserObserver,
      public MProgressDialogCallback
    {    
public: 
    /**
    * Symbian OS two-phased constructor
    * @return DataNetwork Test PDP Engine
    */       
    static CDiagDataNetworkTestPDPEngine* NewL(
        MDiagDataNetworkTestCallBackInterface& aInterface);
    
    /**
    * Destructor
    */        
    ~CDiagDataNetworkTestPDPEngine();

public:    
    
    /**
    * The plugin calls this function of engine to start the Data Network test
    */              
    void RunDataNetworkTestL(); 
    
    /**
    * This function return the total steps of the Data network test
    */
    static TUint TotalSteps();

private: // private data-type
            
    /**
    * States of the PDP test engine
    */
    enum TPDPEngineState
        {
        EStateUnknown,                
        EStateWaitForInitialNetworkRegStatus,
        EStateCheckingConnecitonParams,
        EStateWaitForConnectionCompletion,
        EStateWaitForLogErase,
        EStateCancelTest,
        EStateWaitDialogOn,
        EStateDialogDismissed,
        EStateMax            
        };
                  
private: // private functions
    
    /**
    * C++ default constructor.
    */        
    CDiagDataNetworkTestPDPEngine(MDiagDataNetworkTestCallBackInterface& aInterface);
    
    /**
    * Symbian OS default constructor.
    *
    */        
    void ConstructL(); 
    
    /**
    * @see CDiagPDPTestEngine::ReadAPNsFromCenrepL()
    */
    void ReadAPNsFromCenrepL(TInt& aError);
    
    
    /**
    * This function is the entry point to start establishing PDP context
    */         
    void EstablishPDPContextL();
            
    
    /**
    * @see CDiagPDPTestEngine::ReportResultToPluginL()
    */          
    void ReportResultToPluginL(TBool aResult);
    
    
    /**
    * This function is the handles the process of ongoing connection request
    */
    void HandleOngoingConnectionRequestL();    
    
    /**
    * This function starts the process of PDP establishment
    */
    void StartPdpEstablishmentL();
    
    /**
    * This function sets the state of the engine and reports the progress of the
    * test to the plugin class 
    */
    void SetStateL(TPDPEngineState aState);
    
    /**
    * This function starts the process of erasing logs.
    * 
    */
    void StartLogEraseOperationL(TBool aTestResult);
    
    /**
    * Progress Note
    *
    */
    TBool ShowProgressNoteL(TInt);
        
private: // from CActive
    
    /**
    * Handle active object completion event
    */
    void RunL();

    /**
    * Handle cancel
    */
    void DoCancel();

private: // from MProgressDialogCallback
        
    /**
    * Called when the progress dialog is dismissed.
    *
    * @param aButtonId - indicate which button made dialog to dismiss
    */
    virtual void DialogDismissedL( TInt aButtonId );
        
private: // from MDiagNetworkRegStatusObserver

    /**
    * This function is used to inform the intial network registration status
    *
    * @param aRegistered. 
    *  ETrue :The device is currently registered on the network
    *  EFalse:The device is currently not registered on the network.
    */
    void InitialNetworkRegistrationStatusL( const TBool& aRegistered );

    /**
    * This interface is used to inform the clients of any changes in the network registration
    * status
    *
    * @param aRegistered
    * ETrue :The device has now changed to registered status.
    * EFalse:The device has now lost network coverage.
    */    
    void NetworkRegistrationStatusChangeL( const TBool& aRegistered );            
        
        
private: // from MDiagLogEventEraserObserver

    /**
    * This callback interface is used to determine if the log event is
    * to be deleted or not.
    * @param aEvent is the event which has to be deleted
    * @return ETrue if the event is to be deleted and EFalse if the event is
    * not to be deleted
    */
    TBool IsEventToBeDeleted( const CLogEvent& aEvent );
    
    /**
    * This callback interface is used to notify that log events are erased
    */
    void CompleteEventEraseL( TInt aError );
        
        
private: // private data


    /**
    * State of the Data network test engine
    */        
    TPDPEngineState iEngineState;       
    TPDPEngineState iPrevState;     
    
    /**
    * UTC time recorded just before starting the process of PDP connection
    * establishment. This time is used to filter the log events for deletion
    */
    TTime iStartTime;
                                  
    /**
    * Flag to remember the result of the test prior to erasing of logs
    */
    TBool iTestResult;
        
    /**
    * Callback interface used by the engine class to report the progress of the test
    * and result of the test
    */
    MDiagDataNetworkTestCallBackInterface& iInterface;
    
    
    /**
    * This index is used to store the current index of IAP array on which 
    * asynchrouns RConnection::Start() has been issued.
    */ 
    TInt iCurrentIndex;
    
    /**
    * This enum is used to store the current IAP array (primary or secondary) on which 
    * asynchrouns RConnection::Start() has been issued.        
    */
    TIapArray iCurrentArray;
    
    /**
    * For providing prefernce about how to set up the connection
    */
    TCommDbConnPref iPref;
    
    /**
    * Instance of the Diagnostics network registration status watcher
    * Owned by this class
    */
    CDiagNetworkRegStatusWatcher* iNetRegStatusWatcher;
    
    /**
    * Instance of the Diagnostics Log event eraser    
    * Owned by this class
    */
    CDiagLogEventEraser* iLogEventEraser;    
    /**
    *
    * Wait Dialog
    *
    */
    CAknWaitDialog*      iWaitDialog;
    
	TBool idialogOn, idialogDismissed;
    };

#endif //DIAGDATANETWORKTESTPDPENGINE_H

// End of File
