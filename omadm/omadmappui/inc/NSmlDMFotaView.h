/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares the firmware update view for the application
*
*/



#ifndef CNSMLDMFOTAVIEW_H
#define CNSMLDMFOTAVIEW_H

//  INCLUDES
#include <aknview.h>
#include <AknWaitNoteWrapper.h>
#include <e32property.h>
#include "NSmlDMProgress.h"

// FORWARD DECLARATIONS
class CNSmlDMFotaContainer;
class CNSmlDMFotaModel;
class CAknIconArray;
class CDLObserver;
// CLASS DECLARATION

/**
*  Fota view in the NSmlDMSyncApp.
*
*  @lib NSmlDMSyncApp
*  @since Series 60 3.1
*/
enum TFotaState
			{
			/** No firmware update has been started */
	    EIdle                       = 10,
	    /** Client has sent a client initiated request */
	    EClientRequest              = 5,
	    /** There is no data available and download is about to start */
	    EStartingDownload           = 15,
	    /** Download failed and there is no data received */
	    EDownloadFailed             = 20,
	    /** Download is progressing with resume support. This state shouldn't be set as FUMO state*/
		EDownloadProgressingWithResume	= 25,
	    /** Denotes that a download has started and that 0 or more bytes of 
        data have been downloaded */
	    EDownloadProgressing        = 30,
	    /** Have data and download has been completed successfully */
	    EDownloadComplete           = 40,
	    /** Have data and about to start update */
	    EStartingUpdate             = 50,
	    /** Denotes that the update is currently running, but has not yet 
        completed */
	    EUpdateProgressing          = 60,
	    /** Have data but update failed */
	    EUpdateFailed               = 70,
	    /** Update failed and data deleted or removed */
	    EUpdateFailedNoData         = 80,
	    /** Update complete and data still available */
	    EUpdateSuccessful           = 90,
	    /** Data deleted or removed after a successful update */
	    EUpdateSuccessfulNoData     = 100,	
			};

class CNSmlDMFotaView : public CAknView, public MAknBackgroundProcess
    {
    friend  class CDLObserver;
    public:  // Constructors and destructor
        
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMFotaView();

    public: // Functions from base classes

        /**
        * From CAknView Returns view id.
        * @since Series 60 3.1
        * @param None.
        * @return View identifier.
        */
        TUid Id() const;
		
		/**
        * From CAknView Handles user commands.
        * @since Series 60 3.1
        * @param aCommand The command identifier.
        * @return None.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MAknBackgroundProcess Is called periodically by the
        * wait note wrapper.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void StepL();
		
        /**
        * From MAknBackgroundProcess Checks if the process is finished
        * and the wait note can be dismissed.
        * @since Series 60 3.1
        * @param None.
        * @return Can the wait note be dismissed.
        */
		TBool IsProcessDone() const;
		
	public: // New functions
        
        /**
        * Refreshes the container contents.
        * @since Series 60 3.2
        * @param aPostponeDisplay, on ETrue adds the first list item
        * @return None.
        */
        
        void RefreshL(TBool aPostponeDisplay);
        
        
        /**
        * For setting the iExitPress value.        
        * @since Series 60 3.2
        * @param None.
        * @return None.
        */
        void UpdateExitL();
        	
    private:  // New functions
        
        /**
        * ?member_description.
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void InitiateFotaCheckL();

        /**
        * Shows a popup list of available device management
        * profiles. Leaves if the query is cancelled.
        * @since Series 60 3.1
        * @param None
        * @return Identifier of the selected profile.
        */
        TInt ShowFotaProfileQueryL();
        
        /**
        * Formats the fota initiation profile query
        * listbox items.
        * @since Series 60 3.1
        * @param aItems Array of items included in the listbox
        * @param aProfileIdList Is changed to an array of profile identifiers. The order is the same as the listbox itmes.
        * @return None
        */
        void FormatProfileQueryListItemsL( CDesCArray*          aItems,
                                           CArrayFixFlat<TInt>* aProfileIdList );

        /**
        * Creates a listbox icon array containing the device
        * management bearer icons.
        * @since Series 60 3.1
        * @param aIconArray Pointer to the icon array to be formatted.
        * @return None
        */
        void CreateIconArrayL( CAknIconArray* aIconArray );

        /**
        * Initiates a firmware update package installation.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void InitiateFwUpdInstallL();
		
		/**
        * Continues a firmware update package installation which is suspended.
        * @since Series 60 3.2
        * @param None.
        * @return None.
        */
        void InitiateFwUpdContinueL();

        /**
        * Manual check allowed or not.
        * @since Series 60 3.2
        * @param None.
        * @return TBool based on manual; check allowed or not.
        */
        TBool ManualCheckAllowedL();
        
        /**
        * Enabling context options msk.
        * @since Series 60 3.2
        * @param TBool aEnabled.
        * @return None.
        */
        void EnableMskL ( TBool aEnable );
                                
    private:  // Functions from base classes
        /**
        * From CAknView Activates view.
        * @since Series 60 3.1
        * @param aPrevViewId Id of previous view.
        * @param aCustomMessageId Custom message id.
        * @param aCustomMessage Custom message.
        * @return None.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                                TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView Deactivates view.        
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void DoDeactivate();

        /**
        * From CAknView Dynamically initialises options menu.
        * @since Series 60 3.1
        * @param aResourceId Id identifying the menu pane to initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        * @return None.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    protected:
        /**
        * From CAknView to handle foreground events
        * @since Series 60 3.2
        * @param aForeground         
        * @return None.
        */ 
    	void HandleForegroundEventL(TBool aForeground);        

    private:    // Data
        // Container for view contol.
        CNSmlDMFotaContainer*   iContainer;
        
        // Pointer to the fota model instance.
        CNSmlDMFotaModel*       iFotaModel;
        
        // Pointer to the wait note wrapper that is used during
        // the manual checking of firmware updates.
        CAknWaitNoteWrapper*    iWaitNoteWrapper;
        
        //For storing cenrep value of manual check updates
        TInt iManCheckAllowed;
      
        //T be used in HandleForegroundeventL for Exit and End key press
        TInt iExitPress;

		// Active object for refreshing main view    
        CDLObserver* iFotaDLObserver;
    };

// CLASS DECLARATION
// Active object for refreshing main view    
class CDLObserver : public CActive
	{	
	friend class CNSmlDMFotaView;	
	public:
	
		/**
	     * Constructor.
	     */
		inline CDLObserver();    	
		
		/**
	     * Starts the active object monitoring on P&S key
	     * @param aResetKey,whcih sets the P&S key 
	     * @return None
	     */
		void StartL(TBool aResetKey);
		
		/**
	     * Destructor.
	     */    	
		~CDLObserver();
		
	private:
	
		/**
	     * From CActive. Called when asynchronous request completes.
	     * @param None
	     * @return None
	     */
		void RunL();
		
		/**
	      * From CActive. Cancels asynchronous request.
	      * @param None
	      * @return None
	      */
		void DoCancel();
		
		/**
	     * Symbian 2nd phase constructor.
	     */
		void ConstructL(CNSmlDMFotaView *iFotaView);
		
		/**
	     * From CActive. Called when RunL Leaves.
	     * @param None
	     * @return error
	     */
	    TInt RunError( TInt aError );    	    	
	    
	private:	      	
		CNSmlDMFotaView* iView;
		RProperty iProperty;    
	}; 

#endif      // CNSMLDMFOTAVIEW_H   
            
// End of File
