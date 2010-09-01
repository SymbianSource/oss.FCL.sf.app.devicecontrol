/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Implementation of applicationmanagement components
 *
*/


#ifndef CAppMgmtSRVDOCUMENT_H
#define CAppMgmtSRVDOCUMENT_H

// INCLUDES
#include <aknapp.h>
#include <AknDoc.h>
#include <eikenv.h>

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikEnv;
class CNSmlDMSyncProfileList;

// CLASS DECLARATION

/**
 *  CAppMgmtSrvDocument application class.
 *
 *  @lib    AppMgmtserver
 *  @since  S60 v3.1
 */
class CAppMgmtSrvDocument : public CAknDocument
    {
public:

    /**
     * Two-phased constructor.
     */
    static CAppMgmtSrvDocument* NewL(CAknApplication& aApp);

    /**
     * Destructor.
     */
    virtual ~CAppMgmtSrvDocument();

public:
    // New functions

    /**
     * Getter fo CEikAppUi class
     * @param    None
     * @return   Pointer to the CEikAppUi
     */
    CEikAppUi* AppUi() const
        {
        return iAppUi;
        }

private:

    /**
     * Constructor
     * @param    aApp Used by akndocument
     * @return   None
     */
    CAppMgmtSrvDocument(CEikApplication& aApp) :
        CAknDocument(aApp)
        {
        }

    void ConstructL();

    /**
     * From CEikDocument, create CAppMgmtSrvUi "App UI" object.
     * @param    None
     * @return   UI
     */
    CEikAppUi* CreateAppUiL();

private:

    /**
     * Reference to eikonenv
     * 
     */
    //CEikonEnv*                          iEikEnv;
    };

#endif

// End of File
