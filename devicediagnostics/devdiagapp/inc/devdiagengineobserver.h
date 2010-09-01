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
* Description:  This is the observer class for the Device Diagnostics 
*                Application's Engine class.  It is used to propagate engine 
*                events to UI elements.
*
*/


#ifndef DEVDIAGENGINEOBSERVER_H
#define DEVDIAGENGINEOBSERVER_H

/**
 *  The observer class for the application engine.
 *
 *  This class is an abstract interface which is used by the application
 *  engine to pass events from itself and the diagnostics framework to the UI
 *  components.
 */
class MDevDiagEngineObserver
    {

public: // Data Types

    /**  An enum which identifies different engine events. */
    enum TAppEngineCommand
        {
        EDevDiagEngineCommandPluginLoadProgress,
        EDevDiagEngineCommandPluginLoadComplete,
        EDevDiagEngineCommandRunTimeReqs,
        EDevDiagEngineCommandTestExecutionBegin,
        EDevDiagEngineCommandProgressDataUpdated,
        EDevDiagEngineCommandResultsDataUpdated,
        EDevDiagEngineCommandSinglePluginExecutionDone,
        EDevDiagEngineCommandGroupExecutionDone,
        EDevDiagEngineCommandGroupExecutionProgress,
        EDevDiagEngineCommandTestExecutionSuspended,
        EDevDiagEngineCommandTestExecutionCancelled,
        EDevDiagEngineCommandTestExecutionResumed,
        EDevDiagEngineCommandObserverChanged,
        EDevDiagEngineCommandGetLastResults,
        EDevDiagEngineCommandViewSwitch,
        EDevDiagEngineCommandExecutionStopping
        };


public: // Pure Virtual Functions

    /**
     * The observer interface to inform of engine events.
     *
     * @param aCommand The engine event.
     * @param aError Any error code related to the event.
     * @param aCustom Any data related to the event.  Ownership is not
     *                transferred.  The format of the data is determined by
     *                the event type.
     */
    virtual void HandleEngineCommandL( TAppEngineCommand aCommand,
                                       TInt aError,
                                       TAny* aCustom ) = 0;
    };

#endif // DEVDIAGENGINEOBSERVER_H
