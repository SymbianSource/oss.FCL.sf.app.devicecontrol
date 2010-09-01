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
* Description:  Header file for Server Profile settings dialog
*
*/


#ifndef NSMLDMDLGPROFILEVIEW_H
#define NSMLDMDLGPROFILEVIEW_H

// INCLUDES

#include <AknDialog.h>  //CAknDialog
#include <eiklbo.h>     //MEikListBoxObserver
#include <akntextsettingpage.h>
#include "NSmlDMdef.h"

// FORWARD DECLARATIONS
class CNSmlDMSyncDocument;
class CEikListBox;
class CEikTextListBox;
class CAknSettingStyleListBox;
class CAknNavigationDecorator;
class CAknTabGroup;
class CNSmlDMSyncAppUi;
class CNSmlDMSyncProfile;
class CNSmlDMSyncAppEngine;
class CNSmlDMProfileData;
class CAknNavigationControlContainer;

typedef CArrayPtr<CNSmlDMProfileData> CNSmlDMSettingItemList;

// CLASS DECLARATION
/**
*  CNSmlDMDlgProfileView
*
*  PRofile view
*/
NONSHARABLE_CLASS (CNSmlDMDlgProfileView) : public CAknDialog, 
                                            public MEikListBoxObserver
    {
    public:
    	enum TNSmlProfile
    		{
    		ENSmlProfileName = 0,
    		ENSmlServerId,
    		ENSmlProfileDMServerPassword,
    		ENSmlConnType,
    		ENSmlAccessPoint,
    		ENSmlProfileURL,
    		ENSmlProfilePort,
    		ENSmlUsername,
    		ENSmlPassword,
    		ENSmlServerAlertAllowed,
    		ENSmlServerAccept,
    		ENSmlHttpsUsed,
    		ENSmlHttpsUsername,
    		ENSmlHttpsPassword
    		};

    public:// Constructors and destructor			
        /**
        * Two-phased constructor.
		* @param aDoc       - pointer to document instance
		* @param aMode      - edit, new or copy profile mode
		* @param aProfileId - profile id
        */
		static CNSmlDMDlgProfileView* NewL( CNSmlDMSyncDocument* aDoc,
		                                    TNSmlEditMode aMode,
		                                    TInt aProfileId );

		/**
		* Destructor
		*/
	    virtual ~CNSmlDMDlgProfileView();
	    
    private:

		/**
		* C++ default constructor.
		* @param aDoc       - pointer to document instance
		* @param aMode      - edit, new or copy profile mode
		* @param aProfileId - profile id
		* @return None.
		*/
		CNSmlDMDlgProfileView( CNSmlDMSyncDocument* aDoc,
		                       TNSmlEditMode aMode,
		                       TInt aProfileId );
	  
    public:
		/**
		* From the base class
        * @param aKeyEvent
		* @param aType
        * @return -
        */
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
		                                   TEventCode aType );

		/**
		* From base class
		* @param CDesCArray* aArray - array of the contacts 
		*                             items on the smlcontact
		* @return -
		*/
		virtual void ProcessCommandL( TInt aCommandId );
		
		/**
		* Called before menu is shown
        * @param TInt aResourceId Menu resource id
		* @param CEikMenuPane* aMenuPane Pointer to the menu
        * @return -
        */
		void DynInitMenuPaneL( TInt aResourceID, CEikMenuPane* aMenuPane );

		/**
		* Handle selection key presses
		* @param -
		* @return -
		*/
		void HandleOKL();

        /**
        * Add setting items to listbox
        * @param aListBox - pointer to listbox
        * @param aItemList - pointer to setting item array
        * @return -
		*/
		void AddProfileListItemL( CEikTextListBox* aListBox,
		                          CNSmlDMSettingItemList* aItemList );

		/**
		* Loads setting dialog title from resource
		* @param -
		* @return -
		*/
		void SetSettingTitleL();
	
		/**
		* Checks the type of the setting item
		* @param CNSmlDMProfileData& aItem - reference to setting item
		* @return ETrue if changed, EFalse otherwise
		*/
		TBool PrepareToEditSettingItemL( CNSmlDMProfileData& aItem );

		/**
		* Exit application when selected from Options menu
		* @param -
		* @return -
		*/
		void DoExitFromMenuL();

		/**
		* Return pointer to CNSmlDMSyncDocument
		* @param -
		* @return CNSmlDMSyncDocument*
		*/
		CNSmlDMSyncDocument* Document() const { return iDoc; }

		/**
		* Method to get context sensitive help topic
        * @param TCoeHelpContext& aContext - Help topic to open
		* @return -
        */
		void GetHelpContext( TCoeHelpContext& aContext ) const;

	private:
	
		/**
        * From CEikDialog OkToExitL( TInt aButtonId );
		* Called by framework before exiting the dialog
        * @param TInt aButtonId - id of the pressed button
		* @return TBool (ETrue to exit\ EFalse to not to exit yet
        */
		TBool OkToExitL( TInt aButtonId );
	
		/**
        * From CEikDialog PreLayoutDynInitL()
		* Called by framework before the dialog is shown
        * @param -
		* @return -
        */
		void PreLayoutDynInitL();

		/**
        * HandleListBoxEventL MEikListBoxObserver
        * @param aListBox Pointer to used listbox
		* @param aEventType Event type
        * @return -
        */
		void HandleListBoxEventL( CEikListBox* aListBox,
		                          TListBoxEvent aEventType );
	
		/**
		* Build list items
		* @param -
		* @return -
		*/
		void CreateProfileSettingsListL();

		/**
		* Launch access point dialog
		* @param aItem Profile item
		* @return Boolean
		*/
		TBool EditSettingItemAccessPointL( CNSmlDMProfileData& aItem );

		/**
		* Check access point
		* @param TUint aId - id of the access point
		* @return -
		*/
		void SetAccessPointInfoL( TUint aId );
		
		/**
		* Get access point info
		* @param TUint aId - id of the access point
		* @return -
		*/		
		void GetAccessPointInfoL( TInt aId );
	
		/**
		* Handle text type setting items
		* @param aItem - reference to setting item
		* @return ETrue if changed, EFalse otherwise
		*/
		TBool EditSettingItemTextL( CNSmlDMProfileData& aItem );

		/**
		* Handle password type setting items
		* @param aItem - reference to setting item
		* @return ETrue if changed, EFalse otherwise
		*/
		TBool EditSettingItemSecretL(CNSmlDMProfileData& aItem);

		/**
		* Handle number type setting items
		* @param aItem - reference to setting item
		* @return ETrue if changed, EFalse otherwise
		*/
		TBool EditSettingItemNumberL(CNSmlDMProfileData& aItem);

		/**
		* Handle list type setting items
		* @param aItem - reference to setting item
		* @return ETrue if changed, EFalse otherwise
		*/
		TBool EditSettingItemListL(CNSmlDMProfileData& aItem);

		/**
		* Handle boolean type setting items
		* @param aItem - reference to setting item
		* @return ETrue to refresh list
		*/
		TBool EditSettingItemListYesNoL(CNSmlDMProfileData& aItem);
		
		/**
		* Handle boolean type setting items
		* @param aItem - reference to setting item
		* @return ETrue to refresh list
		*/
		TBool EditHttpSettingItemListYesNoL(CNSmlDMProfileData& aItem);		

		/**
		* Saves profile
		* @param -
		* @return -
		*/
		void SaveProfileL();

		/**
		* Sets default values for new profile
		* @param -
		* @return -
		*/
		void FillProfileWithDefaultsL();

		/**
		* Check that mandatory fields are filled correctly
		* @param -
		* @return Index of the invalid setting item, -1 if ok.
		*/
		TInt CheckMandatoryFieldsL();

		/**
		* Check that mandatory fields are filled correctly
		* @param -
		* @return Index of the invalid setting item, -1 if ok.
		*/
		TInt CheckUniqueFieldsL();

		/**
		* Launch Confirmation note
		* @param aResource - resource id
		* @return ETrue if accepted, otherwise EFalse
		*/
		TBool ShowConfirmationNoteL(TInt aResource);

		/**
		* Is edited profile just edited
		* @param -
		* @return ETrue if Edit mode
		*/
		inline TBool EditMode() { return iEditMode == ESmlEditProfile; }
		
		/**
		* Is edited profile copied
		* @param -
		* @return ETrue if Copy profile
		*/
		inline TBool CopyMode() { return iEditMode == ESmlCopyProfile; }

		/**
		* Show confirmation note to user before editing setting item
		* @param -
		* @return - return prompt id if warning needed, 0 otherwise
		*/
		TInt WarnUserOfChangeL();

		/**
		* Get item for current index
		* @param aIndex index of item
		* @return ProfileData from item list
		*/
        CNSmlDMProfileData* Item( TInt aItemId );
		
		/**
		* Get item for current index
		* @param aIndex index of item
		* @return ProfileData from item list
		*/        
        CNSmlDMProfileData* IndexItem( TInt aItemId );
		
		/**
		* Get item for current index
		* @param aIndex index of item
		* @return ProfileData from item list
		*/        
        CNSmlDMProfileData* GetItemForIndex( TInt aIndex );

		/**
		* Utility function
		* @param aHeaders
		* @return None
		*/
        void BuildHeaders( CDesCArray* aHeaders );

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildItemIds();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileNameItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileServerIdItemL();
        
		/**
		* Utility function
		* @param None
		* @return None
		*/
        void BuildProfileServerPasswordItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileConnectionTypeItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileAccessPointItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileUrlItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfilePortItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileUserNameandPassWordItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileActiveAcceptedItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileHttpsUsedItemL();

		/**
		* Utility function
		* @param None
		* @return None
		*/        
        void BuildProfileHttpsUserNameAndPasswordItemL();
        
        /**
         * Handle connection setting item selection
         * @param aItem - reference to setting item
         * @return boolean to refresh list
         */
        TBool EditConnectionSettingItemL(CNSmlDMProfileData& aItem );

	private:
		// Non owned data members
        CNSmlDMSyncAppEngine*       iSyncAppEngine;
		CNSmlDMSyncDocument*        iDoc;
		CNSmlDMSettingItemList*     iProfileList;
		CNSmlDMSyncProfile*         iProfile;
		CAknSettingStyleListBox*    iProfileListBox;
		CAknNavigationDecorator*    iNaviDecorator;
		CAknTabGroup*               iTabGroup;
		TNSmlEditMode               iEditMode;
		TInt                        iProfileId;
		TInt                        iOldProfileId;
		TBool                       iServerIdChanged;
		TBuf<KBufSize256>           iBuf;
		CAknNavigationControlContainer* iNaviPane;
		TInt iResourceOffset;
		TInt iConnUtilResourceOffset;
    };

// CLASS DECLARATION
/**
*  CNSmlDMTextEditor
*
*  For Handling Enter keys in Editors
*/

NONSHARABLE_CLASS (CNSmlDMTextEditor) : public CAknTextSettingPage
	{
    public:
    
    /**
		* Constructor
		* @param resource id
		* @param text
		* @param flag
		*/ 
        CNSmlDMTextEditor(TInt aResourceID,TDes& aText,TInt aTextSettingPageFlags);                                                    
         
   /**
		* Destructor
		*/                                            
        ~CNSmlDMTextEditor();   
        
    /**
     * From the base class
     * @param aKeyEvent
     * @param aType
     * @return -Key response
     */             
        
	    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	};



                                            
#endif //NSMLDMDLGPROFILEVIEW_H

// End of File
