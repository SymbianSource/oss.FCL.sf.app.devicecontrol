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
* Description:  This is the implementation of the Top-Level Diagnostics
*                Suite, which is used to contain all tests and suites.
*
*/


// System Include Files
#include <DiagSuiteObserver.h>              // MDiagSuiteObserver
#include <DiagSuiteExecParam.h>             // TDiagSuiteExecParam
#include <DiagFrameworkDebug.h>             // Debugging Macros
#include <DiagEngineCommon.h>               // MDiagEngineCommon
#include <StringLoader.h>                   // StringLoader
#include <devdiagtoplevelsuitepluginrsc.rsg>    // Resource Definitions

// User Include Files
#include "diagtoplevelsuiteplugin.h"        // CDiagTopLevelSuitePlugin
#include "diagtoplevelsuiteplugin.hrh"      // UID definition
#include "diagtoplevelsuiteplugin.pan"      // Panic

// Local Constants
_LIT( KDiagTopLevelSuitePluginResourceFileName,
      "z:devdiagtoplevelsuitepluginrsc.rsc" );
const TUid KDiagTopLevelSuitePluginUid = { _UID3 };


// ============================ GLOBAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Static two-phase constructor.
// ---------------------------------------------------------------------------
//
MDiagPlugin* CDiagTopLevelSuitePlugin::NewL( TAny* aInitParams )
    {
    __ASSERT_ALWAYS( aInitParams,
                     Panic( EDiagTopLevelSuitePluginConstruction ) );

    // Construct the plugin.  The base class will take ownership of the
    // initialization parameters.
    CDiagPluginConstructionParam* param =
            static_cast< CDiagPluginConstructionParam* >( aInitParams );

    CleanupStack::PushL( param );
    CDiagTopLevelSuitePlugin* self =
        new( ELeave ) CDiagTopLevelSuitePlugin( param );
    CleanupStack::Pop( param );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CDiagTopLevelSuitePlugin::~CDiagTopLevelSuitePlugin()
    {
    }

// ---------------------------------------------------------------------------
// The default constructor.
// ---------------------------------------------------------------------------
//
CDiagTopLevelSuitePlugin::CDiagTopLevelSuitePlugin(
    CDiagPluginConstructionParam* aParam )
:   CDiagSuitePluginBase( aParam )
    {
    }

// ---------------------------------------------------------------------------
// The second phase constructor.
// ---------------------------------------------------------------------------
//
void CDiagTopLevelSuitePlugin::ConstructL()
    {
    LOGSTRING( "CDiagTopLevelSuitePlugin::ConstructL");
    BaseConstructL ( KDiagTopLevelSuitePluginResourceFileName );
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns whether the plugin should be displayed or not.
// ---------------------------------------------------------------------------
TBool CDiagTopLevelSuitePlugin::IsVisible() const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin name.
// ---------------------------------------------------------------------------
HBufC* CDiagTopLevelSuitePlugin::GetPluginNameL(
    TNameLayoutType aLayoutType ) const
    {
    switch ( aLayoutType )
        {
        case ENameLayoutListLargeGraphic:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_LIST_LARGE_GRAPHIC );

        case ENameLayoutHeadingPane:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_HEADING_PANE );

        case ENameLayoutPopupInfoPane:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_POPUP_INFO_PANE );

        case ENameLayoutTitlePane:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_TITLE_PANE );

#if _DEBUG
        /* These two layouts are only available in debug builds, to allow for
         * tracing the plugin's name.  The top-level suite does not support
         * them for released builds, because it should not be used in either
         * of these layouts.
         */

        case ENameLayoutListSingleGraphic:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_LIST_SINGLE_GRAPHIC );

        case ENameLayoutListSingle:
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_LIST_SINGLE );

#endif // _DEBUG

        default:
            __ASSERT_DEBUG( EFalse,
                            Panic( EDiagTopLevelSuitePluginBadArgument ) );
            return StringLoader::LoadL( R_DIAG_TOP_LEVEL_SUITE_LIST_LARGE_GRAPHIC );
        }
    }

// ---------------------------------------------------------------------------
// From class MDiagPlugin.
// Returns the plugin's UID.
// ---------------------------------------------------------------------------
TUid CDiagTopLevelSuitePlugin::Uid() const
    {
    return KDiagTopLevelSuitePluginUid;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object run handler.
// ---------------------------------------------------------------------------
void CDiagTopLevelSuitePlugin::RunL()
    {
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Active object cancel.
// ---------------------------------------------------------------------------
void CDiagTopLevelSuitePlugin::DoCancel()
    {
    }

// End of File
