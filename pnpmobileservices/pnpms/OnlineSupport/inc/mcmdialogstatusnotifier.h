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




#ifndef MCMDIALOGSTATUSNOTIFIER_H
#define MCMDIALOGSTATUSNOTIFIER_H

//  INCLUDES
#include <e32base.h>
//#include <etelmm.h>
#include "ccmdialogmonitor.h"

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
*  MCMDialogStatusNotifier.
*
*  @since 2.0
*/
class MCMDialogStatusNotifier
    {
    public:  // Constructors and destructor
        
    public: // New functions
        /**
        * @param aDialog
        */
        virtual void CommonDialogDismissedL( const CCMDialogMonitor::TDialogMode aDialog, const TInt aStatusCode ) = 0;

        /**
        * @param aStatus 
        * @param aError
        */
        //virtual void RegistrationReportErrorL( const CCMRegistrationMonitor::TMonitoringStatus aStatus, const TInt aError ) = 0;
    };

#endif      // MCMREGISTERATIONSTATUSNOTIFIER_H  
            
// End of File
