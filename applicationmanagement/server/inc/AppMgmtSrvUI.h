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

#ifndef CAppMgmtSRVUI_H
#define CAppMgmtSRVUI_H

#include <bldvariant.hrh>

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <eikdialg.h>
#include <eikmenup.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknnavilabel.h>
#include <akntitle.h>
#include <AknProgressDialog.h>

#include "AppMgmtSrvDocument.h"
#include <aknViewAppUi.h>

// FORWARD DECLARATIONS

/**
 *  Application UI class 
 *
 *  @lib    AppMgmtserver
 *  @since  S60 v3.1
 */
class CAppMgmtSrvUi : public CAknAppUi
    {
public:
    // Constructors and destructor

    void ConstructL();

    virtual ~CAppMgmtSrvUi();

    void HandleForegroundEventL(TBool aValue);

private:
    // from base classes


    /**
     * @param aResourceId        
     * @param aMenuPane
     * @return none
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    /**
     * From CEikAppUi, takes care of command handling.
     * @param aCommand id of the  command to be handled
     * @return None 
     */
    void HandleCommandL(TInt aCommand);

    /**
     * From CEikAppUi, handles key events.
     * @param aKeyEvent  Event to handled.
     * @param aType   Type of the key event. 
     * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed). 
     */
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,
            TEventCode aType);

    /**
     * From CAknAppUi, handles file change.
     * @param aFileName New file to be opened.
     * @return None
     */
    void OpenFileL(const TDesC& aFileName);

    /**
     * From CAknAppUi, Preparing to close server
     * @param None
     * @return None
     */
    void PrepareToExit();

public:

private:
    //Data

    /**
     * Should AppMgmt operation close on an system event (ex. MMC card removal)?
     */
    TBool iNeedToClose;

    };

#endif

// End of File
