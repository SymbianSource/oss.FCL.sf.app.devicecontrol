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
* Description:  This class is an abstract interface used by the Headset cable test
*                engine class (CDiagHeadsetCableTestEngine) to report result to the 
*                plug-in class (CDiagHeadsetCableTestPlugin)    
*
*/


#ifndef DIAGHEADSETCALLBACKINTERFACE_H
#define DIAGHEADSETCALLBACKINTERFACE_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>

// CLASS DECLARATION

/**
* MDiagHeadsetTestCallBackInterface
*
* This is an abstract interface class used by the Headset cable test
* engine class (CDiagHeadsetCableTestEngine) to report result of the test
* to the plug-in class (CDiagHeadsetCableTestPlugin).
*/
class MDiagHeadsetTestCallBackInterface
    {
public:
    /**
    * This is a callback from the engine class to report the result of the test
    * to the plug-in class
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
    virtual TBool AskCancelExecutionL(TInt& aUserResponse) = 0;    
    };

#endif // DIAGHEADSETCALLBACKINTERFACE_H
