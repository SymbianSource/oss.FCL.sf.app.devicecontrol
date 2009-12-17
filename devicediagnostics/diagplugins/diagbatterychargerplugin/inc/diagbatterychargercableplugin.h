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
* Description:  This is the header file for Diagnostics Battery Charger Cable Test Plug-in
*
*/


#ifndef DIAGBATTERYCHARGERCABLETESTPLUGIN_H 
#define DIAGBATTERYCHARGERCABLETESTPLUGIN_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>
#include <DiagTestPluginBase.h>
#include "diagbatterychargercallbackinterface.h"

//CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDiagResultsDbItemBuilder;
class CDiagBatteryChargerCableTestEngine;


// CLASS DEFINITION

/**
* Diagnostics Battery Charger Cable Test Plug-in
* This class is the main plug-in class for the Battery charger cable test plug-in
*/
class CDiagBatteryChargerCableTestPlugin : public CDiagTestPluginBase, 
                                           public MDiagBatteryChargerTestCallBackInterface
    {
public: // Constructors and destructor

    /**
    * Symbian OS two-phased constructor
    * @return DataNetwork Test Plugin
    */
    static MDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor
    */
    ~CDiagBatteryChargerCableTestPlugin();
    
private: // from MDiagBatteryChargerTestCallBackInterface
    
    /**
    * The engine class uses this interface to inform the plug-in class about the test result.
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
    TBool AskCancelExecutionL(TInt& aUserResponse);
    
    // ADO & Platformization Changes
    TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );

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

private: // New functions

    /**
    * C++ default constructor.
    */
    CDiagBatteryChargerCableTestPlugin( CDiagPluginConstructionParam* aInitParams );

    /**
    * Symbian OS second phase constructor.
    *
    */
    void ConstructL();

protected:    // from CActive
    /**
    * Handle active object completion event
    */
    void RunL();

    /**
    * Handle cancel
    */
    void DoCancel();

private:    // data

    /** Instance of the engine class for this test. Own */ 
    CDiagBatteryChargerCableTestEngine *iEngine;
    };

#endif //DIAGBATTERYCHARGERCABLETESTPLUGIN_H

// End of File

