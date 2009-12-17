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
* Description:  Application class
*
*/



#ifndef DEVDIAGAPP_APPLICATION_H
#define DEVDIAGAPP_APPLICATION_H

#include <aknapp.h>

 /** 
 * @class CDevDiagAppApplication
 * @since 5.0 
 **/
class CDevDiagAppApplication : public CAknApplication
    {
public:  // from CAknApplication

    /**
    * Returns the application DLL UID value
    *
    * @result the UID of this Application/Dll
    **/
    TUid AppDllUid() const;

protected: // from CAknApplication

    /**     
    * Create a CApaDocument object and return a pointer to it
    *
    * @return a pointer to the created document
    **/
    CApaDocument* CreateDocumentL();
    
    };

#endif // DEVDIAGAPP_APPLICATION_H
