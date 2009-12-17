/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file CMiniBrowser class
*
*/



#ifndef MINI_BROWSER_OBSERVER_H
#define MINI_BROWSER_OBSERVER_H

//  INCLUDES
// None

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

/**
*  CMiniBrowser class
*
*  @since 3.1?
*/
class MMiniBrowserObserver
    {
    public:
        /**
        Called after MiniBrowser has handled a request for an URL
        (a call to HttpGetL resulted a successful http request-response)
        */
        virtual void MiniBrowserRequestDoneL() = 0;

        /**
        Called if RunL of CMiniBrowser leaves.
        @param aError The leave code.
        */
        virtual void MiniBrowserErrorL( const TInt aError ) = 0;

        /**
        Called if a call to MiniBrowserRequestDoneL or MiniBrowserErrorL leaves
        @param aError The leave code
        */
        virtual void MiniBrowserUnhandledLeave( const TInt aError ) = 0;
    };

#endif  // MINI_BROWSER_OBSERVER_H

// End of File
