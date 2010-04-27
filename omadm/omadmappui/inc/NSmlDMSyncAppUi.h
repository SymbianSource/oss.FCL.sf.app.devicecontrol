/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for Application UI Class CNSmlDMSyncAppUi
*
*/



#ifndef __NSMLDMSYNCAPPUI_H
#define __NSMLDMSYNCAPPUI_H

// INCLUDES
#include <bldvariant.hrh>

#include <aknViewAppUi.h>
#include <akntitle.h>

#include "NSmlDMdef.h"
#include "NSmlDMSyncAppEngine.h"

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

class CNSmlDMProfilesView;
class CNSmlDMDlgProfileView;
class CNSmlDMSyncDocument;
class CNSmlDMSyncAppEngine;
class CNSmlDMFotaModel;
class CNSmlDMDbNotifier;
class CNSmlDMFotaView;


/** If download is active this P&S key is set to ETrue, otherwise EFalse. 
 * The key is owned by omadmappui**/
const TUint32 KFotaDownloadActive = 0x0000008;

/**
* Application UI class.
* Provides support for the following features:
* - dialog architecture
* 
*/
NONSHARABLE_CLASS (CNSmlDMSyncAppUi) : public CAknViewAppUi,
    public MNSmlDMSyncObserver
   
    {
	enum TViews
		{
		MainView,
		LogView,
		SettingsView,
		ProgressView
		};

    public: // Constructors and destructor
	
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CNSmlDMSyncAppUi();
        
    public: // New functions
		
		/**
		* Used to close the settings view gracefully
        * @param aRefresh
        * @return  -
        */
		void ChangeViewL( TBool aRefresh = EFalse );

		/**
		* Help context
        * @param None.
        * @return  pointer to CArrayFix<TCoeHelpContext>
        */
		CArrayFix<TCoeHelpContext>* HelpContextL() const;
		/**
		* Shows profile editing dialog
        * @param aMode (EEditProfile to edit or ENewProfile to create a profile
        * @return - 
        */
        void ShowEditProfileDialogL( TNSmlEditMode aMode, TInt aStartUp = 0 );

		/**
		* Public method to access HandleCommandL outside the class
        * @param aCommand -forwards command from listquery
        * @return - 
        */
        void HandleContextMenu( TInt aCommand ) { HandleCommandL( aCommand ); }

		/**
		* Called before the application is started
        * @param aCommand - StartUp command
		* @param aDocumentName - Document name
		* @param aTail - Additional information (profileID)
        * @return - 
        */
		TBool ProcessCommandParametersL( TApaCommand aCommand,
		                                 TFileName& aDocumentName,
		                                 const TDesC8& aTail );
		
		/**
		* Called by settingsdialog to set if the settingdialog is properly closed
        * @param -
        * @return - 
        */
		void DoExit() { iDestroyed = 0; 
			              CloseGs();
			              
			            }

		/**
		* Called to exit Dm application and control panel
        * @param -
        * @return - 
        */
        void CloseGs();


		/**
		* Shows error note
        * @param TInt aResource resource id
		* @return -
        */
		void ShowResourceErrorNoteL( TInt aResource ) const;

        /**
        * Returns the iFotaModel pointer.
        * @param None.
        * @return Pointer to the CNSmlDMFotaModel instance or NULL if it does not exist.
        */
        CNSmlDMFotaModel* FotaModel() const;
        
        /**
        * Saves the current state of update packages.
        * @param None.
        * @return None.
        */
        void MarkFwUpdChangesStartL();

        /**
        * Checks if the state of update packages has changed since the saved
        * situation.
        * @param None.
        * @return True if there are differences in pcakage identifiers or states.
        */
        TBool FwUpdStatesChangedL();
        

        void SetTitleCall(TInt aView)
        {
        iViewTitle = aView;	
        }


    public: // Functions from base classes
		
		/**
		* Initiates a device management session with a server.
        * @param aProfileId Profile identifier, with which the session is be
        *                   started. If left empty, defaults to Currently
        *                   selected profile in the list.
        * @param aUseFotaProgressNote If true, the progress notes are replaced
        *                             with a simpler FOTA specific note and
        *                             changes in firmware update packages are
        *                             checked after the session.
        * @return None
        */
		void StartSyncL( const TInt aProfileId = KErrNotFound,
		                 const TBool aUseFotaProgressNote = EFalse );

		/**
		* Confirms if the synchronisation on and the progress dialog is up
        * @param None
        * @return ETrue if sync running
        */
		TBool Synchronise();
			
		/**
		* Handles error notes
        * @param aError error number
        * @return -
        */
		void ShowErrorNoteL( TInt aError ) const;

        void SyncComplete( TNSmlStatus aStatus );
        
        /**
		* Sets the iExitPress value of fota view
        * @param None
        * @return -
        */        
        void ExitCallL();        
        
        /**
		* Refrsehes Main view 
        * @param None
        * @return -
        */
        void RefreshMainViewL();
        
        /**
 		* Used to check the fota download going 
        * @param aNone
        * @return  aNone
        */
        void CheckFotaDlL();        
        
        void CheckAMDlL();    
    private:

               
		/**
		* From MEikMenuObserver, called by the framework before
		*                        the options menu is shown
        * @param aResourceId - resourceid of the menu
		* @param aMenuPane - pointer to that menu
        * @return - 
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:

        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand - id of the  command to be handled
		* @return -
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent - Event to handled.
        * @param aType - Type of the key event. 
        * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, 
                                              TEventCode aType );
                                              
	private:

		/**
        * Handles up scroll and forwards the action to right view
        * @param -
		* @return -
        */
		void HandleUpScrollL();

		/**
        * Handles down scroll and forwards the action to right view
        * @param -
		* @return -
        */
		void HandleDownScrollL();

		/**
        * Handles the error in synchronisation
        * @param aProfileId the id of the 
		* @return 0 if no error or user cancellation
        */
		TInt HandleSyncErrorL( TInt aProfileId );

		/**
        * Handles the error message text in synchronisation
        * @param aError Error value
		* @param aMsg the error message
		* @return -
        */
		void SetErrorMessageL( TInt aError, TDes& aMsg ) const;

		/**
		* Shows confirmation dialog
        * @param aResource prompt resource id
		* @return ETrue in case note is accepted or EFalse if rejected
        */
		TBool ShowConfirmationNoteL( TInt aResource ) const;
		
		/**
		* Utility function for handling New profile command
        * @param    None
		* @return   None
        */
        void HandleCommandNewProfileL();
        
		/**
		* Utility function for handling erase profile command
        * @param    None
		* @return   None
        */        
        void HandleCommandEraseProfileL();

       /**
        * Determines whether Fota update is allowed.
        * @param    Server profile to check
        * @return   ETrue if allowed
        *           EFalse if not not allowed
        */
        TBool IsUpdateAllowedL( CNSmlDMSyncProfile& aProfile );


	private: //Data
	    CNSmlDMProfilesView*        iProfilesView;
	
		CNSmlDMDlgProfileView*      iAppView;
		CNSmlDMDbNotifier*          iSmlDbNotifier;
		CEikStatusPane*             iStatusPane;
		CAknNavigationControlContainer* iNaviPane;  // NaviPane
		CAknNavigationDecorator*    iNaviDecorator;

		TInt iViewTitle;// 1 for DM main view, 0-for servers view

		CNSmlDMFotaView* iMainView;
		//	Left/RightSoftkeys and scrolling
		CEikButtonGroupContainer*   iControlPane;
		TInt                        iActiveView;
		CAknTitlePane*              iTitlePane;
		
		TInt                        iDialogUp;
		TInt                        iDestroyed;
		TInt                        iConfedProfileId;
		TBool                       iDbChanged;
		TBool                       iBusy;
		
		CNSmlDMSyncDocument*        iSyncDocument;
		CNSmlDMSyncAppEngine*       iSyncAppEngine;

        // Pointer to the fota model.
        CNSmlDMFotaModel*           iFotaModel;
        
    };

#endif

// End of File
