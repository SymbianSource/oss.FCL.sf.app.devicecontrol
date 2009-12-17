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
* Description:  This is the header for the Diag Headset Cable Test Plug-in
*
*/


#ifndef DIAGHEADSETCABLETESTPLUGIN_H 
#define DIAGHEADSETCABLETESTPLUGIN_H

// INCLUDES
#include <DiagResultsDatabaseItem.h>
#include <DiagTestPluginBase.h>     // CDiagTestPluginBase
#include "diagheadsetcabletestcallbackinterface.h"
#include "diagheadsetcableengine.h"

//CONSTANTS


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CDiagResultsDbItemBuilder;
class CDiagHeadsetCableTestEngine;

// CLASS DEFINITION
class CDiagHeadsetCableTestPlugin : public CDiagTestPluginBase, 
                                    public MDiagHeadsetTestCallBackInterface
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
    ~CDiagHeadsetCableTestPlugin();
        
        
private: // from MDiagHeadsetTestCallBackInterface        

    /**
    *  The Headset Cable Test PDP Engine calls this function to provide the result of the test
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

public: // From CDiagTestPluginBase

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
    
    
    // ADO & Platformization Changes
    TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );
        



protected: // from CDiagTestPluginBase

    /**
    * C++ default constructor.
    */
    CDiagHeadsetCableTestPlugin( CDiagPluginConstructionParam* aInitParams  );

    /**
    * Symbian OS default constructor.
    *
    */
    void ConstructL();

    /**
    * Handle plug-in execution.
    *   @see CDiagTestPluginBase::DoRunTestL
    */
    void DoRunTestL();

    /**
    * Handle clean up.
    *   @see CDiagTestPluginBase::DoStopAndCleanupL
    */
    void DoStopAndCleanupL();

                        
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
    CDiagHeadsetCableTestEngine *iEngine;
    };

#endif //DIAGHEADSETCABLETESTPLUGIN_H

// End of File
