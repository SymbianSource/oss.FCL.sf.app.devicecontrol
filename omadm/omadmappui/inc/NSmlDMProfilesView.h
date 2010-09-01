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
* Description:  Header file for Server Profiles View
*
*/



#ifndef CNSMLDMPROFILESVIEW_H
#define CNSMLDMPROFILESVIEW_H

// INCLUDES
#include <aknview.h>

// FORWARD DECLARATIONS
class CNSmlDMProfilesContainer;

// CLASS DECLARATION

/**
* CNSmlDMProfilesView dialog class
* 
*/
class CNSmlDMProfilesView : public CAknView
    {
    public: // Constructors and destructor
        
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

		/**
        * Destructor.
        */
        ~CNSmlDMProfilesView();

    public: // New functions

        /**
        * Refreshes the container contents.
        * @since Series 60 3.1
        * @param None.
        * @return None.
        */
        void RefreshL();

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
	    * @since Series 60 5.0
	    * @param aForeground         
	    * @return None.
	    */ 
		void HandleForegroundEventL(TBool aForeground);   

    private: //data
        // Control container for profiles view.
        CNSmlDMProfilesContainer*       iContainer;
    };

#endif // CNSMLDMPROFILESVIEW_H

// End of File
