/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


#ifndef PNPPROVISIONINGAPP_H
#define PNPPROVISIONINGAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidPnpProvisioning = { 0x10204339 };

// CLASS DECLARATION

/**
* CPnpProvisioningApp application class.
* Provides factory to create concrete document object.
* 
*/
class CPnpProvisioningApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CPnpProvisioningDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidPnpProvisioning).
        * @return The value of KUidPnpProvisioning.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

