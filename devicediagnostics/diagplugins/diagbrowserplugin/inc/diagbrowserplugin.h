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
* Description:  This is the declaration of the Browser Test Plugin class.
*
*/


#ifndef DIAGBROWSERPLUGIN_H
#define DIAGBROWSERPLUGIN_H

// System Include Files
#include <DiagTestPluginBase.h>             // CDiagTestPluginBase

// Local Include Files
#include "diagbrowserpluginmodelobserver.h" // MDiagBrowserPluginModelObserver

// Forward Declarations
class CDiagBrowserPluginModel;

/**
 *  Diagnostics Browser Test Plugin
 *  This class defines the browser test plugin.  This plugin is used to
 *  validate end-to-end browser (HTTP) functionality.
 *
 *  @lib diagpluginbase.lib
 *  @lib diagframework.lib
 *  @lib commonengine.lib
 */
class CDiagBrowserPlugin : public CDiagTestPluginBase,
                           public MDiagBrowserPluginModelObserver
    {

public: // New functions

    /**
    * Two-phased constructor.
    *
    * @return An instance of CDiagBrowserPlugin.
    */
    static MDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor.
    */
    ~CDiagBrowserPlugin();
    
    
    // ADO & Platformization Changes
    TBool ShowMessageQueryL( TInt aResourceId, TInt &aButtonId  );



private: // New functions

    /**
    * The default constructor.
    */
    CDiagBrowserPlugin( CDiagPluginConstructionParam* aInitParams );

    /**
    * Two-phased constructor.
    */
    void ConstructL();


private: // From base class CDiagTestPluginBase

    /**
    * @see CDiagTestPluginBase::Visible()
    */
    virtual TBool IsVisible() const;

    /**
    * @see CDiagTestPluginBase::RunMode()
    */
    virtual TRunMode RunMode() const;

    /**
    * @see CDiagTestPluginBase::TotalSteps()
    */
    virtual TUint TotalSteps() const;

    /**
    * @see CDiagTestPluginBase::GetPluginNameL
    */
    virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

    /**
    * @see CDiagTestPluginBase::Uid
    */
    virtual TUid Uid() const;

    /**
    * @see CDiagTestPluginBase::DoRunTestL
    */
    virtual void DoRunTestL();

    /**
    * @see CDiagTestPluginBase::DoStopAndCleanupL
    */
    virtual void DoStopAndCleanupL();


private: // From base class CActive

    /**
    * Handles active object completion events.
    */
    virtual void RunL();

    /**
    * Handle active object cancel event.
    */
    virtual void DoCancel();


private: // From base class MDiagBrowserPluginModelObserver

    /**
    * This function notifies that test execution has completed.
    *
    * @param aResult The test result.
    */
    virtual void TestEndL( CDiagResultsDatabaseItem::TResult aResult );

    /**
    * This function notifies of test progress.
    *
    * @param aCurrentStep The current execution step.
    */
    void TestProgressL( TUint aCurrentStep );


private: // Data

    /** The plugin's model class, which contains the testing logic.  Own. */
    CDiagBrowserPluginModel* iModel;

    };

#endif // DIAGBROWSERPLUGIN_H

