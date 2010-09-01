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
* Description:  Application UI class implementation.
*
*/


#include <avkon.hrh>
#include "devdiag.pan"
#include "devdiagappappui.h"
#include "devdiagsuiteview.h"
#include "devdiagapp.hrh"
#include "devdiagappdocument.h"

// ---------------------------------------------------------------------------
// ConstructL is called by the application framework
// ---------------------------------------------------------------------------
void CDevDiagAppAppUi::ConstructL()
    {
    //LOGSTRING("CDevDiagAppAppUi::ConstructL");
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    
    // Get the document so we can access the application engine.
    CDevDiagAppDocument* document = static_cast< CDevDiagAppDocument* > 
                                                    ( Document() );
    //Plugin view is created by the suite view.
    iSuiteView = CDevDiagSuiteView::NewL( document->Engine() );  
          
    AddViewL( iSuiteView );
    
    SetDefaultViewL( *iSuiteView );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
CDevDiagAppAppUi::CDevDiagAppAppUi()                              
    {

    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CDevDiagAppAppUi::~CDevDiagAppAppUi()
    {

    }

// ---------------------------------------------------------------------------
// handle any menu commands.
// ---------------------------------------------------------------------------

void CDevDiagAppAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EAknSoftkeyBack:
           {
           Exit();     

           break;
           }
        
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        default:
            Panic(EDevDiagApplicationUnhandledCommand);
            break;
        }
    }



