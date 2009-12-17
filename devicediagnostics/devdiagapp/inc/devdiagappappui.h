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
* Description:  Application view UI class.
*
*/



#ifndef DEVDIAGAPP_APPUI_H
#define DEVDIAGAPP_APPUI_H

#include <aknViewAppUi.h> 

// Forward reference
class CDevDiagSuiteView;

/** 
* An instance of class CDevDiagAppAppUi is the UserInterface part of the AVKON
* application framework for the DevDiagApp application
* @since 5.0
**/
class CDevDiagAppAppUi : public CAknViewAppUi
    {
public:

    /**  
    * Perform the second phase construction of a CDevDiagAppAppUi object
    * this needs to be public due to the way the framework constructs the AppUi 
    **/
    void ConstructL();

    /**
    * Perform the first phase of two phase construction.
    * This needs to be public due to the way the framework constructs the AppUi 
    **/
    CDevDiagAppAppUi();


    /**
    * Destroy the object and release all memory objects.
    **/
    ~CDevDiagAppAppUi();

public: // from CAknAppUi

    /**
    * Handle user menu selections.
    *
    * @param aCommand the enumerated code for the option selected.
    **/
    void HandleCommandL(TInt aCommand);

private:

    // The application view. Own
    CDevDiagSuiteView* iSuiteView;
    };


#endif // DEVDIAGAPP_APPUI_H

