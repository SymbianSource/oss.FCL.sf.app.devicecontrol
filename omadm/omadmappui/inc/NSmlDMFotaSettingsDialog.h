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
* Description:  Declaration of FOTA Settings Dialog 
*
*/



#ifndef CNSMLDMFOTASETTINGSDIALOG_H
#define CNSMLDMFOTASETTINGSDIALOG_H

//  INCLUDES
#include <AknDialog.h>

// FORWARD DECLARATIONS
class CAknSettingStyleListBox;
class CAknTitlePane;
class CNSmlDMSyncDocument;
class CAknNavigationControlContainer;

// CLASS DECLARATION

/**
*  Dialog for fota settings.
*
*  @lib ?library
*  @since Series 60 3.1
*/
class CNSmlDMFotaSettingsDialog : public CAknDialog,
                                  public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNSmlDMFotaSettingsDialog* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMFotaSettingsDialog();

    protected:  // New functions

        /**
        * Formats the fota settings dialog listbox and items.
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void UpdateFotaSettListboxL();

        /**
        * Called when OK button is pressed.
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void HandleOKL();

    protected:  // Functions from base classes
        
        /**
        * From CEikDialog Called when a key is pressed. Checks
        * if it is ok to exit the dialog.
        * @since Series 60 3.1
        * @param aButtonId Identifier of the button pressed
        * @return 
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CEikDialog Initializes the dialog and the required
        * data. Called before the dialog is shown.
        * @since Series 60 3.1
        * @param None
        * @return None
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog
        * @since Series 60 3.1
        * @param aKeyEvent
        * @param aType
        * @return Was the key event consumed or not.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * From MEikCommandObserver Called when a command is received
        * from the dialog menu.
        * @since Series 60 3.1
        * @param aCommandId Identifier of the received command
        * @return None
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From MEikListBoxObserver Handles events originated from the listbox.
        * @param aListBox Pointer to used listbox
        * @param aEventType Event type
        * @return None
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        /**
        * From CoeControl Gets the control's help context.
        * @since Series 60 3.1
        * @param aContext The control's help context
        * @return None.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMFotaSettingsDialog();

        /**
        * From CAknView Dynamically initialises options menu.
        * @since Series 60 5.0
        * @param aResourceId Id identifying the menu pane to initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        * @return None.
        **/         
        void DynInitMenuPaneL( TInt aResourceID,CEikMenuPane* aMenuPane ); 
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // The listbox of the dialog
        CAknSettingStyleListBox*    iFotaSettListBox;
         
        // Pointer to title pane
        CAknTitlePane*              iTitlePane;

        // Pointer to the application document instance.
        CNSmlDMSyncDocument*        iDocument;
        
        // Original title text storage
        HBufC*                      iOriginalTitle;
        
        // Pointer to the navigation pane
        CAknNavigationControlContainer* iNaviPane;

    };

#endif      // CNSMLDMFOTASETTINGSDIALOG_H   
            
// End of File
