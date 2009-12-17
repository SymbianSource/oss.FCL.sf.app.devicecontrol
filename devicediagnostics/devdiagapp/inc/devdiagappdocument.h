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
* Description:  Document of the application.
*
*/

#ifndef DEVDIAGAPP_DOCUMENT_H
#define DEVDIAGAPP_DOCUMENT_H


#include <AknDoc.h>

// Forward references
class CDevDiagAppAppUi;
class CEikApplication;
class CDevDiagEngine;

/**
* An instance of class CDevDiagAppDocument is the Document part of the AVKON
* application framework for the DevDiagApp.
*/
class CDevDiagAppDocument : public CAknDocument
    {
public:

    /**
    * Construct a CDevDiagAppDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    *
    * @param aApp application creating this document
    * @return a pointer to the created instance of CDevDiagAppDocument.
    **/
    static CDevDiagAppDocument* NewL(CEikApplication& aApp);

    /**
    * Construct a CDevDiagAppDocument for the AVKON application aApp 
    * using two phase construction, and return a pointer to the created object
    *
    * @param aApp application creating this document
    * @return a pointer to the created instance of CDevDiagAppDocument
    **/
    static CDevDiagAppDocument* NewLC(CEikApplication& aApp);

    /**
    * Destroy the object and release all memory objects
    **/
    ~CDevDiagAppDocument();

public: // from CAknDocument

    /**
    * Create a CDevDiagAppAppUi object and return a pointer to it
    * @return a pointer to the created instance of the AppUi created
    **/
    CEikAppUi* CreateAppUiL();

public:
    
    /**
    * Return the Engine.
    * @return DevDiagEngine.
    **/
    CDevDiagEngine& Engine();

private:

    /**
    * Perform the second phase construction of a CDevDiagAppDocument object
    **/
    void ConstructL();

    /**
    * Perform the first phase of two phase construction 
    *
    * @param aApp application creating this document
    **/
    CDevDiagAppDocument(CEikApplication& aApp);
    
private:

    // Owns.
    CDevDiagEngine* iEngine;    

    };


#endif // DEVDIAGAPP_DOCUMENT_H
