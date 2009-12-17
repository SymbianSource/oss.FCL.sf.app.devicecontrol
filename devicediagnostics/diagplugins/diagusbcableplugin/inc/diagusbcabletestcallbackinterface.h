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
* Description:  This class is an abstract interface used by the USB cable test
*                engine class (CDiagUSBCableTestEngine) to report result to the 
*                plug-in class (CDiagUSBCableTestPlugin)    
*
*/


#ifndef DIAGUSBCALLBACKINTERFACE_H
#define DIAGUSBCALLBACKINTERFACE_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>

// CLASS DECLARATION

/**
* MDiagUSBTestCallBackInterface
*
* This is an abstract interface class used by the USB cable test
* engine class (CDiagUSBCableTestEngine) to report result of the test
* to the plug-in class (CDiagUSBCableTestPlugin) and invoke few other callback functions
*/
 
class MDiagUSBTestCallBackInterface
    {
public:
    /**
    * The engine class uses this callback interface to report result back to the plugin
    * class
    */
    virtual void ReportTestResultL(CDiagResultsDatabaseItem::TResult aResult) = 0;
    
    /**
    * The engine class uses this callback on the plugin class to ask the user if the entire test
    * execution needs to be cancelled.
    * @param aUserResponse : The user response is returned in this reference argument.
    *      
    * @return ETrue     : The dialog was dismissed by the user.
    *         EFalse    : The dialog was dismissed internally.
    */    
    virtual TBool AskCancelExecutionL(TInt& aUserResponse,TInt aOption) = 0;  
     
    };    

#endif // DIAGUSBCALLBACKINTERFACE_H
