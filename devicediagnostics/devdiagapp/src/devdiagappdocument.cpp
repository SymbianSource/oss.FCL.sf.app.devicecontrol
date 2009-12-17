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



#include "devdiagappappui.h"
#include "devdiagappdocument.h"
#include "devdiagengine.h"

// Standard Symbian OS construction sequence
CDevDiagAppDocument* CDevDiagAppDocument::NewL(CEikApplication& aApp)
    {
    //LOGSTRING("CDevDiagAppDocument* CDevDiagAppDocument::NewL");
    CDevDiagAppDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CDevDiagAppDocument* CDevDiagAppDocument::NewLC(CEikApplication& aApp)
    {
    //LOGSTRING("CDevDiagAppDocument* CDevDiagAppDocument::NewLC");
    CDevDiagAppDocument* self = new (ELeave) CDevDiagAppDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDevDiagAppDocument::ConstructL()
    {
	iEngine = CDevDiagEngine::NewL();
    }    

CDevDiagAppDocument::CDevDiagAppDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    }

CDevDiagAppDocument::~CDevDiagAppDocument()
    {
	if ( iEngine )    
        {
        delete iEngine;
        iEngine = NULL;
        } 
    }

CEikAppUi* CDevDiagAppDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    //LOGSTRING("CDevDiagAppDocument::CreateAppUiL");
    CEikAppUi* appUi = new (ELeave) CDevDiagAppAppUi;
    return appUi;
    }


CDevDiagEngine& CDevDiagAppDocument::Engine()
    {
    //LOGSTRING("CDevDiagEngine& CDevDiagAppDocument::Engine");
    return *iEngine;
    }
///@@@KSR: changes for BAD Warnings - Added a new Line.
