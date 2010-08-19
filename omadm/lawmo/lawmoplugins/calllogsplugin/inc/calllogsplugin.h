/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Callogs plug-in for LAWMO
*
*/

#ifndef _CALLLOGSPLUGIN__
#define _CALLLOGSPLUGIN__

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

#include <f32file.h>
#include <flogger.h>
#include <lawmointerface.h>
#include <lawmocallbackinterface.h>
#include "lawmocalllogsasyncwipe.h"

class CLAWMOCallLogsPlugin: public CLAWMOPluginInterface
    {
public:
    // Standardised safe construction which leaves nothing the cleanup stack.
    static CLAWMOCallLogsPlugin* NewL(MLawmoPluginWipeObserver* aObserver);
    // Destructor   
    ~CLAWMOCallLogsPlugin();
    
    /*
    * Description: Calls Aynchronous object for wiping call-logs.
    */
    void WipeL ();    
private:
    // Construction
    CLAWMOCallLogsPlugin();

    /**
     * second phase constructor
     */
    void ConstructL(MLawmoPluginWipeObserver* aObserver); 
private:
    CActiveWipeObject *iAsyncOp;
    };

#endif // _CALLLOGSPLUGIN__
