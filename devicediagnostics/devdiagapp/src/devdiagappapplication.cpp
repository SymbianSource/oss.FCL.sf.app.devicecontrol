/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of diagnostics components
*
*/


#include "devdiagappdocument.h"
#include "devdiagappapplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidDevDiagAppApp = {0x2000E542};

CApaDocument* CDevDiagAppApplication::CreateDocumentL()
    {  
   // LOGSTRING(CDevDiagAppApplication::CreateDocumentL);
    // Create an DevDiagApp document, and return a pointer to it
    CApaDocument* document = CDevDiagAppDocument::NewL(*this);
    return document;
    }

TUid CDevDiagAppApplication::AppDllUid() const
    {
    // Return the UID for the DevDiagApp application
    //LOGSTRING("CDevDiagAppApplication::AppDllUid");
    return KUidDevDiagAppApp;
    }

