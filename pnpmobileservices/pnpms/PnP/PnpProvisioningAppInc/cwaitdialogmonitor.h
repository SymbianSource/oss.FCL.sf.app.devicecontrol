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



#ifndef CWAITDIALOGMONITOR_H
#define CWAITDIALOGMONITOR_H

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

class MWaitDialogStatusNotifier;

// CLASS DECLARATION

/**
*  CWaitDialogMonitor.
*  Monitors the dialog status
*
*  @lib Confmanager.app
*  @since 2.0
*/
class CWaitDialogMonitor : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWaitDialogMonitor* NewL( MWaitDialogStatusNotifier& aNotifier );
        
        /**
        * Destructor.
        */
        ~CWaitDialogMonitor();

    public: // New functions
            

        /**
        * StartMonitoring.
        * @since 2.0
        * @return void
        */
        void Start();

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
        CWaitDialogMonitor( MWaitDialogStatusNotifier& aNotifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    public:     // Data

    protected:  // Data        

    private:    // Data
        MWaitDialogStatusNotifier& iNotifier;

    };

#endif      // CWaitDialogMonitor_H   
            
// End of File
