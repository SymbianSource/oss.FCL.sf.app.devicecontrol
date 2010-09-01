/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of PnPMS components
*
*/


#ifndef PNPPROVISIONINGAPPUI_H
#define PNPPROVISIONINGAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include "PnpProvisioningDocument.h"
#include "mwaitdialogstatusnotifier.h"  // for MWaitDialogStatusNotifier
#include <PnpProvUtil.h>
#include "PnpServiceActivation.h"


// FORWARD DECLARATIONS
//class CPnpProvisioningContainer;
class CWaitDialogMonitor;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CPnpProvisioningAppUi : 
    public CAknAppUi,
    public MCoeForegroundObserver,
    private MWaitDialogStatusNotifier
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CPnpProvisioningAppUi();
        
    public: // New functions

        /**
        * From Exits app.
        */
        void DoExit();
        void RedirectL( const TDesC& aUrl, THttpProvStates::TProvisioningStatus aStatus );
        static TInt PeriodicCallback( TAny* aPtr );

    public: // from MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();
        
    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    
    public: // from MCMDialogStatusNotifier
        void WaitDialogDismissedL( const TInt aStatusCode );

    public:
	TBool ServiceActivationEnabledL();
        
    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);
            
        void ShowWaitNoteL();
        
        void TerminateBrowserL(TInt aError, TInt& aNoRedirect);

    private: //Data
        void DoPeriodicCallbackL( TAny* aPtr );
        /**
        Tries to read AP id from a connection (that is currently in use)
        that RConnectionMonitor::GetConnectionInfo gives first.
        */
        void ResolveApL( TUint32& aAp );
        
        void ShowErrorNoteL();

        //CPnpProvisioningContainer* iAppContainer; 
        CPeriodic* iPeriodic;
        TUint32 iApInUse;
        
         // wait note dialog monitor
        CWaitDialogMonitor* iWaitDialogMonitor;
        // wait note dialog
        CAknGlobalNote* iGlobalWaitNote;
    };

#endif

// End of File
