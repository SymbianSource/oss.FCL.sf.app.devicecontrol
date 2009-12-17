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
* Description:  Main container that displayes test suites.
*
*/



#ifndef CDEVDIAGSUITECONTAINER_H
#define CDEVDIAGSUITECONTAINER_H

// SYSTEM INCLUDES
#include <eiklbo.h>
#include <coecntrl.h> 

// FORWARD DECLARATIONS
class CDevDiagSuiteView;
class CDevDiagEngine;
class MDiagPlugin;
class CAknSingleLargeStyleListBox;

// CLASS DECLARATION

/**
* CDevDiagSuiteContainer dialog class
* 
*/
class CDevDiagSuiteContainer : public CCoeControl,
                             	 public MCoeControlObserver,
                                 public MEikListBoxObserver                                                  
    {    
  
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aView Main view.
        * @param aEngine Model.
        * @param aRect Area to draw on.
        */
        static CDevDiagSuiteContainer* NewL( CDevDiagSuiteView* aView,
                                               CDevDiagEngine& aEngine,
                                               const TRect& aRect );

		/**
        * Destructor.
        */
        ~CDevDiagSuiteContainer();	
	
    public: // Functions from base classes

		/**
        * From MProgressDialogCallback. Gets called when the
        * dialog is dismissed/closed.
        * @param aButtonId Softkey Button Id
        */
        void DialogDismissedL( const TInt aButtonId );

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
        * Display children of the parent.
        *
        * @param aParentUid Suite UID.
        **/
        void DisplayChildrenL( TUid aParentUid );
        
        /**
        * Append plug-in into the listbox
        * 
        * @param aPlugin Plug-in to be added.
        *
        **/
        void DisplayPluginL( MDiagPlugin* aPlugin );
                
        /**
        * Return the plug-in that is selected 
        * Return always a test suite.
        *
        * @param aPlugin The selected plug-in.
        **/
        void SelectedPlugin( MDiagPlugin*& aPlugin ) const;

    protected:

        /**
		* Change of focus
        * @param -
		* @return -
        */
        void FocusChanged(TDrawNow aDrawNow);
	
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
        *
        * @param aView Main view.
        * @param aEngine Model.
        */
        CDevDiagSuiteContainer( CDevDiagSuiteView* aView, CDevDiagEngine& aEngine );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRect& aRect );
                 
	public:  // data
	
	    //Listbox. Own.
		CAknSingleLargeStyleListBox*   iListBox;
	
	    //Main view. 
        CDevDiagSuiteView*       iView;
        
        //Model.
        CDevDiagEngine&           iEngine;
                
        //Currently displayed plug-ins
        //Plug-ins are owned by the engine.
        //Do not call ResetAndDestroy.
        RPointerArray<MDiagPlugin> iChildren;
			
    };

#endif  // CDEVDIAGSUITECONTAINER_H

// End of File
