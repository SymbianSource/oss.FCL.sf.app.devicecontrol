/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plug-in container. Displayes test plug-ins.
*
*/



#ifndef CDEVDIAGPLUGINCONTAINER_H
#define CDEVDIAGPLUGINCONTAINER_H

// INCLUDES
#include <bldvariant.hrh>
#include <eiklbo.h>
#include <aknlists.h>
#include <eikdialg.h>
#include <eikmenup.h>
#include <e32def.h>
#include <aknstaticnotedialog.h>
#include <AknProgressDialog.h> 


// FORWARD DECLARATIONS
class CDevDiagPluginView;
class CDevDiagEngine;
class MDiagPlugin;
class CDiagResultsDatabaseItem;
class CAknDoubleLargeStyleListBox;

// CLASS DECLARATION

/**
* CDevDiagPluginContainer class
* @since S60 5.0
* 
*/
class CDevDiagPluginContainer : public CCoeControl,
                             	public MCoeControlObserver,
                                public MEikListBoxObserver                                                  
    {    
  
    // Container results
    enum TContainerResult
        {
        ESuccess = 0,
        EFailed,           
        ENotPerformed,       
        };
  
    public: // Constructors and destructor
    
        /**
        * NewL.
        *
        * @param aView Plug-in view.
        * @param aEngine Diagnostics Engine.
        * @param aRect The area to draw on.
        */
        static CDevDiagPluginContainer* NewL( CDevDiagPluginView* aView,
                                              CDevDiagEngine& aEngine,
                                               const TRect& aRect );

		/**
        * Destructor.
        */
        ~CDevDiagPluginContainer();	
	
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
        * Handle Pointer EventL
        * @since Series 60 3.2
        * @param  aPointerEvent The event type
        * @return None.
        */
        //void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    public: //New functions       
        
        /**
        * Retrieve currently selected plug-in 
        *
        * @param aPlugin Selected plug-in.
        **/
        void SelectedPlugin( MDiagPlugin*& aPlugin );
        
        /**
        * Display plug-ins and their last results.
        * Last results are retrieved from the results DB.
        * Model provides a helper function for that.
        *
        * @param aArray plug-in result array.
        **/
        void DisplayChildrenWithResultsL(
                        RPointerArray<CDiagResultsDatabaseItem>& aArray );
                    
        /**
        * Change plug-in's text into "Testing ..."
        * The container uses the currently selected plug-in.
        **/                
        void DisplayStartTestingL();
        
        /**
        * Move "Testing..." to the next plug-in in the listbox.
        **/
        void MoveStartTestingDownL();
        
        /**
        * Reset listbox index.
        **/
        void ResetL();
        
        /**
        * Loads children of the parent suite.
        * Container keeps model in memory.
        * @param aParentSuiteUid UID of the parent.
        **/
        void UpdateModelL( TUid aParentSuiteUid );                        

    protected:

        /**
		* Change of focus
        * @param -
		* @return -
        */
        void FocusChanged(TDrawNow aDrawNow);
	
        /**
        * Appends icons to the contained listbox.
        * @param None.
        * @return None.
        */
        void AddIconsL();       

    private:

        /**
        * C++ default constructor.
        */
        CDevDiagPluginContainer( CDevDiagPluginView* aView, CDevDiagEngine& aEngine );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRect& aRect );
        
        /**
        * Load plug-in's test result text (localised).
        **/
        HBufC* GetPluginResultL( TContainerResult aResult ) const;
        
        /**
        * Load plug-in's test result text (localised).
        **/
        HBufC* ConvertResultLC( const TDesC& aString, 
                               TInt& aIconIndex, 
                               CDiagResultsDatabaseItem* aResult ) const;
        
        /**
        * Displays plug-in's result into the listbox.
        *
        * @param aListboxModel Listbox model.
        * @param aPlugin The plug-in that is displayed.
        * @param aItem Plug-in's test result.
        **/
        void DisplayLastResultL( CDesCArray& aListboxModel,
                                 MDiagPlugin*& aPlugin,
                                 CDiagResultsDatabaseItem& aItem ); 
        
        /**
        * Display not performed.
        *
        * @param aListboxModel Listbox model.
        * @param aPlugin The plug-in that is displayed.        
        **/                         
        void DisplayNotPerformedL( CDesCArray& aListboxModel, 
                                  MDiagPlugin*& aPlugin );                                  
                           
	public:  // data
	
	    // The listbox. Own.
		CAknDoubleLargeStyleListBox*   iListBox;
	
	    // Plug-in view. Not own.
        CDevDiagPluginView*       iView;
        
        // Diagnostics engine
        CDevDiagEngine&           iEngine;
                      
        // Currently displayed plug-ins
        // Plug-ins are owned by the engine.
        // Do not call ResetAndDestroy.
        // This is the model
        RPointerArray<MDiagPlugin> iChildren;
        
        // Is the model updated.
        TBool iModelUpdated;			
    };

#endif  // CDEVDIAGPLUGINCONTAINER_H

// End of File
