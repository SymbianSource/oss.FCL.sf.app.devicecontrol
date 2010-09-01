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
* Description:  This is the declaration of the Coverage Suite Plugin class.
*
*/


#ifndef DIAGCOVERAGESUITEPLUGIN_H
#define DIAGCOVERAGESUITEPLUGIN_H

// System Include Files
#include <DiagSuitePluginBase.h>            // CDiagSuitePluginBase

/**
 *  Diagnostics Coverage Suite Plugin
 *  This class defines the coverage suite plugin.  This plugin is the container
 *  for all test plugins which test coverage functionality.
 *
 *  @lib diagpluginbase.lib
 *  @lib diagframework.lib
 *  @lib commonengine.lib
 *  @lib aknicon.lib
 *  @lib egul.lib
 */
class CDiagCoverageSuitePlugin : public CDiagSuitePluginBase
    {

public: // New functions

    /**
     * Two-phased constructor.
     *
     * @return An instance of CDiagCoverageSuitePlugin.
     */
    static MDiagPlugin* NewL( TAny* aInitParams );

    /**
    * Destructor.
    */
    ~CDiagCoverageSuitePlugin();


private: // New functions

    /**
     * The default constructor.
     */
    CDiagCoverageSuitePlugin( CDiagPluginConstructionParam* aInitParams  );

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

    /**
    * @see CDiagSuitePluginBase::CreateIconL
    */
    virtual CGulIcon* CreateIconL() const;


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

#endif // DIAGCOVERAGESUITEPLUGIN_H

