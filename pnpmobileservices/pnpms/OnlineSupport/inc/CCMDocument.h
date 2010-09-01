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
* Description:  This file contains the header file of the CCMDocument object.
*
*/



#ifndef CCMDOCUMENT_H
#define CCMDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class  CEikAppUi;


// CLASS DECLARATION

/**
*  CCMDocument application class.
*
*  @lib ConfManager.app
*  @since 2.0
*/

class CCMDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        CCMDocument( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CCMDocument();

    private: // Functions from base classes

        /**
        * From CEikDocument Creates CCMAppUi object.
        * @since 2.0
        * @return Pointer to CEikAppUi object
        */
        CEikAppUi* CreateAppUiL();
        
   };

#endif  // CCMDOCUMENT_H

// End of File

