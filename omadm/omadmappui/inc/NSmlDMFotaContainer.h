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
* Description:  Declaration of fota view control container
*
*/



#ifndef CNSMLDMFOTACONTAINER_H
#define CNSMLDMFOTACONTAINER_H

//  INCLUDES
#include <aknview.h>    // AVKON components
#include <aknlists.h>
//#include <FotaEngine.h>

// CLASS DECLARATION
class CNSmlDMFotaModel;

/**
*  Control container for NSmlDMSyncApp fota view.
*
*  @lib NSmlDMSyncApp
*  @since Series 60 3.1
*/
class CNSmlDMFotaContainer : public CCoeControl, 
                             public MCoeControlObserver, 
                             public MEikListBoxObserver
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CNSmlDMFotaContainer* NewL( CAknView* aView,
                                           const TRect& aRect,
                                           CNSmlDMFotaModel* aFotaModel );
        
        /**
        * Destructor.
        */
        virtual ~CNSmlDMFotaContainer();

    public: // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * Refreshes the listbox contents.
        * @since Series 60 3.2
        * @param None.
        * @return None.
        */
        
        void RefreshL(TBool aPostponeDisplay);
        
        
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
        * From MEikListBoxObserver Handles list box events.
        * @since Series 60 3.1
        * @param aListBox The originating list box.
        * @param aEventType The event type.
        * @return None.
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

    private:  // New functions
        
        /**
        * Retrieves the required information for the main listbox
        * and formats the item array.
        * @since Series 60 3.1
        * @param aItemsArray Descriptor array that is populated and formatted.
        * @param aPostponeDisplay on ETrue displays the first item
        * @return None.
        */
        void FormatListboxL( CDesCArray* aItemsArray, TBool aPostponeDisplay );

        /**
        * Shows a menu corresponding to the given menu bar
        * resource identifier.
        * @since Series 60 3.1
        * @param aResource Resource identifier of the menu to be shown.
        * @return None.
        */
        void ShowContextMenuL( TInt aResource );
        
    private:

        /**
        * C++ default constructor.
        */
        CNSmlDMFotaContainer( CAknView* aView,
                              CNSmlDMFotaModel* aFotaModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRect& aRect );
        
    private:    // Data
        // Fota view listbox
        CAknDoubleStyleListBox* iFotaListBox;

        // Pointer to the view instance
        CAknView*               iView;

        // Pointer to the fota model instance
        CNSmlDMFotaModel*       iFotaModel;
    };

#endif      // CNSMLDMFOTACONTAINER_H  
            
// End of File
