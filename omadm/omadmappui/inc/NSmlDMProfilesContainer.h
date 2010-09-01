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
* Description:  Header file for CNSmlDMProfilesContainer
*
*/



#ifndef CNSMLDMPROFILESCONTAINER_H
#define CNSMLDMPROFILESCONTAINER_H

// INCLUDES
#include <bldvariant.hrh>
#include <eiklbo.h>
#include <aknlists.h>
#include <eikdialg.h>
#include <eikmenup.h>
#include <e32def.h>
#include <aknstaticnotedialog.h>
#include "NSmlDMSyncDocument.h"
#include "NSmlDMSyncAppUi.h"

// FORWARD DECLARATIONS
class CNSmlDMSyncDocument;
class TNSmlDMProfileItem;

// CLASS DECLARATION

/**
* CNSmlDMProfilesContainer dialog class
* 
*/
class CNSmlDMProfilesContainer : public CCoeControl,
                                 public MCoeControlObserver,
                                 public MEikListBoxObserver
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CNSmlDMProfilesContainer* NewL( CAknView* aView,
                                               const TRect& aRect );

		/**
        * Destructor.
        */
        ~CNSmlDMProfilesContainer();

    public: // New functions

        /**
        * Refreshes the listbox contents.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void RefreshL();

		/**
        * Sets the iSettingsView true/false if settingsview is up/down
        * @param -
		* @return -
        */
		inline void SetSettingsView() { iSettingsView = !iSettingsView; }

		/**
        * Return status of the Settings view
        * @param -
		* @return True if settings isn't open
        */
		inline TBool SettingsView() const { return iSettingsView == EFalse; }

		/**
        * Update index on the main view
		* @param TInt aIndex - value of the index
		* @return -
        */
		void SetCurrentIndex( TInt aIndex );
		
        /**
        * Shows a menu corresponding to the given menu bar
        * resource identifier.
        * @since Series 60 3.1
        * @param aResource Resource identifier of the menu to be shown.
        * @return None.
        */
        void ShowContextMenuL( TInt aResource );

		/**
        * Read profiles and fill the profile item list
		* @param -
		* @return -
        */
		void ReadProfileListL();

		/**
		* Create and show the logview dialog
		* @param -
		* @return -
        */
		void PopupLogViewL();

		/**
        * Close the active popupmenu
		* @param -
		* @return -
        */
		void ClosePopupMenuL();

		/**
		* Status of the main dialog menu
        * @param -		
        * @return - ETrue if menu is up
        */
		TBool MenuUp() const { return iMenuUp == 1; }
		
		/**
		* Closes menu
        * @param -		
        * @return - 
        */
		void CloseMenuUp() { iMenuUp =0; }

    public: // Functions from base classes

        /**
        * From CoeControl Responds to size changes. 
        * Sets the size and position of the contents of this control.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void SizeChanged();

        /**
        * From CoeControl Handles the situations where
        * a resource (e.g. skin or layout) is changed.
        * @since Series 60 3.1
        * @param aType Type of resource change.
        * @return None.
        */
        void HandleResourceChange( TInt aType );

        /**
        * From CoeControl Gets the number of controls contained 
        * in a compound control.        
        * @since Series 60 3.1
        * @param None.
        * @return The number of component controls contained by this control.
        */
        TInt CountComponentControls() const;

        /**
        * From CoeControl Gets the specified component of a compound control.
        * @since Series 60 3.1
        * @param aIndex The index of the control to get.
        * @return Reference to the component control.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CoeControl Draws the control.
        * @since Series 60 3.1
        * @param aRect The region of the control to be redrawn.
        * @return None.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CoeControl Handles key events.
        * @since Series 60 3.1
        * @param aKeyEvent The key event that occurred.
        * @param aType The event type.
        * @return Response to the key event.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );       

		/**
        * Handle ListBox EventL
        * @param CEikListBox* aListBox Pointer to used listbox
		* @param TListBoxEvent aEventType Event type
        * @return -
        */
		void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
		
        /**
        * From MCoeControlObserver Handles an event from an observed control.
        * @since Series 60 3.1
        * @param aControl The control that sent the event.
        * @param aEventType The event type.
        * @return None.
        */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );        

        /**
        * From CoeControl Gets the control's help context.
        * @since Series 60 3.1
        * @param aContext The control's help context
        * @return None.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        /**
        * Handle Pointer EventL
        * @since Series 60 3.2
        * @param  aPointerEvent The event type
        * @return None.
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    protected:

        /**
		* Change of focus
        * @param -
		* @return -
        */
        void FocusChanged(TDrawNow aDrawNow);

    protected:  // Functions from CEikDialog

        /**
		* Called by framework before the dialog is shown
        * @param -
		* @return -
        */
        void PreLayoutDynInitL();

        /**
		* Called by framework after the dialog is shown
        * @param -
		* @return -
        */
		void PostLayoutDynInitL();
		
		/**
		* Build and format the logview listbox texts
		* @param aArray -
		* @return -
        */
		void FormatLogViewListL(CDesCArray* aArray);
		
        /**
        * Appends icons to the contained listbox.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void AddIconsL();
		
    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMProfilesContainer( CAknView* aView );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRect& aRect );
        
	public:  // data
		CAknDoubleLargeStyleListBox*    iProfilesListBox;

    private: // data
		CNSmlDMSyncAppUi*               iAppUi;
		CNSmlDMSyncDocument*            iDocument;
        // Pointer to the view instance
        CAknView*                       iView;

		TBool                           iContextMenuUp;
		TBool                           iConfirmationNoteUp;
		TInt                            iMenuUp;
		TBool                           iSettingsView;
    };

#endif  // CNSMLDMPROFILESCONTAINER_H

// End of File
