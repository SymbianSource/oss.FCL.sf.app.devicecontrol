/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface between the sockets engine and UI
*
*/




#ifndef MCMSOCKETSENGINENOTIFER_H
#define MCMSOCKETSENGINENOTIFER_H

//  INCLUDES
#include <e32base.h>

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
*  MCMSocketsEngineNotifier
*  Notifier class 
*
*  @lib Configuration Manager application
*  @since 2.0
*/
class MCMSocketsEngineNotifier
    {
    public:  // Constructors and destructor
             
    public: // New functions
        
        virtual void ConnectionResultL(TInt aErrorCode ) = 0;

        virtual void UnhandledLeave( TInt aErrorCode ) = 0;
        
    public: // Functions from base classes
    
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
        
    protected:  // Data
        
    private:    // Data
        
    public:     // Friend classes
       
    protected:  // Friend classes
         
    private:    // Friend classes
         

    };

#endif      // MCMSOCKETSENGINENOTIFER_H   
            
// End of File
