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
*           Document class required by Symbian OS architecture.
*
*/


#ifndef __UISettingsSrvDocument_H__
#define __UISettingsSrvDocument_H__

// INCLUDES
#include <AknDoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;
class CVpnSettingsUi;

//sf-class CUISettingsSrvDocument;

// CLASS DECLARATION

/**
*  CGSDocument application class.
*/
class CUISettingsSrvDocument : public CAknDocument
{
    public: // constructor and destructor
        /**
        * Two-phased constructor
        * @param aApp Application.
        * @return GS document.
        */
        static CUISettingsSrvDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CUISettingsSrvDocument();

    private:

        /**
        * C++ constructor
        */
        CUISettingsSrvDocument(CEikApplication& aApp) : CAknDocument(aApp) { }

        /**
        * Symbian OS default constructor.
        * 
        */
        void ConstructL();

    private: // from CEikDocument

		void UpdateTaskNameL(CApaWindowGroupName* aWgName);

        /**
        * Create CGSUi "App UI" object.
        * @return Eikon application UI.
        */
        CEikAppUi* CreateAppUiL();

    private: // data

    public:
        CVpnSettingsUi* iVpnSettingsUi;
};

#endif // __UISettingsSrvDocument_H__

