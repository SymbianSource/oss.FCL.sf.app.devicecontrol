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


#ifndef __AppMgmtSRVAPP_H_
#define __AppMgmtSRVAPP_H_

// INCLUDES
#include <eikapp.h>
#include <aknapp.h>

#include "AppMgmtSrvUI.h"

#include "ApplicationManagementServer.h"
using namespace NApplicationManagement;

//class CApplicationManagementServer;

// CLASS DECLARATION

/**
 * AppMgmt server application
 *
 *  @lib    AppMgmtserver
 *  @since  S60 v3.1
 */
class CAppMgmtSrvApp : public CAknApplication
    {

public:
    NApplicationManagement::CApplicationManagementServer* Server();

    CAppMgmtSrvApp();

    virtual ~CAppMgmtSrvApp();

    /**
     * Show or hide UI
     *
     * @since   S60   v3.1
     * @param   aVisible    Tells whether to hide / show the ui
     * @return  none
     */
    void SetUIVisibleL();

private:
    /**
     * From CApaApplication, creates AppMgmtSrvDocument document object.
     * @since   S60   v3.1
     * @param    None    
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();

    /**
     * From CApaApplication, returns application's UID (KUidAppMgmtServer).
     * @since   S60   v3.1
     * @param    None
     * @return   The value of KUidAppMgmtServer.
     */
    TUid AppDllUid() const;

    /**
     * From CApaApplication
     * @since   S60   v3.1
     * @param   aAppServer returned server
     * @return  None
     */
    void NewAppServerL(CApaAppServer*& aAppServer);

public:

    /**
     * AppMgmt server.
     * Own. 
     */
    CApplicationManagementServer* iAppMgmtServer;

    /**
     * Document
     * Own. 
     */
    CAppMgmtSrvDocument* iDocument;

    };

#endif

// End of File
