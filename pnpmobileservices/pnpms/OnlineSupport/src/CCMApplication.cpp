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
* Description:  Source file for class CCMApplication
*
*/



// INCLUDE FILES
#include "CCMApplication.h"
#include "CCMDocument.h"
#include "OnlineSupportLogger.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None


// ============================= LOCAL FUNCTIONS ===============================
// None


// ============================ MEMBER FUNCTIONS ===============================

    

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CCMApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// -----------------------------------------------------------------------------
// CCMApplication::CreateDocumentL
// Creates CCMDocument object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CApaDocument* CCMApplication::CreateDocumentL()
    {
    CCMDocument* self = new CCMDocument( *this );
    return self; 
    }


// -----------------------------------------------------------------------------
// CCMApplication::AppDllUid()
// Returns application UID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUid CCMApplication::AppDllUid() const
    {
    return KUidConfigurationManager;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File  

