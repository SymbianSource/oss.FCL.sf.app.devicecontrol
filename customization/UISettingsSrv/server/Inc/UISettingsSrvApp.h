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
*     UISettingsSrvApp is an application class required by Symbian OS architecture.
*
*/


#ifndef __UISettingsSrvApp_H__
#define __UISettingsSrvApp_H__

// INCLUDES
#include <aknapp.h>
#include <e32std.h>


// CONSTANTS
// UID of app
const   TUid    KUidUISettingsSrv = { 0x10207839 };


// CLASS DECLARATION

class CUISettingsSrvServer;


/**
*  CGSApp application class.
*/
class CUISettingsSrvApp : public CAknApplication
{
	public:
		CUISettingsSrvApp( CUISettingsSrvServer* aServer );
		virtual ~CUISettingsSrvApp();

    private: // from CApaApplication

        /**
        * Create CUISettingsSrvDocument document object.
        * 
        */
        CApaDocument* CreateDocumentL();

        /**
        * Returns application UID.
        * @return KUidGS.
        */
        TUid AppDllUid() const;
        
	private:
		CUISettingsSrvServer* iServer;
};


#endif // __UISettingsSrvApp_H__
