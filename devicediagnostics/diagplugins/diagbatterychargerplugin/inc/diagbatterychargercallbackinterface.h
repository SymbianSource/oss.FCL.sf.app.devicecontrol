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
* Description:  This class is an abstract interface used by the Battery charger cable test
*                engine class (CDiagBatteryChargerCableTestEngine) to report result to the 
*                plug-in class (CDiagBatteryChargerCableTestPlugin)    
*
*/


#ifndef DIAGBATTERYCHARGERCALLBACKINTERFACE_H
#define DIAGBATTERYCHARGERCALLBACKINTERFACE_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>

// CLASS DECLARATION

/**
* MDiagBatteryChargerTestCallBackInterface
*
* This is an abstract interface class used by the Battery charger cable test
* engine class (CDiagBatteryChargerCableTestEngine) to report result of the test
* to the plug-in class (CDiagBatteryChargerCableTestPlugin).
*/
class MDiagBatteryChargerTestCallBackInterface
    {
public:
    /**
    * The engine class uses this interface to inform the plug-in class about the test result.
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
    

#endif // DIAGBATTERYCHARGERCALLBACKINTERFACE_H
