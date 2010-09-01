/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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




#ifndef MWAITDIALOGSTATUSNOTIFIER_H
#define MWAITDIALOGSTATUSNOTIFIER_H

//  INCLUDES
#include <e32base.h>
//#include <etelmm.h>
//#include "cwaitdialogmonitor.h"

// CONSTANTS
// None

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
*  MWaitDialogStatusNotifier.
*
*  @since 3.1
*/
class MWaitDialogStatusNotifier
    {
    public:  // Constructors and destructor
        
    public: // New functions
        /**
        * @param aDialog
        */
        virtual void WaitDialogDismissedL( const TInt aStatusCode ) = 0;
    };

#endif      // MWAITDIALOGSTATUSNOTIFIER_H  
            
// End of File
