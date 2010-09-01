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
* Description:  This is the header for the Diagnostics Battery Charger 
*                 Cable Test Engine
*
*/


#ifndef DIAGBATTERYCHARGERCABLETESTENGINE_H
#define DIAGBATTERYCHARGERCABLETESTENGINE_H


// INCLUDES

#include <DiagResultsDatabaseItem.h>

//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagPluginWaitingDialogWrapper;
class MDiagBatteryChargerTestCallBackInterface;


// CLASS DEFINITION
/**
* CDiagBatteryChargerCableTestEngine
*
* Engine class of the Battery charger cable test plug-in
*/    
class CDiagBatteryChargerCableTestEngine : public CActive
    {
    
public: //New functions
    static CDiagBatteryChargerCableTestEngine* NewL(
        MDiagBatteryChargerTestCallBackInterface& aInterface,
        TBool aIsSinglePluginExecution);
    
    /**
    * C++ destructor
    */
    ~CDiagBatteryChargerCableTestEngine();
    
public: // external functions
    /**
    * The plug-in class uses this function to start the Battery charger test
    */
    void RunBatteryChargerTestL();

private:

    /**
    * Standard c++ constructor
    */
    CDiagBatteryChargerCableTestEngine(MDiagBatteryChargerTestCallBackInterface& aInterface,
                                       TBool aIsSinglePluginExecution);
    
    /**
    * Symbian second phase constructor
    */
    void ConstructL();
    
    
private: //internal functions
    
    /**
    * To report the result to the plug-in class
    */
    void ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult);
    
    /**
    *   Detect the Battery charger status
    *   @param none
    *   @return ETrue - If the charger was detected
    *           EFalse - If the charger was not detected
    */    
    TBool CheckIfChargerDetected();


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
    * Displays a query dialog with header.
    *   @param aDialogResourceId - Resource ID of the dialog to display
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue - If dialog was dismissed by end user.
    *           EFalse - If dialog was deleted.
    */    
    TBool DisplayQueryDialogWithHeaderL( 
        TInt aDialogResourceId,
        TInt& aUserResponse );
    
private:    // from CActive

    /**
    * Handle active object completion event, used to handle internal trigger in our 
    * plug-in
    */
    void RunL();
    
    /**
    * Handle when RunL leaves
    */
    TInt RunError( TInt aError );
    
    /**
    * Handle cancel
    */
    void DoCancel();
    
private: // data

    /**
    * Reference of the callback interface between the plug-in class
    * and the engine class.
    */
    MDiagBatteryChargerTestCallBackInterface& iInterface;
    
    
    /**
    * Wrapper to display waiting dialog. This helps to identify if the dialog
    * was dismissed by end-user or by object deletion.
    * Ownership: This class.
    */
    CDiagPluginWaitingDialogWrapper* iWaitingDialogWrapper;
    
    /**
    *   This flag indicates if the plugin is run in single test execution mode.
    */
    TBool iIsSinglePluginExecution;
    
    };

#endif //DIAGBATTERYCHARGERCABLETESTENGINE_H

// End of File
