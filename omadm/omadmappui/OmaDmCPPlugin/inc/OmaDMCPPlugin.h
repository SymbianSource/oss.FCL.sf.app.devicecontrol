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
* Description:  This class implements ECom interface for OmaDmCPPlugin.dll
*
*/


#ifndef DM_CP_PLUGIN_H
#define DM_CP_PLUGIN_H

// System includes
#include <gsplugininterface.h>

// Move this EasAppUids.h
#define KUidOmaDmCPPluginDll   0x1028339D
#define KUidOmaDmCPPluginImpl  0x1028339E


class CAknNullService;

/**
* OmaDmCP ECom Plugin
*/
class COmaDmCPPlugin : public CGSPluginInterface,
                     public MAknServerAppExitObserver
    {    
	public: 

	    /**
	    * Two-Phased constructor
	    * Uses Leave code KErrNotFound if implementation is not found.
	    */                                                 
	    static CGSPluginInterface* NewL();
	    
	    /**
	    * From CAknView Returns this view Id.
	    */
		TUid Id() const;
	    
	    /**
	    * returns the caption to be displayed in GS view
	    *
	    * @param aCaption - The caption to be displayed is to be copied here.
	    */
	    void GetCaptionL( TDes& aCaption ) const;
		
		/**
		* returns the icon to be displayed in GS view
		* 
		* @param aIconType - type of icon to be displayed
		*/
	    CGulIcon* CreateIconL( const TUid aIconType );
	    
	    /**
	    * Destructor
	    */
	    ~COmaDmCPPlugin();            
		
		/**
        * From CGSPluginInterface.
        * @return this plugin's provider category.
        */
        TInt PluginProviderCategory() const;

        /**
        * From CGSPluginInterface.
        * @return this plugin's item type.
        */
        TGSListboxItemTypes ItemType();

        /**
        * From CGSPluginInterface.
        * @param aSelectionType selection type.
        */
        void HandleSelection( const TGSSelectionTypes aSelectionType );
	
	    /**
        * Opens the Device Manager application
        * @param None, @return None
        */
        void OpenDmAppL();

	private:
	  
		void ConstructL();
		
		/**
		* Constructor
		*/
	    COmaDmCPPlugin();
	    
	    /**
        * From CAknView.
        * This implementation is empty because this class, being just a dialog,
        * does not implement the CAknView finctionality.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView.
        * This implementation is empty because this class, being just a dialog,
        * does not implement the CAknView finctionality.
        */
        void DoDeactivate();
        
        /**
        * Closes DmUi   
        * This closes Dm Ui when CAknNullService is destroyed.
        * 
        */
        void CloseDmUi();
		CEikonEnv* env;
		TInt       iResId;
		
		/**
        * AVKON NULL Service.
        * Own.
        */
        CAknNullService* iNullService;
	
};

#endif // DM_CP_PLUGIN_H
//End of file
