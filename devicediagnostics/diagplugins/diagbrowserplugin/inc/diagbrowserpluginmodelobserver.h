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
* Description:  This is the header for the Browser Test Model Observer
*                 class, which the Browser Test plugin implements to receive
*                 events from the model.
*
*/


#ifndef DIAGBROWSERPLUGINMODELOBSERVER_H
#define DIAGBROWSERPLUGINMODELOBSERVER_H

// System Include Files
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem

/**
*  Browser Test Model Observer
*   
*   This is a pure virtual class that the Browser Test Plugin must implement
*   to receive callbacks from the model class.
*/
class MDiagBrowserPluginModelObserver
    {
public:

    /**
    * This function notifies that test execution has completed.
    *
    * @param aResult The test result.
    */
    virtual void TestEndL( CDiagResultsDatabaseItem::TResult aResult ) = 0;

    /**
    * This function notifies of test progress.
    *
    * @param aCurrentStep The current execution step.
    */
    virtual void TestProgressL( TUint aCurrentStep ) = 0;

    };

#endif // DIAGBROWSERPLUGINMODELOBSERVER_H
