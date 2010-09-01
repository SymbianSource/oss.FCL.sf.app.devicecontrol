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
* Description:  Header file for CNSmlDMSyncApp
*
*/


#ifndef NSMLDMSYNCAPP_H
#define NSMLDMSYNCAPP_H

// INCLUDES
#include <aknapp.h>
#include <AknServerApp.h>

const TUid KUidSmlSyncApp = { 0x101F6DE5 };

// CLASS DECLARATION

/**
* Device Management Application
*
*/
NONSHARABLE_CLASS (CNSmlDMSyncApp) : public CAknApplication
    {
    public:
        /**
        * From CApaApplication, creates CNSmlDMSyncDocument document object.
        * @param None
		* @return A pointer to the created document object.
		*/
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidSmlSyncApp).
        * @param None
		* @return The value of KUidSmlSyncApp.
        */
        TUid AppDllUid() const;
        
        /**
        * From CApaApplication, Creates new application server
        * @param aAppServer App server
		* @return None
        */
        void NewAppServerL( CApaAppServer*& aAppServer );
    };

#endif  // NSMLDMSYNCAPP_H

// End of File
