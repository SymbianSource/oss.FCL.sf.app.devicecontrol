/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*        class definition for CUISettingsSrvUi
*
*/


#ifndef __UISettingsSrvUi_H__
#define __UISettingsSrvUi_H__

// INCLUDES
#include <aknViewAppUi.h>
#include <aknsettingpage.h>
#include <AknTabObserver.h>
//#include <scshortcut.hrh>
//#include <scuiobserver.h>
#include <ecom/ecom.h>
#include <MMGFetchVerifier.h>


// CLASS DECLARATION
class CAknWaitDialog;
class CAknNavigationControlContainer;
class CAknTabGroup;
class CAknNavigationDecorator;
class CAknQueryDialog;
//
class CScShortcutUi;

class CUISettingsSrvAppView;

//sf-
/*sf--
//For embedding CC in CS
class TAppInfo
	{
public:
	TAppInfo(TUid aUid, const TDesC& aFile)
		: iUid(aUid), iFile(aFile)
		{}
	TUid iUid;
	TFileName iFile;
	};
--fs*/

/**
*  'AppUi' class.
*  UI class for UISettingsSrv application
*/
class CUISettingsSrvUi : public CAknAppUi //sf-CAknViewAppUi
//sf-, public MAknSettingPageObserver,
//sf-              public MAknTabObserver, public MMGFetchVerifier,
//sf-              public MScUiObserver, public MGSViewOwner
    {
    public: // Constructors and destructor
    
        /**
        * C++ default constructor
        */
        CUISettingsSrvUi();

        /**
        * Symbian OS default constructor.
        * 
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CUISettingsSrvUi();

    public: // from CEikAppUi
        /**
        * Takes care of command handling.
        * @param aCommand command to be handled
        * 
        */
        void HandleCommandL(TInt aCommand);

    public: // new

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private: // from CEikAppUi
        /**
        * handles key events
        * @param aKeyEvent 
        * @param aType type of the key event 
        * @return TKeyResponse
        */
        virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,
                                             TEventCode aType);


    private: // new
    	CUISettingsSrvAppView* iAppView;

    protected: // data

    public:
        CAknTabGroup* iTabGroup;   //tab group handle

    protected:

    protected:
        CApaDocument* iEmbedded;

    private: // For AI Settings View integration
//sf-        CGSAIMyTodaySettingsViewInterface* iAISettingsViewInterface;
    };

#endif // __UISettingsSrvUi_H__
