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
* Description:  This is the header for the Diag Headset Cable Test Engine
*
*/


#ifndef DIAGHEADSETCABLETESTENGINE_H 
#define DIAGHEADSETCABLETESTENGINE_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>                    // CDiagResultsDatabaseItem
#include "diagheadsetcabletestcallbackinterface.h"      // MDiagHeadsetTestCallBackInterface


//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagPluginWaitingDialogWrapper;

// CLASS DEFINITION    
class CDiagHeadsetCableTestEngine : public CActive
    {
public: //constructor and destructor
    /**
    * Symbian OS two phase constructor
    */
    static CDiagHeadsetCableTestEngine* NewL(MDiagHeadsetTestCallBackInterface& aInterface,
                                            TBool aIsSinglePluginExecution);
    
    /**
    * Virtual
    */
    ~CDiagHeadsetCableTestEngine();
    
public: // external interfaces
    
    /**
    * The plug-in class starts the Headset cable test through this function
    */
    void RunHeadsetCableTestL();

private: // private constructor
    
    /**
    * C++ constructor
    **/
    CDiagHeadsetCableTestEngine(MDiagHeadsetTestCallBackInterface& aInterface,
                                TBool aIsSinglePluginExecution);
    
    /**
    * Symbian second phase constructor
    */
    void ConstructL();

private: // internal functions
    
    /**
    * This is an internal function to end test execution and report result
    * to the plug-in class
    */
    void ReportResultToPluginL(CDiagResultsDatabaseItem::TResult aResult);
    
    
    /**
    * This function is used to detect the Headset cable status
    * @return ETrue : The Headset cable is detected, EFalse otherwise
    */
    TBool CheckIfHeadsetDetectedL();


    /**
    *   Ask user to connect charger and press OK
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
    *   Displays a query dialog with header.
    *   @param aDialogResourceId - Resource ID of the dialog to display
    *   @param aQueryResourceId - Resource ID of the text query.
    *   @param aUserResponse - Dialog response will be returned via this parameter.
    *   @return ETrue - If dialog was dismissed by end user.
    *       EFalse - If dialog was deleted.
    */
    TBool CDiagHeadsetCableTestEngine::DisplayQueryDialogWithHeaderL( 
            TInt aDialogResourceId,
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
        
private: // private data

    /**
    * A reference to the callback interface between the plug-in class and the engine class.
    */
    MDiagHeadsetTestCallBackInterface& iInterface;
    
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

#endif //DIAGHEADSETCABLETESTENGINE_H

// End of File
