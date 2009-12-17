/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Synchronization handler
*
*/


#ifndef NSMLDMSYNCHANDLER_H
#define NSMLDMSYNCHANDLER_H

// INCLUDES
#include <e32base.h>
#include <AknProgressDialog.h>
#include <SyncMLObservers.h>
#include "NSmlDMProgress.h"
#include "NSmlDMSyncUtil.h"
#include "NSmlDMSyncProfile.h"

// FORWARD DECLARATIONS
class CNSmlDMSyncState;
class CNSmlDMActiveCaller;
class CEikonEnv;
class CNSmlDMSyncDocument;
class CAknWaitDialog;

// CLASS DECLARATION

/**
* CNSmlDMSyncHandler class
* 
*/
NONSHARABLE_CLASS (CNSmlDMSyncHandler) : public CBase, 
                        public MNSmlDMProgressDialogObserver,
						public MNSmlDMActiveCallerObserver,
						public MSyncMLEventObserver,
						public MSyncMLProgressObserver,
						public MProgressDialogCallback
						
	{
    public:
        /**
        * Two-phased constructor.
        */
	    static CNSmlDMSyncHandler* NewL( RSyncMLSession* aSyncSession,
	                                     CNSmlDMSyncAppEngine* aAppEngine,
	                                     CNSmlDMSyncDocument* aSyncDocument );

	    /**
        * Destructor.
        */
	    virtual ~CNSmlDMSyncHandler();

    private:
	    /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	    /**
        * C++ default constructor.
        */
	    CNSmlDMSyncHandler( RSyncMLSession* iSyncSession,
                            CNSmlDMSyncAppEngine* aAppEngine,
	                        CNSmlDMSyncDocument* aSyncDocument );

		
	private:	
        /**
        * From MAspProgressDialogObserver
    	* Gets called when sync progress dialog closes.
        * @param aButtonId Button id.
        * @return None
        */
		void HandleDialogEventL( TInt aButtonId );

    
	private: // from MAspActiveCallerObserver
		/**
        * From MAspActiveCallerObserver
		* Called when CAspActiveCaller completes.
        * @param None
        * @return None.
        */
		void HandleActiveCallL();


    private:    // from MSyncMLEventObserver
        
		/**
        * From MSyncMLEventObserver
		* Called when SyncML session events oocur.
        * @param aEvent Sync event
        * @param aIdentifier Identifier for the sync event
        * @param aError Synce event error
        * @param aAdditionalData.
        * @return None.
        */
        void OnSyncMLSessionEvent( TEvent aEvent,
                                   TInt aIdentifier,
                                   TInt aError,
                                   TInt aAdditionalData );
	
	
    private:  //from MSyncMLProgressObserver
	    
	    /**
	    * Receives notification of a synchronisation error.
        * @param aErrorLevel  The error level.
	    * @param aError		  The type of error. This is one of 
	    *                     the SyncMLError error values.	
	    * @param aTaskId      The ID of the task for which the error occurred.
	    * @param aInfo1       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @param aInfo2       An integer that can contain additional
	    *                     information about the error. Normally 0.
	    * @return             None.
	    */
	    void OnSyncMLSyncError( TErrorLevel aErrorLevel,
	                            TInt aError,
	                            TInt aTaskId,
	                            TInt aInfo1,
	                            TInt aInfo2 );
       	
       	/**
	    * Receives notification of synchronisation progress.
	    * @param aStatus	The current status, e.g. 'Connecting'.
	    * @param aInfo1	    An integer that can contain additional
	    *                   information about the progress.
	    * @param aInfo2	    An integer that can contain additional
        *                   information about the progress.
	    */
	    void OnSyncMLSyncProgress( TStatus aStatus, TInt aInfo1, TInt aInfo2);
	    
	    /**
	    * Receives notification of modifications to synchronisation tasks.
        * @param aTaskId               The ID of the task.
	    * @param aClientModifications  Modifications made on 
	    *                              the clients Data Store.
	    * @param aServerModifications  Modifications made on
	    *                              the server Data Store.
	    */
	    void OnSyncMLDataSyncModifications( 
	                TInt /*aTaskId*/,
	                const TSyncMLDataSyncModifications& aClientModifications,
	                const TSyncMLDataSyncModifications& aServerModifications );
        
	private:

        /**
        * Shows error dialog.
		* @param None.
        * @return Error code.
        */
        void HandleSyncErrorL();
        
        /**
        * Called when sync completes
		* @param aError.
        * @return None.
        */        
        void SynchronizeCompletedL(TInt aError);

        /**
        * Gets sync error text.
		* @param aError.
        * @return Error text.
        */
        void GetErrorText( TDes& aErrorText,
                           TInt aErrorCode,
                           TInt aTransport );
        
        /**
        * Read sync error from CNSmlSyncLog and show error message.
		* @param aProfileId Profile id.
        * @return Result code.
        */
		TInt HandleSyncErrorL(TInt aProfileId);

        /**
        * Set synchronisation error message text.
		* @param aError Error code.
		* @param aMsg Error text.
        * @return None.
        */
		void SetErrorMessageL(TInt aError, TDes& aMsg);
	
	
	public:
        /**
        * Performs synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile identifier.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */
		void SynchronizeL( TDesC& aServerName,
		                   const TInt aProfileId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Performs ServerInitiated synchronization.
        * @param aServerName Name of the server
		* @param aProfileId Profile id
		* @param aJobId Job id.
		* @param aConnectionBearer Bearer
		* @param aUseFotaProgressNote Should the simplified progress note used. Only
		*                             used when checking for firmware updates.
        * @return None
        */		
		void SynchronizeL( TDesC& aServerName, 
		                   const TInt aProfileId,
		                   const TInt aJobId,
		                   const TInt aConnectionBearer,
                           const TBool aUseFotaProgressNote );
		
        /**
        * Shows the progress dialog.
		* @param None.
        * @return None.
        */
        void ShowProgressDialogL();

        /**
        * Deletes the progress dialog if it exists.
		* @param None.
        * @return None.
        */
        void HideProgressDialogL();
        
		/**
        * Cancel synchronization.
		* @param None 
        * @return None
        */
		void CancelSynchronizeL();
		
        /**
        * From MProgressDialogCallback. Handles the situation when the dialog
        * is dismissed.
        * @param aButtonId The identifier of the button, with which the dialog
        *                  was dismissed.
        * @return None
        */
        void DialogDismissedL( TInt aButtonId );
		
	private:
        
        /**
        * Performs synchronization.
        * @return None
        */	
		void SynchronizeL();
		
		/**
        * Utility function.
        * @return Sync session.
        */
		RSyncMLSession& Session();
		
		/**
        * Utility function.
        * @return Progress dialog.
        */
		CNSmlDMProgressDialog* Dialog();

		/**
        * Utility function.
        * @return Sync state.
        */
		CNSmlDMSyncState* State();

    public:

		/**
        * Utility function.
        * @return ETrue if sync is currently running, EFalse otherwise.
        */
		TBool SyncRunning();

    private:
    
		/**
        * Checks the progress state and returns a corresponding
        * dialog text string.
        * @param Reference to the descriptor where the result is stored.
    	* @return None.
        */
        void GetStatusDialogTextL( TDes& aBuffer );
    
		/**
        * Updates progress dialog.
        * @param None.
    	* @return None.
        */
		void UpdateDialogL();

		/**
        * Utility function.
        * @param aText text to show
        * @param aResourceId resource id
        * @return None.
        */
        void ReadL( TDes& aText, TInt aResourceId );

		/**
        * Utility function.
        * @param aText text to show
        * @param aContent 
        * @param aCurrent Progress phase
        * @param aFinal Progress phase
        * @param aPhase Progress phase
        * @return None.
        */
        void ReadProgressTextL( TDes& aText,
                                const TDesC& aContent,
                                TInt aCurrent,
                                TInt aFinal,
                                TInt aPhase );

    private:
		// session with sync server
		RSyncMLSession*                 iSyncSession;
		// app engine
		CNSmlDMSyncAppEngine*           iSyncAppEngine;
        // Pointer to the application document class
        CNSmlDMSyncDocument*            iSyncDocument;
		// profile id
		TInt                            iProfileId;
		// sync job id
		TInt                            iSyncJobId;
		// sync job
		RSyncMLDevManJob                iSyncJob;
		// shows sync progress dialog
		CNSmlDMProgressDialog*          iProgressDialog;
				
        // The alternative wait dialog used in FOTA
        CAknWaitDialog*                 iWaitDialog;
         
        // The progress wait dialog used for any session from main view
        CNSmlDMProgressDialog*          iFotaProgressDialog;
        
        // CheckUpdate state shown or Not
        TBool iCheckUpdate;
                        
		// sync handler state
		CNSmlDMSyncState*               iState;
		// for making function call via active scheduler
		CNSmlDMActiveCaller*            iActiveCaller;
		// is sync currently running
		TBool                           iSyncRunning;
		// sync error code
		TInt                            iSyncError;
		// long buffer for string handling
		TBuf<KBufSize256>               iBuf;
		//Job id
        TSmlJobId                       iJobId;
        // Bearer
        TInt                            iConnectionBearer;
        // Server name
        TBuf<KNSmlMaxProfileNameLength> iServerName;
        // Server sync
        TBool                           iServerAlertedSync;
        // Should the simpler FOTA progress not be used
        TBool                           iUseFotaProgressNote;
        // Retry sync (authentication error)
        TBool							iRetrySync;
          // Variable to show whether in fotaview or not        
        TInt  iNotinFotaView;
        
        // Identifies Fota progress note launched or not
        TBool iFotaProgressLaunched;
	};

#endif  // NSMLDMSYNCHANDLER_H

// End of file
