/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CNSmlDMSyncDocument
*
*/



#ifndef NSMLDMSYNCDOCUMENT_H
#define NSMLDMSYNCDOCUMENT_H

// INCLUDES
#include <aknapp.h>
#include <AknDoc.h>

#include "NSmlDMSyncUi.hrh"
#include "NSmlDMDlgProfileView.h"
#include "NSmlDMUIDefines.h"
#include "NSmlDMDbNotifier.h"

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikonEnv;
class CNSmlDMSyncAppEngine;
class CNSmlDMSyncProfileList;
class CNSmlDMFotaModel;

// CLASS DECLARATION


/**
*  TNSmlDMProfileItem helper class
*/
NONSHARABLE_CLASS (TNSmlDMProfileItem)
	{
	public:
		TBuf<KNSmlMaxProfileNameLength> iProfileName;	
		TInt     iProfileId;
		TBool	 iSynced;
		TTime	 iLastSync;
		TInt	 iBearer;
		TInt	 iActive;
		TTime    iLastSuccessSync;
		TBool    iDeleteAllowed;
		TBool    iProfileLocked; 
	};

/**
*  CNSmlDMSyncDocument application class.
*/
NONSHARABLE_CLASS (CNSmlDMSyncDocument) : public CAknDocument,
                                          public MNSmlDMDbEventHandler
    {
    enum TSanSupport
			{
			EOff,					
	        EOn
			};	
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNSmlDMSyncDocument* NewL( CAknApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CNSmlDMSyncDocument();

    public: // New functions

		/**
		* Returns the pointer to the appengine
        * @param -
		* @return Pointer to appEngine
        */
		inline CNSmlDMSyncAppEngine* AppEngine() const { return iAppEngine; }

		/**
		* Returns the id of the recently accessed profile
        * @param -
		* @return - old profile id
        */
		inline TInt OldProfileID() const { return iOldProfileId; }

		/**
		* Return the number of items
		* @param -
		* @return Profile item count
		*/
		inline TInt ProfileCount() const { return iProfileList->Count(); }

        /**
        * Returns the pointer to the fota model.
        * @since Series 60 3.1
        * @param None.
        * @return Pointer to the fota model instance.
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

		/**
		* Return the pointer to the profileitem list
        * @param aIndex Sets current index on the caller
		* @return Pointer to the profileitem list
        */
		CArrayFixFlat< TNSmlDMProfileItem >* ProfileList( TInt& aIndex );

		/**
		* Set current index
        * @param aIndex - new index value
		* @return -
        */
		void SetCurrentIndex(TInt aIndex);

		/**
		* Reads profiles again from the database and fills the profilelist
        * @param -
		* @return -
        */
		inline void RefreshProfileListL() { ReadProfileListL(); }
		
		/**
		* Reads profiles again from the database and fills the profilelist
        * @param aIncludeHidden Should hidden profile be included
		* @return -
        */
		inline void RefreshProfileListL( TBool aIncludeHidden ) { ReadProfileListL( aIncludeHidden ); }

		/**
		* Deletes selected profile and updates new index
        * @param  aIndex Set new index
		* @return (1 the deleted profile was default one /
		* 0 deleted was other than default or there was only one left)
        */
		TInt DeleteProfile( TInt& aIndex );

		/**
		* Return the current profile item
		* @param -
		* @return Pointer to the current TProfileItem profile item
		*/
		TNSmlDMProfileItem* ProfileItem() const;

		/**
		* Return the current profile item
		* @param -
		* @return Pointer to the indexed TProfileItem profile item
		*/
		TNSmlDMProfileItem* ProfileItemAt( TInt aIndex ) const;

		/**
		* Return the pointer to the CEikAppUi class
		* @param -
		* @return Pointer to the CEikAppUi
		*/
		CEikAppUi* AppUi() const { return iAppUi; }

		/**
		* Moves the index on the main view to the profile
		* with specified profile Id
		* @param aProfileId profile id
		* @return -
		*/
		void MoveIndexToProfile( TInt aProfileId );

		/**
		* Copies an existing profile settings to a new id
		* @param  aProfileId profile id
		* @return -
		*/
		void GetCopyProfileL( TInt& aProfileId );

		/**
		* Return next available default profile name
		* @param -
		* @return - new default profile name
		*/
		HBufC* GetNextProfileNameLC();

        /**
		* Disables DB events
		* @param aEvent 
		* @return None
		*/
        void DisableDbNotifications( TBool aEvent );
        
		/**
		* Handles DB events
		* @param aEvent DB event
		* @return None
		*/
        void HandleDbEventL( TNSmlDMDbEvent aEvent );
        
        /**
        * Utility function.
        * @param -
        * @return None.
        */                                
        TBool IsServerAlertSync();
  
        /**
        * Utility function.
        * @param -
        * @return None.
        */           
        void StartServerL();
        
        /**
        * ServerAlert Support function
        * @param -
        * @return TBool.
        */
        TBool SANSupport();
        
        /**
        * ServerAlert type return function
        * @param -
        * @return TBool.
        */
        TBool SilentSession();
        
         /**
        * ServerAlert type variable reset function
        * @param -
        * @return None.
        */
        void ResetSession();
        
        /**
        * Server profiles view current index
        * @param -
        * @return index of the current focussed profile.
        */
        TInt CurrentIndex();
        
        /**
         * FOTA set DL active P&S key defined by DM or not
         * @param None
         * @return ETrue or EFalse.
         */
         TBool DMDefinesFotaDLKey();
        
    private:
        /**
        * C++ default constructor.
        */
        CNSmlDMSyncDocument(CEikApplication& aApp): CAknDocument(aApp) { }

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		/**
		* Read settings from the database
        * @param -
		* @return -
        */
		void ReadProfileListL();

		/**
		* Read settings from the database
        * @param aIncludeHidden Should hidden profile be included
		* @return -
        */
		void ReadProfileListL( TBool aIncludeHidden );

		/**
        * From CEikDocument, create CNSmlDMSyncAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
        
		/**
		* Restore
        * @param aStore Stream  store
        * @param aStreamDic Dictionary
		* @return None
        */
        void RestoreL( const CStreamStore& aStore, 
                       const CStreamDictionary& aStreamDic );
		
    private:
        //Application engine
		CNSmlDMSyncAppEngine*               iAppEngine;
		// Profile list
		CNSmlDMSyncProfileList*             iSyncProfileList;
		// DB notifier
		CNSmlDMDbNotifier*                  iDbNotifier;
		//Profile list array
		CArrayFixFlat<TNSmlDMProfileItem>*  iProfileList;
		// Current index
		TInt                                iCurrentIndex;	
		// Eikon environment
		CEikonEnv*                          iEikEnv;
		// Old profile id
		TInt                                iOldProfileId;	
		// Blocks db events
		TBool                               iDbEventsBlocked;
		
		CApaAppServer*                      iServer;
		//Server alert support
		TInt  								iSanSupport;
		//Silent session or not
		TBool								iSilent;
		//FOTA DL Active P&S key defined by DM or not
		TBool 							iDMDefinesFotaDLKey;
    };

#endif // NSMLDMSYNCDOCUMENT_H

// End of File
