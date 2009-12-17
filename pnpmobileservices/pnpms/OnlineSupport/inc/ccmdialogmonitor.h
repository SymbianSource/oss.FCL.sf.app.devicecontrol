/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for dialog monitor
*
*/



#ifndef CCMDIALOGMONITOR_H
#define CCMDIALOGMONITOR_H

//  INCLUDES
#include <e32base.h>
//#include <etelmm.h>

// CONSTANTS
//None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
//class MCMRegisterationStatusNotifier;
class MCMDialogStatusNotifier;

// CLASS DECLARATION

/**
*  CCMDialogMonitor.
*  Monitors the dialog status
*
*  @lib Confmanager.app
*  @since 2.0
*/
class CCMDialogMonitor : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Registeration status.
        */
        enum TDialogMode
            {
            EConfirmation = 0,
            EWait,
            EError,
            EUnknown
            };

        void SetDialogMode(TDialogMode aMode);
        /**
        * Two-phased constructor.
        */
        static CCMDialogMonitor* NewL( MCMDialogStatusNotifier& aNotifier );
        
        /**
        * Destructor.
        */
        ~CCMDialogMonitor();

    public: // New functions
            

        /**
        * StartMonitoring.
        * @since 2.0
        * @return void
        */
        //void StartMonitoring( );
        void Start();
        
        /**
        * StopMonitoring.
        * @since 2.0
        * @return void
        */
        //void StopMonitoring( );

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private: // Functions from base classes
        
        /**
        * From CActive Handles an active object’s request completion event.
        * @since 2.0
        * @return void
        */
        void RunL();
        
        /**
        * From CActive Cancels any outstanding request.
        * @since 2.0
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles a leave occurring in the request completion event handler RunL().
        */
        //TInt RunError( TInt aError );
    private:

        /**
        * C++ default constructor.
        */
        CCMDialogMonitor( MCMDialogStatusNotifier& aNotifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    public:     // Data

    protected:  // Data        

    private:    // Data
        TDialogMode iDialogMode;
        MCMDialogStatusNotifier& iNotifier;

    };

#endif      // CCMDIALOGMONITOR_H   
            
// End of File
