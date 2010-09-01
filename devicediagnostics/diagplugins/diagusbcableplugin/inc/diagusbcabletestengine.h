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
* Description:  This is the header for the Diagnostics USB Cable Test Engine
*
*/


#ifndef DIAGUSBCABLETESTENGINE_H 
#define DIAGUSBCABLETESTENGINE_H


// INCLUDES
#include <DiagResultsDatabaseItem.h>                // CDiagResultsDatabaseItem
#include <usbman.h>                                 // RUsb

#include "diagusbcabletestcallbackinterface.h"      // MDiagUSBTestCallBackInterface

//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagPluginWaitingDialogWrapper;
class CRepository;

// CLASS DEFINITION

/**
* This is the engine class of the USB cable test plugin.
*/
class CDiagUSBCableTestEngine : public CActive
    {
public: //public constructor and destructor.

    /**
    * Symbian two phase constructor
    */
    static CDiagUSBCableTestEngine* NewL(MDiagUSBTestCallBackInterface& aInterface,
                                        TBool aIsSinglePluginExecution);
    
    /**
    * Destructor
    */
    ~CDiagUSBCableTestEngine();
    
public: // public interfaces

    /**
    * The plugin class starts the USB cable test using this function
    */
    void RunUSBCableTestL();

private: // private constructors
    
    /**
    * C++ constructor
    */
    CDiagUSBCableTestEngine(MDiagUSBTestCallBackInterface& aInterface,
                            TBool aIsSinglePluginExecution);
    
    /**
    * Symbian second phase constructor.
    */
    void ConstructL();

private: // private internal functions

    /**
    * This is an internal function to end the test execution and report result to the 
    * plug-in class.
    */
    void ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult);
    
    /**
    * This function detects the USB cable status.
    */
    TBool CheckIfUSBCableDetectedL();

    /**
    * Ask user to connect charger and press OK
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue - If dialog was dismissed by end user.
    *       EFalse - If dialog was deleted.
    */
    TBool ConnectAndPressOkL( TInt& aUserResponse ); 

    /**
    * Ask user to remove all cables and press OK    
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue  - If dialog was dismissed by end user.
    *           EFalse - If dialog was deleted.
    */
    TBool RemoveAllCablesL( TInt& aUserResponse ); 
    
    /**
    * Display "No cable detected. Try again?" prompt
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue - If dialog was dismissed by end user.
    *       EFalse - If dialog was deleted.
    */
    TBool AskIfRetryL( TInt& aUserResponse );

    /**
    *   Displays a query dialog with header
    *   @param aDialogResourceId - Resource ID of the dialog to display
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue - If dialog was dismissed by end user.
    *           EFalse - If dialog was deleted.
    */
    TBool DisplayQueryDialogWithHeaderL( TInt aDialogResourceId, 
                               TInt& aUserResponse );
    
protected:    // from CActive
        /**
        * Handle active object completion event, used to handle internal trigger in our plug-in
        */
        void RunL();
        
        /**
        * Handle RunL leaves
        */
        TInt RunError( TInt aError );
    
        /**
        * Handle cancel
        */
        void DoCancel();
    
        
private: // Data

    /**
    * Reference to the callback interface between the engine class and plugin class
    */
    MDiagUSBTestCallBackInterface& iInterface;
    
    /**
    * Wrapper to display waiting dialog. This helps to identify if the dialog
    * was dismissed by end-user or by object deletion.
    * Ownership: This class.
    */
    CDiagPluginWaitingDialogWrapper* iWaitingDialogWrapper;

    /**
    * Instance of the session to the USB manager.
    */
    RUsb iUsbManager;
    
    /**
    * Instance of the central repository handle. Own
    */
    CRepository  *iCenRep;
    
    /**
    * Boolean flag to remember if the cenrep is modified by the test plugin.
    */
    TBool iCenrepModified;
    
    /**
    *   This flag indicates if the plugin is run in single test execution mode.
    */
    TBool iIsSinglePluginExecution;    
    
    };

#endif //DIAGUSBCABLETESTENGINE_H

// End of File
