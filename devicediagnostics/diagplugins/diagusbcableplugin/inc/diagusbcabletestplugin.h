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
* Description:  This is the header for the Diagnostics USB Cable Test Plug-in
*
*/




#ifndef DIAGUSBCABLETESTPLUGIN_H 
#define DIAGUSBCABLETESTPLUGIN_H

// INCLUDES

// System includes
#include <DiagResultsDatabaseItem.h>            // CDiagResultsDatabaseItem
#include <DiagTestPluginBase.h>                 // CDiagTestPluginBase

// User includes
#include "diagusbcabletestcallbackinterface.h"  // MDiagUSBTestCallBackInterface


//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagResultsDbItemBuilder;
class CDiagUSBCableTestEngine;

// CLASS DEFINITION
class CDiagUSBCableTestPlugin : public CDiagTestPluginBase, 
                                public MDiagUSBTestCallBackInterface
    {
    
public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return USB CableTest Test Plugin
        */
        static MDiagPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor
        */
        ~CDiagUSBCableTestPlugin();

private: // from MDiagUSBTestCallBackInterface

        /**
        *  The USB Cable Test Engine calls this callback to provide the result of the test
        */
        void ReportTestResultL(CDiagResultsDatabaseItem::TResult aResult);
        
        /**
        * The engine class uses this callback on the plugin class to ask the user if the entire test
        * execution needs to be cancelled.
        * @param aUserResponse : The user response is returned in this reference argument.
        *      
        * @return ETrue     : The dialog was dismissed by the user.
        *         EFalse    : The dialog was dismissed internally.
        */    
        TBool AskCancelExecutionL(TInt& aUserResponse, TInt aOption);  

private: // From CDiagTestPluginBase

        /**
        * @see CDiagTestPluginBase::Visible()
        */
        TBool IsVisible() const;

        /**
        * @see CDiagTestPluginBase::RunMode()
        */
        TRunMode RunMode() const;

        /**
        * @see CDiagTestPluginBase::TotalSteps()
        */
        TUint TotalSteps() const;

        /**
        * @see CDiagTestPluginBase::GetPluginNameL
        */
        HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;


        /**
        * @see CDiagTestPluginBase::Uid
        */
        TUid Uid() const;
        
        /**
        * Handle plug-in execution.
        * @see CDiagTestPluginBase::DoRunTestL
        */
        void DoRunTestL();

        /**
        * Clean up
        * @see CDiagTestPluginBase::DoStopAndCleanupL
        */
        void DoStopAndCleanupL();

private:    // private constructors

        /**
        * C++ default constructor.
        */
        CDiagUSBCableTestPlugin( CDiagPluginConstructionParam* aInitParams );

        /**
        * Symbian OS default constructor.
        *
        */
        void ConstructL();



private:    // from CActive
        /**
        * Handle active object completion event
        */
        void RunL();

        /**
        * Handle cancel
        */
        void DoCancel();

    private:    // private data
        
        /**
        * An instance of the engine class. Own
        */
        CDiagUSBCableTestEngine *iEngine;
    };

#endif //DIAGUSBCABLETESTPLUGIN_H

// End of File

