/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for Application class.
*
*/




#ifndef CCMAPPLICATION_H
#define CCMAPPLICATION_H

//  INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidConfigurationManager = { 0x10204338 }; // UID of the application


// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

/**
*  Application class.
*  Provides factory to create concrete document object.
*
*  @lib ConfManager.app
*  @since 2.0
*/

class CCMApplication : public CAknApplication
    {
    private: // Functions from base classes
        
        /**
        * From CApaApplication Creates COCDocument document object.
        * @since 2.0
        * @return pointer to created document.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication Returns application's UID.
        * @since 2.0
        * @return KUidConfigurationManager
        */
        TUid AppDllUid() const;
    };

#endif // CCMAPPLICATION_H

// End of File

