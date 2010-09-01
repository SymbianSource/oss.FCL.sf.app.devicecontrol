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
* Description:  Contains the implementation of CCMDocument class methods
*
*/



// INCLUDE FILES
#include "CCMDocument.h"
#include "CCMAppUi.h"
/*** Begin added by Jarmo Pyrro ***/
#include "OnlineSupportLogger.h"
#include <bacline.h>
/*** End added by Jarmo Pyrro ***/

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

// ============================= LOCAL FUNCTIONS ===============================
// None

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCMDocument::CCMDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCMDocument::CCMDocument(
        CEikApplication& aApp  // CEikApplication reference
        ) : CAknDocument(aApp)
    {
    }

// destructor
CCMDocument::~CCMDocument()
    {
    }
    
// -----------------------------------------------------------------------------
// CCMDocument::CreateAppUiL
// Creates CCMAppUi object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEikAppUi* CCMDocument::CreateAppUiL()
    {
    return new (ELeave) CCMAppUi;
    }



// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

// End of File  
