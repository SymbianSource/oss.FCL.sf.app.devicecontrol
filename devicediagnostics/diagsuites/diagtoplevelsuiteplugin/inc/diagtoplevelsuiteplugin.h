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
* Description:  This is the declaration of the Top-Level Suite Plugin class.
*
*/


#ifndef DIAGTOPLEVELSUITEPLUGIN_H
#define DIAGTOPLEVELSUITEPLUGIN_H

// System Include Files
#include <DiagSuitePluginBase.h>            // CDiagSuitePluginBase

/**
 *  Diagnostics Top-Level Suite Plugin
 *  This class defines the top-level suite plugin.  This plugin is container for
 *  all of the suites and plugins which belong to the highest level in the test
 *  hierarchy.
 *
 *  @lib diagpluginbase.lib
 *  @lib diagframework.lib
 *  @lib commonengine.lib
 */
class CDiagTopLevelSuitePlugin : public CDiagSuitePluginBase
    {

public: // New functions

    /**
     * Two-phased constructor.
     *
     * @return An instance of CDiagTopLevelSuitePlugin.
     */
    static MDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor.
    */
    ~CDiagTopLevelSuitePlugin();


private: // New functions

    /**
     * The default constructor.
     */
    CDiagTopLevelSuitePlugin( CDiagPluginConstructionParam* aInitParams  );

    /**
     * Two-phased constructor.
     */
    void ConstructL();


private: // From base class CDiagSuitePluginBase

    /**
    * @see CDiagSuitePluginBase::Visible()
    */
    virtual TBool IsVisible() const;

    /**
    * @see CDiagSuitePluginBase::GetPluginNameL
    */
    virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const;

    /**
    * @see CDiagSuitePluginBase::Uid
    */
    virtual TUid Uid() const;


private: // From base class CActive
    /**
    * Handle active object completion event
    */
    virtual void RunL();

    /**
    * Handle cancel
    */
    virtual void DoCancel();

    };

#endif // DIAGTOPLEVELSUITEPLUGIN_H
